#ifndef EVOL_CONFIGURATION_H
#define EVOL_CONFIGURATION_H

#include <boost/program_options.hpp>
#include <filesystem>

namespace ea {

class Configuration {
 public:
  Configuration();

  void add_options(boost::program_options::options_description const& options);

  void parse(int argc, char** argv);

  void parse(std::filesystem::path const& path);

  static Configuration& instance();

  template <typename T>
  T const& get(std::string const& name) const {
    return vm_[name].as<T>();
  }

 private:
  boost::program_options::variables_map vm_;
  boost::program_options::options_description desc_;
};

}  // namespace ea

#endif  // EVOL_CONFIGURATION_H
