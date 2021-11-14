#ifndef EA_ALGORITHM_H
#define EA_ALGORITHM_H


namespace ea::algorithm {

class IAlgorithm {
public:
  virtual void process() = 0;
};

} // namespace ea::algorithm


#endif // EA_ALGORITHM_H
