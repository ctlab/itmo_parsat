#ifndef ITMO_PARSAT_SHARING_H
#define ITMO_PARSAT_SHARING_H

#include "core/sat/sharing/Shareable.h"

namespace core::sat::sharing {

/**
 * The direction in which the clause sharing will be performed.
 */
enum SharingDir {
  RIGHT,
  LEFT,
  BIDIR,
};

/**
 * @brief The class responsible for clause sharing
 */
class Sharing {
 public:
  Sharing(int interval_us, int shr_lit);

  /**
   * @brief Start sharing between two Shareable instances in the specified direction.
   * @param s1 the first Shareable
   * @param s2 the second Shareable
   */
  void share(Shareable& s1, Shareable& s2, SharingDir dir);

  /**
   * @brief Completely stops sharing and resets.
   */
  void stop() noexcept;

  /**
   * @brief Starts sharing process inside a set of sharing blocks.
   * @param su the set of sharing blocks
   */
  void share(SolverBlockList const& su);

  /**
   * @brief Starts sharing process inside a set of sharing solvers.
   * @param sl the set of sharing solvers
   */
  void share(SolverList const& sl);

  /**
   * @brief Automatically chooses share implementation.
   * @param the SharingUnit in which the sharing will be performed
   */
  void share(SharingUnit const& u);

  /**
   * @brief Starts sharing between two sets of solvers.
   * @param shp1 left sharing specification
   * @param shp2 right sharing specification
   * @param dir the direction specification
   */
  void share(SharingUnit const& su1, SharingUnit const& su2, SharingDir dir);

  /**
   * @brief Retrieves all solvers from sharing unit
   */
  static SolverList get_all_solvers(SharingUnit const& su);

 private:
  void _share_right(SharingUnit const& su1, SharingUnit const& su2);

  void _add_sharer(SolverList const& prod, SolverList const& cons);


 private:
  int _interval_us;
  int _shr_lit;
  std::vector<std::unique_ptr<painless::Sharer>> _sharers;
};

}  // namespace core::sat::sharing

#endif  // ITMO_PARSAT_SHARING_H
