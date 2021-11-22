#ifndef EA_ALGORITHM_H
#define EA_ALGORITHM_H

namespace ea::algorithm {

class Algorithm {
 public:
  /**
   * Starts algorithm.
   */
  virtual void process() = 0;

  /**
   * Interrupts algorithm.
   */
  void interrupt();

 protected:
  [[nodiscard]] bool is_interrupted() const;

 private:
  bool interrupted_ = false;
};

}  // namespace ea::algorithm

#endif  // EA_ALGORITHM_H
