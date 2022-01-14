#ifndef ITMO_PARSAT_VARVIEW_H
#define ITMO_PARSAT_VARVIEW_H

#include <unordered_map>

namespace core::domain {

/**
 * @brief The class representing variable mapping. Is used to compactly represent
 * mapping from a chosen set of variables, where the rest are usually ignored.
 */
class VarView {
 public:
  /**
   * @brief Constructs empty mapping.
   */
  VarView() = default;

  /**
   * @brief Maps the specified index to the specified variable.
   * @param index an identifier by which the variable will be represented.
   * @param var the variable.
   */
  void map_var(int index, int var);

  /**
   * @param index an identifier of a variable.
   * @return variable.
   * @throw std::out_of_range exception if index is not mapped to any variable.
   */
  int operator[](int index) const noexcept;

  /**
   * @return The size of mapping, i.e. the number of distinct keys.
   */
  [[nodiscard]] size_t size() const noexcept;

 private:
  std::unordered_map<int, int> _map;
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_VARVIEW_H
