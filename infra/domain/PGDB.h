#ifndef ITMO_PARSAT_PGDB_H
#define ITMO_PARSAT_PGDB_H

#include <mutex>

#include "core/util/Logger.h"
#include "pqxx/pqxx"

namespace infra::domain {

class PGDB {
 public:
  PGDB(std::string const& dbname, std::string const& user, std::string const& password);

 protected:
  void _exec0(std::string const& sql);

  [[nodiscard]] size_t _exec_count(std::string const& sql);

 protected:
  std::mutex m_;
  pqxx::connection conn_;
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_PGDB_H
