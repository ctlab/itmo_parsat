#include "core/sat/prop/ParProp.h"

#include <filesystem>
#include <mutex>

#include "core/util/stream.h"

namespace {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void _wait_for_futures(std::vector<std::future<void>>& futures) noexcept {
  for (auto& future : futures) {
    future.get();
  }
}

}  // namespace

namespace core::sat::prop {

ParProp::ParProp(ParPropConfig const& config) {
  for (uint32_t i = 0; i < config.max_threads(); ++i) {
    _solvers.emplace_back(PropRegistry::resolve(config.prop_config()));
  }
  auto& parent_generator = core::Generator::current_thread_generator();

  for (uint32_t tid = 0; tid < config.max_threads(); ++tid) {
    _t.emplace_back([this, tid, &parent_generator] {
      auto& solver = *_solvers[tid];
      core::Generator child_generator(parent_generator);

      while (!_stop) {
        std::unique_lock<std::mutex> ul(_m);
        _cv.wait(ul, [this] { return _stop || !_task_queue.empty(); });
        if (IPS_UNLIKELY(_stop)) {
          break;
        }
        if (IPS_UNLIKELY(_task_queue.empty())) {
          continue;
        }

        auto task = std::move(_task_queue.front());
        _task_queue.pop();
        ul.unlock();
        task(solver);
      }
    });
  }
}

ParProp::~ParProp() noexcept {
  {
    std::lock_guard<std::mutex> lg(_m);
    _stop = true;
  }
  _cv.notify_all();

  for (auto& t : _t) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void ParProp::parse_cnf(std::filesystem::path const& input) {
  for (auto& solver : _solvers) {
    solver->parse_cnf(input);
  }
}

bool ParProp::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  return _solvers.front()->propagate(assumptions, propagated);
}

void ParProp::prop_assignments(
    domain::USearch assignment_p, Prop::prop_callback_t const& callback) {
  uint32_t num_threads = _t.size();

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(
        _submit(req_prop_t{assignment_p->split_search(num_threads, index), callback}));
  }
  _wait_for_futures(futures);
}

uint64_t ParProp::prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) {
  if (vars.size() - head_size <= 16) {
    // Use single thread to process small requests.
    return _solvers.front()->prop_tree(vars, head_size);
  }

  // We use the number of threads as the highest power of 2, lower than pool size.
  uint32_t max_threads = 1;
  while (max_threads * 2 < _t.size()) {
    max_threads <<= 1;
  }

  // first hs vars are fixed, vl = |vars| - hs are left
  // we choose new head size, hs' <= vl
  // the number of threads will be 2^{hs'}
  // threads <= max_threads
  // threads = 2^{hs'} <= 2^{vl}
  uint32_t vars_left = vars.size() - head_size;
  uint32_t threads = std::min(uint32_t(1) << vars_left, max_threads);
  uint32_t new_head_size = std::log2(threads);

  std::atomic_uint64_t result = 0;
  std::vector<std::future<void>> futures;
  for (uint32_t thread = 0; thread < threads; ++thread) {
    futures.push_back(_submit(req_prop_full_t{
        /* vars = */ vars,
        /* base_head = */ head_size,
        /* head_size = */ new_head_size,
        /* head_asgn = */ thread,
        /* conflicts& */ result}));
  }
  _wait_for_futures(futures);
  return result;
}

void ParProp::_propagate(sat::prop::Prop& solver, req_prop_t& req) {
  START_ASGN_TRACK(req.assignment->size());
  if (!req.assignment->empty()) {
    auto& assignment = *req.assignment;
    do {
      ASGN_TRACK(assignment());
      bool result = solver.propagate(assignment());
      if (!req.callback(result, assignment())) {
        BREAK_ASGN_TRACK;
        break;
      }
    } while (++assignment);
  }
  END_ASGN_TRACK(false);
}

std::future<void> ParProp::_submit(task_t&& task) {
  // clang-format off
  auto p_task = std::packaged_task<void(sat::prop::Prop&)>(
    [this, task = std::move(task)] (sat::prop::Prop& solver) mutable {
      std::visit(overloaded{
          [&, this](req_prop_t& req) {
            _propagate(solver, req);
          },
          [&](req_prop_full_t& req) {
            auto& asgn = req.vars;
            uint32_t base_head = req.base_head;
            uint32_t head_size = req.head_size;
            uint64_t head_asgn = req.head_asgn;
            for (uint32_t i = 0; i < head_size; ++i) {
              asgn[base_head + i] = Minisat::mkLit(Minisat::var(asgn[base_head + i]), head_asgn & (1ULL << i));
            }
            req.conflicts += solver.prop_tree(asgn, base_head + head_size);
          }
      }, task);
  });
  // clang-format on
  auto future = p_task.get_future();
  {
    std::lock_guard<std::mutex> lg(_m);
    _task_queue.emplace(std::move(p_task));
  }
  _cv.notify_one();
  return future;
}

unsigned ParProp::num_vars() const noexcept {
  return _solvers.front()->num_vars();
}

REGISTER_PROTO(Prop, ParProp, par_prop_config);

}  // namespace core::sat