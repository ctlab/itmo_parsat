#ifndef EA_LIMIT_H
#define EA_LIMIT_H


namespace ea::limit {
  
class ILimit {
public:
  virtual ~ILimit() = default;

  /**
   * Returns true if algorithm can and should proceed.
   */
  virtual bool proceed() = 0;

  /**
   * Indicates that limit must start tracking parameters
   * that it should track (for ex. memory/time consumption).
   */
  virtual void start() {}
};

using RLimit = std::shared_ptr<ILimit>;

} // namespace ea::limit


#endif // EA_LIMIT_H
