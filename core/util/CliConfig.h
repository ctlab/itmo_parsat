#ifndef EVOL_CLICONFIG_H
#define EVOL_CLICONFIG_H

#include <filesystem>
#include <boost/program_options.hpp>
#include <google/protobuf/util/json_util.h>

#include "core/util/assert.h"

namespace core {

class CliConfig {
 public:
  CliConfig();

  void add_options(boost::program_options::options_description const& options);

  void parse(int argc, char** argv);

  void parse(std::filesystem::path const& path);

  void notify();

  [[nodiscard]] bool has(std::string const& name) const;

  template <typename T>
  T const& get(std::string const& name) const {
    return vm_[name].as<T>();
  }

  static CliConfig& instance();

  static void read_config(std::istream& ifs, google::protobuf::Message& message);

 private:
  boost::program_options::variables_map vm_;
  boost::program_options::options_description desc_;
};

}  // namespace core

#endif  // EVOL_CLICONFIG_H