#ifndef ITMO_PARSAT_VARVIEW_H
#define ITMO_PARSAT_VARVIEW_H

#include <unordered_map>

namespace ea::domain {

class VarView {
 public:
  VarView() = default;

  void map_var(int index, int var);

  int operator[](int index) const noexcept;

  [[nodiscard]] size_t size() const noexcept;

 private:
  std::unordered_map<int, int> map_;
};

}  // namespace ea::domain

#endif  // ITMO_PARSAT_VARVIEW_H
