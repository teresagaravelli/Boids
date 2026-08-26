#ifndef TOROIDALSPACE_HPP
#define TOROIDALSPACE_HPP

#include "vector3.hpp"

namespace boids {

class ToroidalSpace
{
 private:
  double Lx_;
  double Ly_;
  double Lz_;

  static double toroidal_shortcut(double d, double L);

 public:
  ToroidalSpace(double Lx, double Ly, double Lz);
  auto Lx() const {return Lx_;}
  auto Ly() const {return Ly_;}
  auto Lz() const {return Lz_;}

  Vector3 toroidal_displacement(Vector3 const& a, Vector3 const& b) const;

  Vector3 wrap_position(Vector3 position) const;
};

} // namespace boids

#endif