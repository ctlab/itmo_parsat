#ifndef ITMO_PARSAT_VARS_H
#define ITMO_PARSAT_VARS_H

#include <vector>
#include <map>

namespace ea::domain {

struct Vars {
 private:
  std::vector<bool> bit_mask_;

 public:
  void flip(size_t pos);

  void resize(size_t size);

  [[nodiscard]] std::vector<bool> const& get_mask() const noexcept;

  std::vector<bool>& get_mask() noexcept;

  [[nodiscard]] std::vector<int> map_to_vars(std::map<int, int> const& var_map) const;
};

}  // namespace ea::domain

#endif  // ITMO_PARSAT_VARS_H
