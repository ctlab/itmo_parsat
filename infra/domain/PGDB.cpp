#include "infra/domain/PGDB.h"

namespace infra::domain {

PGDB::PGDB(std::string const& dbname, std::string const& user, std::string const& password)
    : conn_(
          std::string() + "dbname=" + dbname +
          (user.empty() ? "" : std::string() + "user=" + user + "password=" + password)) {
  IPS_INFO("Connected to " << conn_.dbname());
}

void PGDB::_exec0(std::string const& sql) {
  pqxx::work work(conn_);
  work.exec0(sql);
  work.commit();
}

size_t PGDB::_exec_count(std::string const& sql) {
  pqxx::work work(conn_);
  auto row = work.exec1(sql);
  size_t result = row.front().as<size_t>(0);
  work.commit();
  return result;
}

}  // namespace infra::domain