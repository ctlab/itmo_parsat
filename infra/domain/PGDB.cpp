#include "infra/include/domain/PGDB.h"

namespace infra::domain {

PGDB::PGDB(std::string const& dbname, std::string const& user, std::string const& password)
    : conn_(
          std::string() + "dbname=" + dbname +
          (user.empty() ? "" : std::string() + "user=" + user + "password=" + password)) {
  LOG(INFO) << "Connected to " << conn_.dbname();
}

void PGDB::_exec0(std::string const& sql) {
  pqxx::work work(conn_);
  work.exec0(sql);
  work.commit();
}

}  // namespace infra::domain