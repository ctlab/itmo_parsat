#ifndef EVOL_ASSIGNMENT_H
#define EVOL_ASSIGNMENT_H

#include <vector>
#include <set>
#include <map>
#include <memory>
#include <optional>
#include <minisat/mtl/Vec.h>
#include <minisat/core/SolverTypes.h>

#include "evol/include/domain/VarView.h"

namespace ea::domain {

class Assignment;

using UAssignment = std::unique_ptr<Assignment>;

class Assignment {
 public:
  static constexpr int MAX_VARS_FULL_SEARCH = 63;

 protected:
  explicit Assignment(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

 public:
  virtual ~Assignment() = default;

  /**
   *
   * @param num_split Number of searches this search will be split to.
   * @param index Index of current search in range [0, num_split).
   * @return new search
   */
  [[nodiscard]] virtual UAssignment split_search(uint64_t num_split, uint64_t index) const = 0;

  /**
   * @return the current assignment.
   */
  Minisat::vec<Minisat::Lit> const& operator()() const;

  /**
   * Steps to the next search
   *
   * Intended to be used as follows:
   *
   * // assignment is defined
   * do {
   *   // use assignment
   * } while (++assignment);
   *
   * @return true iff state is valid and operator() can be applied.
   */
  [[nodiscard]] bool operator++();

  [[nodiscard]] bool is_empty() const;

 protected:
  virtual void _advance() = 0;

 protected:
  Minisat::vec<Minisat::Lit> assignment_;
  uint64_t total_, done_ = 0;
};

class SmallSearch : public Assignment {
 private:
  uint64_t _set_range();

 protected:
  SmallSearch(VarView const& var_view, std::vector<bool> const& vars);

  SmallSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

 public:
  [[nodiscard]] UAssignment split_search(uint64_t num_split, uint64_t index) const override;

 protected:
  [[nodiscard]] virtual SmallSearch* clone() const = 0;

  virtual void _reset() = 0;

 protected:
  uint64_t first_ = 0, last_ = 0;
};

class RandomSearch : public Assignment {
  friend UAssignment createRandomSearch(
      VarView const& var_view, std::vector<bool> const&, uint64_t);

 protected:
  explicit RandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

 public:
  [[nodiscard]] UAssignment split_search(uint64_t num_split, uint64_t index) const override;

 protected:
  void _advance() override;
};

class FullSearch : public SmallSearch {
  friend UAssignment createFullSearch(VarView const&, std::vector<bool> const&);

 protected:
  FullSearch(VarView const& var_view, std::vector<bool> const& vars);

 protected:
  void _advance() override;

  [[nodiscard]] SmallSearch* clone() const override;

  void _reset() override;
};

class UniqueSearch : public SmallSearch {
  friend UAssignment createFullSearch(VarView const&, std::vector<bool> const&);

  friend UAssignment createRandomSearch(VarView const&, std::vector<bool> const&, uint64_t);

 public:
  explicit UniqueSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

  explicit UniqueSearch(VarView const& var_view, std::vector<bool> const& vars);

 private:
  std::set<uint64_t> visited_;

 protected:
  void _advance() override;

  void _advance_us();

  [[nodiscard]] SmallSearch* clone() const override;

  void _reset() override;
};

UAssignment createFullSearch(VarView const& var_view, std::vector<bool> const& vars);

UAssignment createRandomSearch(
    VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

}  // namespace ea::domain

#endif  // EVOL_ASSIGNMENT_H
