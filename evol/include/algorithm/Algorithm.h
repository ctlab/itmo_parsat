#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include <utility>

#include "evol/include/domain/Instance.h"
#include "evol/include/limit/Limit.h"
#include "evol/include/method/Selector.h"
#include "evol/include/util/profile.h"
#include "evol/proto/config.pb.h"

namespace ea::algorithm {

class Algorithm {
 public:
  virtual ~Algorithm() = default;

  Algorithm();

  virtual void prepare();

  explicit Algorithm(AlgorithmConfig const& config);

  void process();

  void interrupt();

  sat::Solver& get_solver() noexcept;

  instance::Population& get_population() noexcept;

  instance::Instance& get_best() noexcept;

 protected:
  virtual void step() = 0;

  [[nodiscard]] bool is_interrupted() const;

 protected:
  instance::Population population_;
  sat::RSolver solver_;

 private:
  limit::RLimit limit_;
  bool interrupted_ = false;
};

using RAlgorithm = std::shared_ptr<Algorithm>;

}  // namespace ea::algorithm

#endif  // EA_BASE_ALGORITHM_H
