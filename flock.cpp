#include "flock.hpp"
#include <stdexcept>
#include <cmath>

namespace boids {

Flock::Flock(std::vector<Boid> boids, double d, double ds, double s, double a, double c, ToroidalSpace const& space)
    : boids_{boids}, d_{d}, ds_{ds}, s_{s}, a_{a}, c_{c}, space_{space} {} 

Vector3 Flock::separation(std::size_t i) const {
    Vector3 result{0., 0., 0.};

    for (std::size_t j = 0; j < boids_.size(); ++j) {
        if (j == i) {
            continue;
        }
        Vector3 delta = space_.toroidal_displacement (boids_[i].position(), boids_[j].position());

        if (norm(delta) < ds_) {

            result = result - delta * s_;
        }
    }
    return result;
} 

Vector3 Flock::alignment(std::size_t i) const
{
    Vector3 velocity_diff_sum{0., 0., 0.};
    std::size_t neighbours{0};

    for (std::size_t j = 0; j < boids_.size(); ++j) {
        if (j == i) {
            continue;
        }
        Vector3 delta = space_.toroidal_displacement (boids_[i].position(), boids_[j].position());

        if (norm(delta) < d_ && boids_[i].visible (delta)) {

            velocity_diff_sum = velocity_diff_sum + (boids_[j].velocity() - boids_[i].velocity());

            ++neighbours;
        }
    }

    if (neighbours == 0) {
        return Vector3{0., 0., 0.};
    }

    double const n = static_cast<double>(neighbours);

    return velocity_diff_sum * a_ / n; //è n-1 perché non conta se stesso
}

Vector3 Flock::cohesion(std::size_t i) const {
    Vector3 position_sum{0., 0., 0.};
    std::size_t neighbours{0};

    for (std::size_t j = 0; j < boids_.size(); ++j) {
        if (j == i) {
            continue;
        }
        Vector3 delta = space_.toroidal_displacement (boids_[i].position(), boids_[j].position());

        if (norm(delta) < d_ && boids_[i].visible (delta)) {

            position_sum = position_sum + delta;
            ++neighbours;
        }
    }

    if (neighbours == 0) {
        return Vector3{0., 0.,0.};
    }

    double const n = static_cast<double>(neighbours);
    Vector3 const centre = position_sum / n; 

    return centre  * c_;
}

void Flock::update(double dt)
{
    std::vector<Boid> new_boids = boids_;

    for (std::size_t i = 0; i < boids_.size(); ++i) {

        Vector3 new_velocity = boids_[i].velocity() + separation(i) + alignment(i) + cohesion(i);

        double const speed = norm(new_velocity);
        double const max_speed = boids_[i].max_speed();
        if (speed > max_speed) { new_velocity = new_velocity * (max_speed / speed);
}

        Vector3 new_position = boids_[i].position() + new_velocity * dt;
        new_position = space_.wrap_position (new_position);

        new_boids[i].set_velocity(new_velocity);
        new_boids[i].set_position(new_position);
    }

    boids_.swap(new_boids);
}

double Flock::mean_speed() const
{
    if (boids_.empty()) {
        return 0.;
    }

    double sum{0.};

    for (std::size_t i = 0; i < boids_.size(); ++i) {
        sum += norm(boids_[i].velocity());
    }

    double const n = static_cast<double>(boids_.size());

    return sum / n;
}

double Flock::speed_stddev() const
{
    if (boids_.empty()) {
        return 0.;
    }

    double const mean = mean_speed();

    double sum{0.};

    for (std::size_t i = 0; i < boids_.size(); ++i) {

        double const difference = norm(boids_[i].velocity()) - mean;

        sum += difference * difference;
    }
    
    double const n = static_cast<double>(boids_.size());

    return std::sqrt(sum / n);
}

double Flock::mean_distance() const
{
    if (boids_.size() < 2) {
        return 0.;
    }

    double sum{0.};
    std::size_t pairs{0};

    for (std::size_t i = 0; i < boids_.size(); ++i) {

        for (std::size_t j = i + 1; j < boids_.size(); ++j) {
            Vector3 delta = space_.toroidal_displacement (boids_[i].position(), boids_[j].position());

            sum += norm(delta);
            ++pairs;
        }
    }

    return sum / static_cast<double>(pairs);
}

double Flock::distance_stddev() const
{
    if (boids_.size() < 2) {
        return 0.;
    }

    double const mean = mean_distance();

    double sum{0.};
    std::size_t pairs{0};

    for (std::size_t i = 0; i < boids_.size(); ++i) {

        for (std::size_t j = i + 1; j < boids_.size(); ++j) {
            Vector3 delta = space_.toroidal_displacement (boids_[i].position(), boids_[j].position());

            double const difference = norm(delta) - mean;

            sum += difference * difference;

            ++pairs;
        }
    }

    return std::sqrt(
        sum / static_cast<double>(pairs));
}

std::vector<Boid> const& Flock::boids() const
{
    return boids_;
}
void Flock::set_boids(std::vector<Boid> boids) {
    boids_.swap(boids);
}

double const& Flock::d() const {
    return d_;
}
};
