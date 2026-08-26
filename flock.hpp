#ifndef FLOCK_HPP
#define FLOCK_HPP

#include <cstddef>
#include <vector>

#include "boid.hpp"
#include "ToroidalSpace.hpp"
namespace boids {

class Flock {
 private:
  std::vector<Boid> boids_;

  double d_;
  double ds_;
  double s_;
  double a_;
  double c_;

  ToroidalSpace const& space_;

  Vector3 separation(std::size_t i) const;
  Vector3 alignment(std::size_t i) const; // non modificano i double sopra o il vettore,
  Vector3 cohesion(std::size_t i) const;  // calcolano i dati nuovi v1, v2 e v3


 public:
  Flock(std::vector<Boid> boids, double d, double ds, double s, double a, double c,ToroidalSpace const& space);
  double const& d() const;

  void update(double dt);

  double mean_speed() const;
  double speed_stddev() const;

  double mean_distance() const;
  double distance_stddev() const;

  std::vector<Boid> const& boids() const;
  void set_boids(std::vector<Boid> boids);

};
}

#endif