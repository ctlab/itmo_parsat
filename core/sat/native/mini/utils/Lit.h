#ifndef Minisat_Lit_h
#define Minisat_Lit_h

namespace Mini {

typedef int Var;
const Var var_Undef = -1;

struct Lit {
  int x;

  bool operator==(Lit p) const {
    return x == p.x;
  }
  bool operator!=(Lit p) const {
    return x != p.x;
  }
  bool operator<(Lit p) const {
    return x < p.x;
  }  // '<' makes p, ~p adjacent in the ordering.
};

inline Lit mkLit(Var var, bool sign = false) {
  Lit p;
  p.x = var + var + (int) sign;
  return p;
}
inline Lit operator~(Lit p) {
  Lit q;
  q.x = p.x ^ 1;
  return q;
}
inline Lit operator^(Lit p, bool b) {
  Lit q;
  q.x = p.x ^ (unsigned int) b;
  return q;
}
inline bool sign(Lit p) {
  return p.x & 1;
}
inline int var(Lit p) {
  return p.x >> 1;
}

// Mapping Literals to and from compact integers suitable for array indexing:
inline int toInt(Var v) {
  return v;
}
inline int toInt(Lit p) {
  return p.x;
}
inline Lit toLit(int i) {
  Lit p;
  p.x = i;
  return p;
}

// const Lit lit_Undef = mkLit(var_Undef, false);  // }- Useful special constants.
// const Lit lit_Error = mkLit(var_Undef, true );  // }

const Lit lit_Undef = {-2};  // }- Useful special constants.
const Lit lit_Error = {-1};  // }

}  // namespace Mini

#endif  // Minisat_SolverTypes_h
