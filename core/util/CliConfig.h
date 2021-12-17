#ifndef EVOL_CLICONFIG_H
#define EVOL_CLICONFIG_H

#include <filesystem>
#include <boost/program_options.hpp>
#include <google/protobuf/util/json_util.h>

#include "core/util/assert.h"

namespace core {

/**
 * Configuration manipulation routine
 */
class CliConfig {
 public:
  CliConfig();

  /**
   * Adds options to routine.
   */
  void add_options(boost::program_options::options_description const& options);

  /**
   * Parses options from cli args.
   */
  void parse(int argc, char** argv);

  /**
   * Parses options from file.
   */
  void parse(std::filesystem::path const& path);

  /**
   * Checks if parsing went successfull.
   */
  void notify();

  /**
   * Returns true iff the `name' is present in parsed options.
   */
  [[nodiscard]] bool has(std::string const& name) const;

  /**
   * Retrieves `name' option from arguments.
   */
  template <typename T>
  T const& get(std::string const& name) const {
    return vm_[name].as<T>();
  }

  /**
   * Reads protobuf configuration from a specified stream.
   */
  static void read_config(std::istream& ifs, google::protobuf::Message& message);

 private:
  boost::program_options::variables_map vm_;
  boost::program_options::options_description desc_;
};

}  // namespace core

#endif  // EVOL_CLICONFIG_H