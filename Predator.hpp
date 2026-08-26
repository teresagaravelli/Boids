#ifndef  PREDATOR_HPP
#define PREDATOR_HPP

#include "boid.hpp"

namespace boids {

class Predator : public Boid
{
 private:
    bool hungry_;
    double timer_{0.};
    
 public:

 Predator( Vector3 position, Vector3 velocity, double view_angle, bool hungry, ToroidalSpace space);
 
 bool const& hungry() const;
 void set_hungry(bool hungry);
 
 double const& timer() const;
 void set_timer(double timer);
 }; 
} 



#endif