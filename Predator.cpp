#include <cmath>

#include "Predator.hpp"

namespace boids {

Predator::Predator(Vector3 position, Vector3 velocity, double view_angle, bool hungry, ToroidalSpace space)
    : Boid{position, velocity, view_angle, 75., space}
    , hungry_{hungry}, timer_{0.}
{}

bool const& Predator::hungry() const
{
  return hungry_;
}

void Predator::set_hungry(bool hungry)
{
  hungry_ = hungry;
}

double const& Predator::timer() const { 
  return timer_;
}

void Predator::set_timer(double timer){
  
  timer_ = timer;
}




} // namespace boids

/*
Pred::(int cooldown_timer, bool hungry) : cooldown_timer_{cooldown_timer},
hungry_{hungry} {} void Pred::hunt (Boid prey) { set_velocity
(prey.velocity()-velocity());
}
void Pred::cooldown() {
Vector3 origin{0.0, 0.0, 0.0};
if (position() == origin) {}
else {
    set_velocity( position() * -1);
}
} */
