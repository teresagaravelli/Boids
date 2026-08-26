#include "wild.hpp"
#include "ToroidalSpace.hpp"
#include <cmath>
#include <random>
#include <stdexcept>

namespace boids {
Wild::Wild(boids::Flock& flock, std::vector<Predator> const& predators,
           ToroidalSpace const& space)
    : flock_{flock}
    , predators_{predators}
    , space_{space}
{}

Flock const& Wild::flock() const
{
  return flock_;
}

std::vector<Predator> const& Wild::predators() const
{
  return predators_;
}

void Wild::hunt_speed(std::size_t i)
{
  predators_[i].set_max_speed(hunting_max_speed);
  bool hungry = predators_[i].hungry();
  if (hungry == false) {
  } else {
    auto const& boids = flock_.boids();
    Vector3 min_delta{1000., 1000., 1000.};
    for (std::size_t j = 0; j < boids.size(); j++) {
      Vector3 const delta = space_.toroidal_displacement(
          predators_[i].position(),
          boids[j]
              .position()); // così parte dal predatore ed arriva al piccione

      double const distance = norm(delta);
      if (predators_[i].visible(delta) && distance > 0. &&  distance <= flock_.d()
          && distance < norm(min_delta)) {
        min_delta = delta;
      }
    }
    if (min_delta == Vector3{1000., 1000., 1000.} || min_delta == Vector3{0., 0., 0.}) { 
      return; 
    }

    predators_[i].set_velocity((min_delta / norm(min_delta)) 
    * (hunting_min_speed + ((predators_[i].max_speed() -hunting_min_speed) * (flock_.d() - norm(min_delta)) / flock_.d()))); 
  }
}

void Wild::escape()
{
  auto boids = flock_.boids();
  for (std::size_t i = 0; i < boids.size(); i++) {
    for (std::size_t j = 0; j < predators_.size(); j++) {
      auto delta = space_.toroidal_displacement(boids[i].position(),
                                                predators_[j].position());
      double const distance = norm(delta);
      if ( distance >0. && distance <= flock_.d() && boids[i].visible(delta) ) { // modificato l'if, flock_.d è il
                                          // raggio di vista del piccione
        boids[i].set_velocity(boids[i].velocity()
                              - delta * boids[i].max_speed()
                                    * (flock_.d() - norm(delta))
                                    / (norm(delta) * flock_.d()));
      }
    }
  }
  flock_.set_boids(boids); // stavate modifcando solo una copia chiamata boids
}

void Wild::eaten()
{
  auto boids = flock_.boids(); 
  for (std::size_t i = 0; i < boids.size(); ) {
    bool eaten = false;
    for (std::size_t j = 0; j < predators_.size(); j++) {
      Vector3 delta = space_.toroidal_displacement(
          boids[i].position(),
          predators_[j].position()); // aggiunto una riga per estetica
      if (norm(delta) <= capture_distance && predators_[j].hungry() == true) {
        predators_[j].set_hungry(false);
        boids.erase(boids.begin() + i);
        eaten = true; 
        break; 
      }
  
    }
  if (eaten == false ){ 
    ++i; 
    }
  }
  flock_.set_boids(boids);
}

void Wild::refresh_predators(double dt)
{
  for (std::size_t i = 0; i < predators_.size(); ++i) {
    if (predators_[i].hungry() == true) {
      hunt_speed(i); // se è affamato applica hunt_speed
    } else {
      predators_[i].set_max_speed(digestion_max_speed);
      double max_speed = predators_[i].max_speed();

      if (predators_[i].timer() == 0) {
        Vector3 velocity = predators_[i].velocity();
        velocity.z -= dive_velocity_change; // va verso il basso dopo aver mangiato
        predators_[i].set_velocity(velocity);
        if (norm(predators_[i].velocity()) >= predators_[i].max_speed()) {
          predators_[i].set_velocity(predators_[i].velocity() * max_speed
                                     / norm(predators_[i].velocity()));
        }
      }

      predators_[i].set_timer(predators_[i].timer() + dt); // aumenta tempo
      if (predators_[i].timer()
          >= digestion_time) { // se è finito il tempo di digestione, il
                               // maggiore serve nel caso in cui si sminchino i
                               // calcoli
        predators_[i].set_hungry(true); // torna affamato e resetta il timer
        predators_[i].set_timer(0.);
        predators_[i].set_max_speed(hunting_max_speed);

        std::random_device rd;
        std::default_random_engine generator{rd()};

        std::uniform_real_distribution<double> random_velocity_x{-hunting_max_speed, hunting_max_speed};
        std::uniform_real_distribution<double> random_velocity_y{-hunting_max_speed, hunting_max_speed};
        std::uniform_real_distribution<double> random_velocity_z{0., hunting_max_speed};

        Vector3 new_velocity =
            Vector3{random_velocity_x(generator), random_velocity_y(generator),
                    random_velocity_z(generator)};
        predators_[i].set_velocity(new_velocity);
        if (norm(predators_[i].velocity()) >= predators_[i].max_speed()) {
          predators_[i].set_velocity(predators_[i].velocity()
                                     * predators_[i].max_speed()
                                     / norm(predators_[i].velocity()));
        }
      }
    }
    Vector3 new_position =
        predators_[i].position() + predators_[i].velocity() * dt;
    new_position = space_.wrap_position(new_position);
    predators_[i].set_position(new_position);
  }
}

void Wild::ceiling_correction()
{
  for (std::size_t i = 0; i < predators_.size(); ++i) {
    predators_[i].ceiling_effect();
  }
  auto boids = flock_.boids();
  for (std::size_t j = 0; j < boids.size(); ++j) {
    boids[j].ceiling_effect();
  }
  flock_.set_boids(boids);
}

void Wild::update_all(double dt)
{
  eaten(); // così non ho problemi in escape

  escape(); // va fatto prima escape, perché se no si sminchia l'update
  refresh_predators(dt); // ci sta dentro hunt_speed
  flock_.update(dt); // essendo fatto l'update con il boids[i].velocity va bene,
                     // percé così prende anche escape.
  eaten();
  ceiling_correction();
}
} 


