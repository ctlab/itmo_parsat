#ifndef ITMO_PARSAT_REGISTRY_H
#define ITMO_PARSAT_REGISTRY_H

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <fstream>

#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>

namespace core {

#define DEFINE_REGISTRY(INTERFACE, CONFIG_TYPE, NAME)                                         \
  class INTERFACE##Registry {                                                                 \
   public:                                                                                    \
    using builder_t = std::function<INTERFACE*(CONFIG_TYPE const&)>;                          \
                                                                                              \
   public:                                                                                    \
    struct INTERFACE##RegistryEntry {                                                         \
      INTERFACE##RegistryEntry(std::string const& name, builder_t builder) {                  \
        INTERFACE##Registry::instance().add_impl(name, std::move(builder));                   \
      }                                                                                       \
    };                                                                                        \
                                                                                              \
   public:                                                                                    \
    INTERFACE##Registry() = default;                                                          \
                                                                                              \
   private:                                                                                   \
    INTERFACE* _resolve(CONFIG_TYPE const& config) {                                          \
      std::string name = config.NAME##_type();                                                \
      LOG(INFO) << "Resolving " << name;                                                      \
      assert(map_.count(name) > 0 && bool("Trying to resolve non-existing implementation.")); \
      return map_[name](config);                                                              \
    }                                                                                         \
                                                                                              \
    INTERFACE* _resolve_by_path(std::filesystem::path const& path) {                          \
      std::ifstream is(path);                                                                 \
      CONFIG_TYPE config;                                                                     \
      read_config(is, config);                                                                \
      return _resolve(config);                                                                \
    }                                                                                         \
                                                                                              \
   public:                                                                                    \
    void add_impl(std::string const& name, builder_t builder) {                               \
      std::cerr << "Adding " << name << std::endl;                                            \
      assert(                                                                                 \
          map_.count(name) == size_t(0) &&                                                    \
          bool("Trying to register implementation that already exists."));                    \
      map_[name] = std::move(builder);                                                        \
    }                                                                                         \
                                                                                              \
   public:                                                                                    \
    static INTERFACE* resolve(CONFIG_TYPE const& config) {                                    \
      return instance()._resolve(config);                                                     \
    }                                                                                         \
                                                                                              \
    static INTERFACE* resolve_by_path(std::filesystem::path const& path) {                    \
      return instance()._resolve_by_path(path);                                               \
    }                                                                                         \
                                                                                              \
    static void read_config(std::istream& is, google::protobuf::Message& message) {           \
      std::string message_str(                                                                \
          (std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());            \
      google::protobuf::util::Status status =                                                 \
          google::protobuf::util::JsonStringToMessage(message_str, &message);                 \
      CHECK(status.ok()) << "Failed to read " << #CONFIG_TYPE " configuration from '"         \
                         << message_str << "':\n"                                             \
                         << status.error_message();                                           \
    }                                                                                         \
                                                                                              \
    static INTERFACE##Registry& instance() {                                                  \
      static INTERFACE##Registry instance_;                                                   \
      return instance_;                                                                       \
    }                                                                                         \
                                                                                              \
   private:                                                                                   \
    std::unordered_map<std::string, builder_t> map_{};                                        \
  }  // namespace core

}  // namespace core

#define REGISTER_PROTO(INTERFACE, IMPL, CONFIG_TYPE, GET_CONFIG)  \
  INTERFACE* create_##IMPL(CONFIG_TYPE const& config) {           \
    return new IMPL(config.GET_CONFIG());                         \
  }                                                               \
  INTERFACE##Registry::INTERFACE##RegistryEntry reg_entry_##IMPL( \
      #IMPL, [](CONFIG_TYPE const& config) { return create_##IMPL(config); })

#define REGISTER_SIMPLE(INTERFACE, IMPL, CONFIG_TYPE)             \
  INTERFACE* create_##IMPL(CONFIG_TYPE const&) {                  \
    return new IMPL();                                            \
  }                                                               \
  INTERFACE##Registry::INTERFACE##RegistryEntry reg_entry_##IMPL( \
      #IMPL, [](CONFIG_TYPE const& config) { return create_##IMPL(config); })

#endif  // ITMO_PARSAT_REGISTRY_H
