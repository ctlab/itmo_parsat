#ifndef EA_INSTANCE_H
#define EA_INSTANCE_H

#include <memory>


namespace ea::instance {

class IInstance {
public:
  virtual ~IInstance() = default;

  /**
   * Calculates instance's fitness function
   * Used to perform selection.
   */
  virtual double fitness() const = 0;
};

using RInstance = std::shared_ptr<IInstance>;

} // namespace ea::instance


#endif // EA_INSTANCE_H
