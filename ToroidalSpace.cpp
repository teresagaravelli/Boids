#include "ToroidalSpace.hpp"

#include <stdexcept>

namespace boids {

 ToroidalSpace::ToroidalSpace(double Lx, double Ly, double Lz) : Lx_{Lx}, Ly_{Ly}, Lz_{Lz}
 {
   if (Lx <= 0.0 || Ly <= 0.0 || Lz <= 0.0) {
     throw std::invalid_argument("ToroidalSpace dimensions must be positive");
   }
}


double ToroidalSpace::toroidal_shortcut(double d, double L)
{
  while (d > L / 2.0) {
    d -= L;
  }

  while (d < -L / 2.0) {
    d += L;
  }

  return d;
}


Vector3 ToroidalSpace::toroidal_displacement(
    Vector3 const& a,
    Vector3 const& b) const
{
  Vector3 delta = b - a;

  delta.x = toroidal_shortcut(delta.x, Lx_);
  delta.y = toroidal_shortcut(delta.y, Ly_);
  // delta.z = toroidal_shortcut(delta.z, Lz_);

  return delta;
}


Vector3 ToroidalSpace::wrap_position(Vector3 position) const
{
  position.x = toroidal_shortcut(position.x, Lx_);
  position.y = toroidal_shortcut(position.y, Ly_);
  //position.z = toroidal_shortcut(position.z, Lz_);
  if (position.z > Lz_/2) {
    position.z = Lz_/2;
  } if (position.z < -Lz_/2) {
  position.z = -Lz_/2;
}
  return position;
}

}  // namespace boids