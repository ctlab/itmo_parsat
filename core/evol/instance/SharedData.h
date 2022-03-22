#ifndef ITMO_PARSAT_SHAREDDATA_H
#define ITMO_PARSAT_SHAREDDATA_H

#include <memory>

#include "core/types.h"
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
  core::LRUCache<core::bit_mask_t, Fitness> cache{};

  /**
   * @brief Search space info.
   */
  core::domain::SearchSpace search_space;

  /**
   * @brief Inner configuration parameter.
   */
  uint32_t omega_x{};

  /**
   * @brief The base assumption, i.e. the assumption applied
   * separately to all operations.
   */
  core::lit_vec_t base_assumption{};
};

MAKE_REFS(SharedData);

}  // namespace ea::instance

#endif  // ITMO_PARSAT_SHAREDDATA_H
