#ifndef EVOL_PARPROP_H
#define EVOL_PARPROP_H

#include <vector>
#include <atomic>
#include <variant>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>

#include "core/util/Generator.h"
#include "core/sat/prop/Prop.h"

namespace core::sat::prop {

/**
 * @brief Parallel propagation implementation.
 */
class ParProp : public Prop {
 private:
  struct req_prop_t {
    domain::USearch assignment;
    Prop::prop_callback_t callback;
  };

  struct req_prop_full_t {
    Minisat::vec<Minisat::Lit> vars;
    uint32_t base_head;
    uint32_t head_size;
    uint64_t head_asgn;
    /* return */ std::atomic_uint64_t& conflicts;
  };

  using task_t = std::variant<req_prop_t, req_prop_full_t>;

 private:
  void _propagate(sat::prop::Prop& prop, req_prop_t& req);

  std::future<void> _submit(task_t&& task);

 public:
  explicit ParProp(ParPropConfig const& config);

  ~ParProp() noexcept;

  void parse_cnf(std::filesystem::path const& input) override;

  bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

  void prop_assignments(domain::USearch assignment, prop_callback_t const& callback) override;

  uint64_t prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

 private:
  std::vector<std::thread> _t;
  std::vector<sat::prop::RProp> _solvers;
  std::queue<std::packaged_task<void(sat::prop::Prop&)>> _task_queue;
  std::condition_variable _cv;
  std::mutex _m;
  std::atomic_bool _stop{false};
};

}  // namespace core::sat::prop

#endif  // EVOL_PARPROP_H
