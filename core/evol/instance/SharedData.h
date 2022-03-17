#ifndef ITMO_PARSAT_SHAREDDATA_H
#define ITMO_PARSAT_SHAREDDATA_H

#include <memory>

#include "core/evol/instance/Fitness.h"
#include "core/domain/VarView.h"
#include "core/domain/SearchSpace.h"
#include "util/LRUCache.h"

namespace ea::instance {
/**
 * @brief This struct is shared between all instances under the same algorithm.
 */
struct SharedData {
  /**
   * @brief Sampling configuration.
   */
  struct SamplingConfig {
    uint32_t base_samples;
    uint32_t max_scale_steps;
    double scale;
  } sampling_config{};

  /**
   * @brief Instances cache.
   */
  core::LRUCache<std::vector<bool>, Fitness> cache{};

  /**
   * @brief Search space info.
   */
  core::domain::SearchSpace search_space;

  /**
   * @brief Inner configuration parameter.
   */
  uint32_t omega_x{};

  /// @todo: documentation
  Mini::vec<Mini::Lit> base_assumption{};
};

using RSharedData = std::shared_ptr<SharedData>;

}  // namespace ea::instance

#endif  // ITMO_PARSAT_SHAREDDATA_H
