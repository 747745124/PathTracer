#include "../utils/matrix.hpp"
#include "../utils/utility.hpp"
class PDF {
public:
  virtual float at(const gl::vec3 &direction) const = 0;
  virtual gl::vec3 get() const = 0;
  virtual ~PDF() = default;
};