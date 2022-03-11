#ifndef ITMO_PARSAT_CARTESIANSEARCH_H
#define ITMO_PARSAT_CARTESIANSEARCH_H

#include <vector>
#include <numeric>
#include <algorithm>

#include "core/domain/assignment/Search.h"
#include "util/Logger.h"

namespace core::domain {

class CartesianSearch;
MAKE_REFS(CartesianSearch);

/**
 * @brief The class used to search through the cartesian product of custom searches.
 */
class CartesianSearch : public Search {
 public:
  explicit CartesianSearch(std::vector<std::vector<std::vector<Mini::Lit>>>&& cartesian);

  explicit CartesianSearch(std::vector<std::vector<std::vector<Mini::Lit>>> const& cartesian);

  Mini::vec<Mini::Lit> const& operator()() const override;

  [[nodiscard]] Search* clone() const override;

 protected:
  void _advance() override;

  void _set_cur(uint32_t from);

  void _reset() override;

 private:
  std::vector<uint32_t> _indices;
  Mini::vec<Mini::Lit> _assignment;
  std::vector<std::vector<std::vector<Mini::Lit>>> _cartesian;
};

UCartesianSearch createCartesianSearch(
    std::vector<std::vector<std::vector<Mini::Lit>>>&& cartesian);

UCartesianSearch createCartesianSearch(
    std::vector<std::vector<std::vector<Mini::Lit>>> const& cartesian);

}  // namespace core::domain

#endif  // ITMO_PARSAT_CARTESIANSEARCH_H
