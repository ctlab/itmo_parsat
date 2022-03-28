#ifndef ITMO_PARSAT_RBSEARCH_H
#define ITMO_PARSAT_RBSEARCH_H

#include <variant>

#include "core/evol/instance/Instance.h"

namespace core::solve {

enum RBSReason {
  RBS_SBS_FOUND,
  RBS_INTERRUPTED,
};

using rbs_result_t = std::variant<RBSReason, domain::USearch>;

/**
 * @brief Class used to reduce SAT problems to a set of independent tasks.
 */
class RBSearch {
 public:
  RBSearch(sat::prop::RProp prop, ea::preprocess::RPreprocess preprocess);

  virtual ~RBSearch() = default;

  /**
   * @return either the reason of abortion or the search space.
   */
  virtual rbs_result_t find_rb() = 0;

  /**
   * @brief Interrupts the search.
   */
  void interrupt();

 protected:
  [[nodiscard]] bool _is_interrupted() const noexcept;

  virtual void _interrupt_impl() = 0;

  template <typename T>
  static void _interrupt(std::shared_ptr<T> shp) {
    if (shp != nullptr) {
      shp->interrupt();
    }
  }

 protected:
  bool _interrupted = false;
  sat::prop::RProp _prop;
  ea::preprocess::RPreprocess _preprocess;
};

MAKE_REFS(RBSearch);

DEFINE_REGISTRY(
    RBSearch, RBSearchConfig, rb_search, core::sat::prop::RProp,
    ea::preprocess::RPreprocess);

}  // namespace core::solve

#endif  // ITMO_PARSAT_RBSEARCH_H
