#ifndef ITMO_PARSAT_PGDB_H
#define ITMO_PARSAT_PGDB_H

#include <mutex>
#include "pqxx/pqxx"

namespace infra::domain {

class PGDB {
 public:
  PGDB(std::string const& dbname, std::string const& user, std::string const& password);

 protected:
  void _exec0(std::string const& sql);

 protected:
  std::mutex m_;
  pqxx::connection conn_;
};

}  // namespace infra::instance

#endif  // ITMO_PARSAT_PGDB_H
