#ifndef EA_BUILDER_H
#define EA_BUILDER_H

namespace ea::builder {

/**
 * An abstract builder of instances of class implementing Interface.
 */
template <typename Interface>
class Builder {
 public:
  Builder() = default;
  virtual ~Builder() = default;

  virtual Interface* build() = 0;

  /**
   * Returns an array of strings representing
   * parameter names.
   */
  virtual char const** get_params() = 0;
};

template <typename Interface>
using RBuilder = std::shared_ptr<Builder<Interface>>;

}  // namespace ea::builder

#endif  // EA_BUILDER_H
