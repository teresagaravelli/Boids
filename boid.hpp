#ifndef BOID_HPP
#define BOID_HPP

#include "ToroidalSpace.hpp"
#include "vector3.hpp"

namespace boids {

class Boid {
 private:
    Vector3 position_;
    Vector3 velocity_;
    double view_angle_;
    double max_speed_;
    ToroidalSpace space_;

 public:
    Boid(Vector3 position, Vector3 velocity, double view_angle, double max_speed, ToroidalSpace space);

    Vector3 const& position() const;
    Vector3 const& velocity() const;
    double const& view_angle() const;
    double const& max_speed() const;
    

    void set_position(Vector3 const& position);
    void set_velocity(Vector3 const& velocity);
    void set_max_speed(double max_speed);
    bool visible(Vector3 const& delta) const;
    void ceiling_effect();

};
}
#endif