#ifndef ITMO_PARSAT_CARTESIANSEARCH_H
#define ITMO_PARSAT_CARTESIANSEARCH_H

#include <vector>
#include <numeric>
#include <algorithm>

#include "core/search/Search.h"
#include "util/Logger.h"

namespace core::search {

/**
 * @brief The class used to search through the cartesian product of custom
 * searches.
 */
class CartesianSearch : public Search {
 public:
  using cartesian_set_t = std::vector<std::vector<std::vector<Mini::Lit>>>;

 private:
  explicit CartesianSearch(
      std::shared_ptr<cartesian_set_t> cartesian_set);

 public:
  explicit CartesianSearch(cartesian_set_t&& cartesian_set);

  explicit CartesianSearch(cartesian_set_t const& cartesian_set);

  [[nodiscard]] Search* clone() const override;

 protected:
  void _advance() override;

  void _set_cur(uint32_t from);

  void _reset() override;

 private:
  std::vector<uint32_t> _indices;
  std::shared_ptr<cartesian_set_t> _r_cartesian_set;
};

MAKE_REFS(CartesianSearch);

UCartesianSearch createCartesianSearch(
    CartesianSearch::cartesian_set_t&& cartesian_set);

UCartesianSearch createCartesianSearch(
    CartesianSearch::cartesian_set_t const& cartesian_set);

}  // namespace core::search

#endif  // ITMO_PARSAT_CARTESIANSEARCH_H