/*
#include "wild.hpp"
#include "ToroidalSpace.hpp"
#include <cmath>
#include <random>
#include <stdexcept>

namespace boids {
Wild::Wild(Flock& flock, std::vector<Predator> const& Predators,
           ToroidalSpace const& space)
    : Flock_{flock}
    , Predators_{Predators}
    , space_{space}
{}

auto const& Wild::flock() const
{
  return Flock_;
}

auto const& Wild::Predators() const
{
  return Predators_;
}

void Wild::hunt_speed(std::size_t i)
{
 
  if (!Predators_[i].hungry()) {
    return;
  } 
  else {
    Predators_[i].set_max_speed(75.);
    auto const& boids = Flock_.boids();
    Vector3 min_delta{1000., 1000., 1000.};
    for (std::size_t j = 0; j < boids.size(); j++) {
      Vector3 const delta = space_.toroidal_displacement(
          Predators_[i].position(),
          boids[j]
              .position()); // così parte dal predatore ed arriva al piccione

      double const distance = norm(delta);
      if (distance > 0. &&  distance <= Flock_.d()
          && distance < norm(min_delta) && Predators_[i].visible(delta)) {
        min_delta = delta;
      }
    }
    if (min_delta == Vector3{1000., 1000., 1000.} || min_delta == Vector3{0., 0., 0.}) { 
      return; 
    }

    Predators_[i].set_velocity((min_delta / norm(min_delta)) 
    * (25 + ((Predators_[i].max_speed() -25) * (Flock_.d() - norm(min_delta)) / Flock_.d())));  //25 è stata settata come velocità minima
  }
}

void Wild::escape()
{
  auto boids = Flock_.boids();
  for (std::size_t i = 0; i < boids.size(); i++) {
    for (std::size_t j = 0; j < Predators_.size(); j++) {
      auto delta = space_.toroidal_displacement(boids[i].position(),
                                                Predators_[j].position());
      double const distance = norm(delta);
      if ( distance >0. && distance <= Flock_.d() && boids[i].visible(delta) ) { // modificato l'if, flock_.d è il
                                          // raggio di vista del piccione
        boids[i].set_velocity(boids[i].velocity()
                              - delta * boids[i].max_speed()
                                    * (Flock_.d() - norm(delta))
                                    / (norm(delta) * Flock_.d()));
      }
    }
  }
  Flock_.set_boids(boids); // stavate modifcando solo una copia chiamata boids
}

void Wild::eaten()
{
  auto boids = Flock_.boids(); 
  for (std::size_t i = 0; i < boids.size(); ) {
    bool eaten = false;
    for (std::size_t j = 0; j < Predators_.size(); j++) {
      Vector3 delta = space_.toroidal_displacement(
          boids[i].position(),
          Predators_[j].position()); // aggiunto una riga per estetica
      if (norm(delta) <= 7. && Predators_[j].hungry()) {
        Predators_[j].set_hungry(true);
        boids.erase(boids.begin() + i);
        eaten = true; 
        break; 
      }
  
    }
  if (!eaten){ 
    ++i; 
    }
  }
  Flock_.set_boids(boids);
}

void Wild::refresh_predators(double dt)
{
  double const digestion_time =
      5.; // tempo in cui predatore rimane sazio, settabile da noi
  for (std::size_t i = 0; i < Predators_.size(); ++i) {
    if (Predators_[i].hungry() == true) {
      hunt_speed(i); // se è affamato applica hunt_speed
    } else {
      Predators_[i].set_max_speed(25);
      double max_speed = Predators_[i].max_speed();

      if (Predators_[i].timer() == 0) {
        Vector3 velocity = Predators_[i].velocity();
        velocity.z -= 90.; // va verso il basso dopo aver mangiato
        Predators_[i].set_velocity(velocity);
        if (norm(Predators_[i].velocity()) >= Predators_[i].max_speed()) {
          Predators_[i].set_velocity(Predators_[i].velocity() * max_speed
                                     / norm(Predators_[i].velocity()));
        }
      }

      Predators_[i].set_timer(Predators_[i].timer() + dt); // aumenta tempo
      if (Predators_[i].timer()
          >= digestion_time) { // se è finito il tempo di digestione, il
                               // maggiore serve nel caso in cui si sminchino i
                               // calcoli
        Predators_[i].set_hungry(true); // torna affamato e resetta il timer
        Predators_[i].set_timer(0.);
        Predators_[i].set_max_speed(75.);

        std::random_device rd;
        std::default_random_engine generator{rd()};

        std::uniform_real_distribution<double> random_velocity_x{-75., 75.};
        std::uniform_real_distribution<double> random_velocity_y{-75., 75.};
        std::uniform_real_distribution<double> random_velocity_z{0., 75.};

        Vector3 new_velocity =
            Vector3{random_velocity_x(generator), random_velocity_y(generator),
                    random_velocity_z(generator)};
        Predators_[i].set_velocity(new_velocity);
        if (norm(Predators_[i].velocity()) >= Predators_[i].max_speed()) {
          Predators_[i].set_velocity(Predators_[i].velocity()
                                     * Predators_[i].max_speed()
                                     / norm(Predators_[i].velocity()));
        }
      }
    }
    Vector3 new_position =
        Predators_[i].position() + Predators_[i].velocity() * dt;
    new_position = space_.wrap_position(new_position);
    Predators_[i].set_position(new_position);
  }
}

void Wild::ceiling_correction()
{
  for (std::size_t i = 0; i < Predators_.size(); ++i) {
    Predators_[i].ceiling_effect();
  }
  auto boids = Flock_.boids();
  for (std::size_t j = 0; j < boids.size(); ++j) {
    boids[j].ceiling_effect();
  }
  Flock_.set_boids(boids);
}

void Wild::update_all(double dt)
{
  eaten(); // così non ho problemi in escape

  escape(); // va fatto prima escape, perché se no si sminchia l'update
  refresh_predators(dt); // ci sta dentro hunt_speed
  Flock_.update(dt); // essendo fatto l'update con il boids[i].velocity va bene,
                     // percé così prende anche escape.
  eaten();
  ceiling_correction();
}
} */