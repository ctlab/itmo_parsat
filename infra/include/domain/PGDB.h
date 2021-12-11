#ifndef ITMO_PARSAT_PGDB_H
#define ITMO_PARSAT_PGDB_H

#include "pqxx/pqxx"

namespace infra::domain {

class PGDB {
 public:
  PGDB(std::string const& dbname, std::string const& user, std::string const& password);

 protected:
  void _exec0(std::string const& sql);

 protected:
  pqxx::connection conn_;
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_PGDB_H
