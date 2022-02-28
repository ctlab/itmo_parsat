// -----------------------------------------------------------------------------
// Copyright (C) 2017  Ludovic LE FRIOUX
//
// This file is part of PaInleSS.
//
// PaInleSS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// -----------------------------------------------------------------------------

#pragma once

#include "core/sat/native/painless/painless-src/sharing/SharingStrategy.h"
#include "core/sat/native/painless/painless-src/utils/Threading.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace painless {

/// A sharer is a thread responsible to share clauses between solvers.
class Sharer {
 public:
  /// Constructor.
  Sharer(
      int shr_sleep_us, int id_, SharingStrategy* sharingStrategy_,
      vector<SolverInterface*> producers_, vector<SolverInterface*> consumer_);

  ~Sharer() noexcept;

  void addProducer(SolverInterface* solver);

  void addConsumer(SolverInterface* solver);

  void removeProducer(SolverInterface* solver);

  void removeConsumer(SolverInterface* solver);

 protected:
  void do_remove();

  void do_add();

  void main_sharing_thread();

 protected:
  int id;
  int shr_sleep_us;
  bool _stop = false;

  SharingStrategy* sharingStrategy;

  std::mutex add_mutex;
  std::mutex remove_mutex;
  std::mutex share_mutex;
  std::condition_variable share_cv;
  std::thread sharer_thread;

  std::vector<SolverInterface*> addProducers;
  std::vector<SolverInterface*> addConsumers;
  std::vector<SolverInterface*> removeProducers;
  std::vector<SolverInterface*> removeConsumers;
  std::vector<SolverInterface*> producers;
  std::vector<SolverInterface*> consumers;

  WorkingResult* result;
};

}  // namespace painless
