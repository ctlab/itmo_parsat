#ifndef EA_TYPES_H
#define EA_TYPES_H

#include <vector>
#include <memory>

#include "evol/include/instance.h"


namespace ea::types {

using Population = std::vector<instance::RInstance>;
using RPopulation = std::shared_ptr<Population>;

} // namespace ea::types


#endif // EA_TYPES_H
