#ifndef ITMO_PARSAT_TYPES_H
#define ITMO_PARSAT_TYPES_H

#include <vector>

#include "core/sat/native/mini/mtl/Vec.h"
#include "core/sat/native/mini/utils/Lit.h"

namespace core {

typedef std::vector<bool> bit_mask_t;

typedef Mini::vec<Mini::Lit> lit_vec_t;

}  // namespace core


#endif  // ITMO_PARSAT_TYPES_H
