#ifndef ITMO_PARSAT_REGISTRY_H
#define ITMO_PARSAT_REGISTRY_H

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <fstream>

#include "Logger.h"
#include "TimeTracer.h"
#include "util/Assert.h"

namespace core {

/**
 * @brief Utility macro used to register and resolve dynamically-chosen
 * implementations.
 */
#define DEFINE_REGISTRY(INTERFACE, CONFIG_TYPE, NAME, ...)                    \
  class INTERFACE##Registry {                                                 \
   public:                                                                    \
    using builder_t =                                                         \
        std::function<INTERFACE*(CONFIG_TYPE const&, ##__VA_ARGS__)>;         \
                                                                              \
   public:                                                                    \
    struct INTERFACE##RegistryEntry {                                         \
      INTERFACE##RegistryEntry(std::string const& name, builder_t builder) {  \
        INTERFACE##Registry::instance().add_impl(name, std::move(builder));   \
      }                                                                       \
    };                                                                        \
                                                                              \
   public:                                                                    \
    INTERFACE##Registry() = default;                                          \
                                                                              \
   private:                                                                   \
    template <typename... Args>                                               \
    INTERFACE* _resolve(CONFIG_TYPE const& config, Args&&... args) {          \
      std::string name = config.NAME##_type();                                \
      IPS_VERIFY_S(                                                           \
          map_.count(name) > 0,                                               \
          "Trying to resolve non-existing implementation of " #NAME << ": "   \
                                                                    << name); \
      return map_[name](config, std::forward<Args>(args)...);                 \
    }                                                                         \
                                                                              \
   public:                                                                    \
    void add_impl(std::string const& name, builder_t builder) {               \
      IPS_VERIFY(                                                             \
          map_.count(name) == size_t(0) &&                                    \
          bool("Trying to register implementation of " #NAME                  \
               " that already exists."));                                     \
      map_[name] = std::move(builder);                                        \
    }                                                                         \
                                                                              \
   public:                                                                    \
    template <typename... Args>                                               \
    static INTERFACE* resolve(CONFIG_TYPE const& config, Args&&... args) {    \
      return instance()._resolve(config, std::forward<Args>(args)...);        \
    }                                                                         \
                                                                              \
    static void read_config(                                                  \
        std::istream& is, google::protobuf::Message& message) {}              \
                                                                              \
    static INTERFACE##Registry& instance() {                                  \
      static INTERFACE##Registry instance_;                                   \
      return instance_;                                                       \
    }                                                                         \
                                                                              \
   private:                                                                   \
    std::unordered_map<std::string, builder_t> map_{};                        \
  }  // namespace core

}  // namespace core

#define CONFIG_TYPE(INTERFACE) INTERFACE##Config

#define REGISTER_PROTO(INTERFACE, IMPL, GET_CONFIG)                          \
  template <typename... Args>                                                \
  INTERFACE* create_##IMPL(                                                  \
      CONFIG_TYPE(INTERFACE) const& config, Args&&... args) {                \
    return new IMPL(config.GET_CONFIG(), std::forward<Args>(args)...);       \
  }                                                                          \
  INTERFACE##Registry::INTERFACE##RegistryEntry reg_entry_##IMPL(            \
      #IMPL, [](CONFIG_TYPE(INTERFACE) const& config, auto&&... args) {      \
        return create_##IMPL(config, std::forward<decltype(args)>(args)...); \
      })

#define REGISTER_SIMPLE(INTERFACE, IMPL)                          \
  INTERFACE* create_##IMPL(CONFIG_TYPE(INTERFACE) const&) {       \
    return new IMPL();                                            \
  }                                                               \
  INTERFACE##Registry::INTERFACE##RegistryEntry reg_entry_##IMPL( \
      #IMPL, [](CONFIG_TYPE(INTERFACE) const& config) {           \
        return create_##IMPL(config);                             \
      })

#endif  // ITMO_PARSAT_REGISTRY_H
