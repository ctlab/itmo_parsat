#ifndef EVOL_ASSIGNMENT_H
#define EVOL_ASSIGNMENT_H

#include <vector>
#include <map>
#include <memory>
#include <minisat/mtl/Vec.h>
#include <minisat/core/SolverTypes.h>

namespace ea::domain {

class Assignment {
 public:
  explicit Assignment(std::map<int, int> const& var_map, std::vector<bool> const& vars);

  virtual ~Assignment() = default;

  [[nodiscard]] virtual Assignment* clone() const = 0;

  Minisat::vec<Minisat::Lit> const& operator()() const;

  virtual void set(uint32_t index) = 0;

  virtual bool operator++() = 0;

  [[nodiscard]] virtual uint32_t size() const noexcept = 0;

 protected:
  Minisat::vec<Minisat::Lit> assignment_;
};

class FullSearch : public Assignment {
 public:
  explicit FullSearch(std::map<int, int> const& var_map, std::vector<bool> const& vars);

  [[nodiscard]] Assignment* clone() const override;

  void set(uint32_t index) override;

  bool operator++() override;

  [[nodiscard]] uint32_t size() const noexcept override;
};

class RandomAssignments : public Assignment {
 public:
  explicit RandomAssignments(
      std::map<int, int> const& var_map, std::vector<bool> const& vars, uint32_t total);

  [[nodiscard]] Assignment* clone() const override;

  void set(uint32_t index) override;

  bool operator++() override;

  [[nodiscard]] uint32_t size() const noexcept override;

 private:
  uint32_t total_;
  uint32_t left_;
};

using UAssignment = std::unique_ptr<Assignment>;

}  // namespace ea::domain

#endif  // EVOL_ASSIGNMENT_H
