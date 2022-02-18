#ifndef EVOL_CLICONFIG_H
#define EVOL_CLICONFIG_H

#include <filesystem>
#include <boost/program_options.hpp>
#include <google/protobuf/util/json_util.h>

#include "assert.h"

namespace core {

/**
 * @brief Configuration manipulation routine
 */
class CliConfig {
 public:
  CliConfig();

  /**
   * @param options options to be added.
   */
  void add_options(boost::program_options::options_description const& options);

  /**
   * @brief Parses options from cmd arguments.
   * @param argc number of options.
   * @param argv options array.
   */
  void parse(int argc, char** argv);

  /**
   * @brief Parses options from file.
   * @param path file to be parsed.
   */
  void parse(std::filesystem::path const& path);

  /**
   * @brief Checks if parsing went successfully.
   */
  void notify();

  /**
   * @return true iff the `name' is present in parsed options.
   */
  [[nodiscard]] bool has(std::string const& name) const;

  /**
   * @brief Retrieves option from arguments.
   * @tparam T the type of option.
   * @param name the name of option.
   * @return the option value.
   */
  template <typename T>
  T const& get(std::string const& name) const {
    return vm_[name].as<T>();
  }

  /**
   * @brief Reads protobuf configuration from a specified stream.
   * @param ifs input stream with the configuration.
   * @param message the message to be filled.
   */
  static void read_config(std::istream& ifs, google::protobuf::Message& message);

 private:
  boost::program_options::variables_map vm_;
  boost::program_options::options_description desc_;
};

}  // namespace core

#endif  // EVOL_CLICONFIG_H