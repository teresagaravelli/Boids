#include "boid.hpp"
#include <cmath>

namespace boids {

Boid::Boid(Vector3 position, Vector3 velocity, double view_angle,
           double max_speed, ToroidalSpace space)
    : position_{position}
    , velocity_{velocity}
    , view_angle_{view_angle}
    , max_speed_{max_speed}
    , space_{space}
{}

Vector3 const& Boid::position() const
{
  return position_;
}

Vector3 const& Boid::velocity() const
{
  return velocity_;
}

double const& Boid::view_angle() const
{
  return view_angle_;
}

double const& Boid::max_speed() const
{
  return max_speed_;
}

void Boid::set_position(Vector3 const& position)
{
  position_ = position;
}

void Boid::set_velocity(Vector3 const& velocity)
{
  velocity_ = velocity;
}

void Boid::set_max_speed(double max_speed)
{
  max_speed_ = max_speed;
}

bool Boid::visible(Vector3 const& delta) const
{
  if (norm(velocity()) == 0 || norm(delta) == 0) {
    return true;
  }
  double cos_teta =
      scalar_product(velocity(), delta) / (norm(velocity()) * norm(delta));
  return std::cos(view_angle() / 2.) <= cos_teta;
}

void Boid::ceiling_effect()
{
  double h  = space_.Lz() / 2;
  double Vz = velocity().z;
  if (position().z > 9 * h / 10 || position().z < -9 * h / 10) {
    set_velocity(
        Vector3{velocity().x, velocity().y, Vz * 10 * (h - (position().z > 0 ? 1.0 : -1.0)*position().z) / h});
  }
  if (position().z >= h || position().z <= -h) {
    set_velocity(Vector3{velocity().x, velocity().y, -Vz * position().z / h});
  }
}
} 
