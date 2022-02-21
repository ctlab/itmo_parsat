#ifndef ITMO_PARSAT_REFERENCE_H
#define ITMO_PARSAT_REFERENCE_H

#include <memory>

#define MAKE_REFS(cls)                 \
  using U##cls = std::unique_ptr<cls>; \
  using R##cls = std::shared_ptr<cls>

#endif  // ITMO_PARSAT_REFERENCE_H
