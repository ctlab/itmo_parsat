#ifndef MINI_LBOOL_H
#define MINI_LBOOL_H

#include <cstdint>

namespace Mini {

//=================================================================================================
// Lifted booleans:
//
// NOTE: this implementation is optimized for the case when comparisons between values are mostly
//       between one variable and one constant. Some care had to be taken to make sure that gcc
//       does enough constant propagation to produce sensible code, and this appears to be somewhat
//       fragile unfortunately.

class lbool {
  uint8_t value;

 public:
  explicit lbool(uint8_t v) : value(v) {}

  lbool& operator=(lbool const& o) {
    value = o.value;
    return *this;
  }

  lbool() : value(0) {}
  explicit lbool(bool x) : value(!x) {}

  bool operator==(lbool b) const { return ((b.value & 2) & (value & 2)) | (!(b.value & 2) & (value == b.value)); }
  bool operator!=(lbool b) const { return !(*this == b); }
  lbool operator^(bool b) const { return lbool((uint8_t) (value ^ (uint8_t) b)); }

  lbool operator&&(lbool b) const {
    uint8_t sel = (this->value << 1) | (b.value << 3);
    uint8_t v = (0xF7F755F4 >> sel) & 3;
    return lbool(v);
  }

  lbool operator||(lbool b) const {
    uint8_t sel = (this->value << 1) | (b.value << 3);
    uint8_t v = (0xFCFCF400 >> sel) & 3;
    return lbool(v);
  }

  friend int toInt(lbool l);
  friend lbool toLbool(int v);
};
inline int toInt(lbool l) { return l.value; }
inline lbool toLbool(int v) { return lbool((uint8_t) v); }

#if defined(MINISAT_CONSTANTS_AS_MACROS)
#ifndef l_True
#define l_True (lbool((uint8_t) 0))  // gcc does not do constant propagation if these are real constants.
#endif
#ifndef l_False
#define l_False (lbool((uint8_t) 1))
#endif
#ifndef l_Undef
#define l_Undef (lbool((uint8_t) 2))
#endif
#else
const lbool l_True((uint8_t) 0);
const lbool l_False((uint8_t) 1);
const lbool l_Undef((uint8_t) 2);
#endif

}  // namespace Mini

#endif  // MINI_LBOOL_H

