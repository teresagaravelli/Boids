#ifndef WILD_HPP
#define WILD_HPP

#include "boid.hpp"
#include "Predator.hpp"
#include "flock.hpp"
#include "ToroidalSpace.hpp"

#include <cstddef>
#include <vector>

namespace boids {

class Wild {
 private:
   Flock& flock_;
   std::vector<Predator> predators_;
   ToroidalSpace const& space_;
   static constexpr double hunting_max_speed = 75.;
   static constexpr double hunting_min_speed = 25.;
   static constexpr double digestion_max_speed = 25.;
   static constexpr double capture_distance = 7.;
   static constexpr double digestion_time = 5.;
   static constexpr double dive_velocity_change = 90.;

    
 public:
   Wild (Flock& flock, std::vector<Predator> const&  predators, ToroidalSpace const& space);
   double const& d() const;
   Flock const& flock() const;
   std::vector<Predator> const& predators() const;
   /*auto const& flock() const;
   auto const& predators() const; */
  

   void hunt_speed(std::size_t i); //trova prede e modifica velocità predatore 
   void escape(); //aumenta velocità boid per scappare
   void refresh_predators(double dt);
   void eaten();
   void update_all(double dt);
   void ceiling_correction();
};
} 

#endif