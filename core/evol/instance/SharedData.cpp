#include "core/evol/instance/SharedData.h"

namespace ea::instance {

void SharedData::SamplingConfig::do_scale() {
  IPS_VERIFY(can_scale > 0U && bool("do_scale called when scaling is not possible."));
  --can_scale;
  samples = (uint32_t)(samples * scale);
}

}  // namespace ea::instance
