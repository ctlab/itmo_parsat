#ifndef ITMO_PARSAT_VARS_H
#define ITMO_PARSAT_VARS_H

#include <vector>
#include <algorithm>
#include <cstdint>

#include "core/domain/VarView.h"

namespace core::domain {

/**
 * @brief The class representing variables set. Variables are meant as
 * keys in variable mapping core::domain::VarView.
 */
struct Vars {
 public:
  /**
   * @brief Flips the value for the specified variable index.
   * @param pos the variable index.
   */
  void flip(uint32_t pos);

  /**
   * @brief Changes the size of assignment.
   * @param size
   */
  void resize(uint32_t size);

  /**
   * @return The const reference to internal representation.
   */
  [[nodiscard]] std::vector<bool> const& get_mask() const noexcept;

  /**
   * @return The reference to internal representation.
   */
  std::vector<bool>& get_mask() noexcept;

  /**
   * @param var_view the variable mapping.
   * @return the std::vector containing all indexes for which variable is set to true.
   */
  [[nodiscard]] std::vector<int> map_to_vars(VarView const& var_view) const;

  /**
   * @return the number of variables included in variables set.
   */
  [[nodiscard]] uint32_t size() const noexcept;

 private:
  std::vector<bool> _bit_mask;
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_VARS_H
