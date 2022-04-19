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

#include "../sharing/Sharer.h"

#include <algorithm>
#include <unistd.h>

namespace painless {

void Sharer::do_remove() {
  if (!removeConsumers.empty() || !removeProducers.empty()) {
    std::lock_guard<std::mutex> lg(remove_mutex);
    for (size_t i = 0; i < removeProducers.size(); i++) {
      producers.erase(remove(producers.begin(), producers.end(), removeProducers[i]), producers.end());
    }
    removeProducers.clear();

    for (size_t i = 0; i < removeConsumers.size(); i++) {
      consumers.erase(remove(consumers.begin(), consumers.end(), removeConsumers[i]), consumers.end());
    }
    removeConsumers.clear();
  }
}

void Sharer::do_add() {
  if (!addProducers.empty() || !removeConsumers.empty()) {
    std::lock_guard<std::mutex> lg(add_mutex);
    producers.insert(producers.end(), addProducers.begin(), addProducers.end());
    addProducers.clear();
    consumers.insert(consumers.end(), addConsumers.begin(), addConsumers.end());
    addConsumers.clear();
  }
}

void Sharer::main_sharing_thread() {
  using namespace std::chrono_literals;
  int round = 0;
  auto sleep_dur = std::chrono::microseconds(shr_sleep_us);
  usleep(rand() % (shr_sleep_us >> 2));

  while (!_stop) {
    std::unique_lock<std::mutex> ul(share_mutex);
    share_cv.wait_for(ul, sleep_dur, [this] { return _stop; });
    if (_stop) { break; }
    round++;

    do_add();
    sharingStrategy->doSharing(id, producers, consumers);
    do_remove();
  }
}

Sharer::Sharer(
    int shr_sleep_us, int id, SharingStrategy* sharingStrategy, vector<SolverInterface*> producers,
    vector<SolverInterface*> consumers)
    : shr_sleep_us(shr_sleep_us) {
  this->id = id;
  this->sharingStrategy = sharingStrategy;
  this->producers = std::move(producers);
  this->consumers = std::move(consumers);
  sharer_thread = std::thread([this] { main_sharing_thread(); });
}

Sharer::~Sharer() noexcept {
  {
    std::unique_lock<std::mutex> lg(share_mutex);
    _stop = true;
  }
  share_cv.notify_all();
  if (sharer_thread.joinable()) { sharer_thread.join(); }
  delete sharingStrategy;
}

void Sharer::addProducer(SolverInterface* solver) {
  std::lock_guard<std::mutex> lg(add_mutex);
  addProducers.push_back(solver);
}

void Sharer::addConsumer(SolverInterface* solver) {
  std::lock_guard<std::mutex> lg(add_mutex);
  addConsumers.push_back(solver);
}

void Sharer::removeProducer(SolverInterface* solver) {
  std::lock_guard<std::mutex> lg(remove_mutex);
  removeProducers.push_back(solver);
}

void Sharer::removeConsumer(SolverInterface* solver) {
  std::lock_guard<std::mutex> lg(remove_mutex);
  removeConsumers.push_back(solver);
}

}  // namespace painless