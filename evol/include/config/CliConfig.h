#ifndef EVOL_CLICONFIG_H
#define EVOL_CLICONFIG_H

#include <boost/program_options.hpp>
#include <filesystem>

namespace ea::config {

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

 private:
  boost::program_options::variables_map vm_;
  boost::program_options::options_description desc_;
};

}  // namespace ea::config

#endif  // EVOL_CLICONFIG_H
