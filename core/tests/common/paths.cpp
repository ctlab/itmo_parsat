#include "core/tests/common/paths.h"

namespace common {

std::vector<std::string> small_inputs = {
    "sat_aim-50-1_6-yes1-4-@1.cnf",
    "sat_par8-1-c-@1.cnf",
    "sat_quinn-@1.cnf",
    "sat_simple_v3_c2-@0.cnf",
    "unsat_aim-100-1_6-no-1-@0.cnf",
    "unsat_bf0432-007-@1.cnf",
    "unsat_dubois20-@0.cnf",
    "unsat_dubois21-@0.cnf",
    "unsat_dubois22-@0.cnf",
    "unsat_hole6-@1.cnf",
};

std::vector<std::string> large_inputs = {
    "unsat_WS_400_16_90_70.apx_0.cnf",
    "unsat_battleship-12-12-unsat.cnf",
    "unsat_E02F17.cnf",
    "unsat_MVD_ADS_S1_5_5.cnf",
    "unsat_MVD_ADS_S10_5_6.cnf",
    "unsat_pancake_vs_selection_7_4-@2.cnf",
    "unsat_sgen6_900_100-@2.cnf",
    "unsat_bubble_vs_pancake_7_6_simp-@3.cnf",
};

}  // namespace common