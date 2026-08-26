#ifndef RENDER_HPP
#define RENDER_HPP

#include "Predator.hpp"
#include "flock.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

namespace boids {

// Renderer si occupa esclusivamente della rappresentazione grafica.
//
// La classe contiene:
// - una finestra per la proiezione XY;
// - una finestra per la proiezione YZ;
// - un pallino bianco per i boids;
// - un pallino rosso per i predatori.
class Renderer {
 private:
  sf::RenderWindow window_xy_;
  sf::RenderWindow window_yz_;

  // Forma grafica utilizzata per tutti i boids.
  sf::CircleShape boid_shape_;

  // Forma grafica utilizzata per tutti i predatori.
  sf::CircleShape predator_shape_;

  // Dimensioni dello spazio tridimensionale della simulazione.
  double Lx_;
  double Ly_;
  double Lz_;

  // Converte una coordinata della simulazione in una
  // coordinata in pixel.
  //
  // Per esempio, se lo spazio va da -500 a +500:
  // -500 viene convertito nel bordo iniziale della finestra;
  // 0 viene convertito nel centro della finestra;
  // +500 viene convertito nel bordo finale.
  static float coordinate_to_pixel(
      double coordinate,
      double space_length,
      unsigned int window_length);

  // Controlla gli eventi di una singola finestra.
  // In particolare, permette di chiuderla premendo la X.
  static void handle_events(sf::RenderWindow& window);

  // Disegna boids e predatori nella proiezione XY.
  void draw_xy(
      Flock const& flock,
      std::vector<Predator> const& predators);

  // Disegna boids e predatori nella proiezione YZ.
  void draw_yz(
      Flock const& flock,
      std::vector<Predator> const& predators);

 public:
  // Costruisce il renderer specificando le dimensioni
  // dello spazio tridimensionale.
  Renderer(double Lx, double Ly, double Lz);

  // Restituisce true se entrambe le finestre sono aperte.
  bool is_open() const;

  // Gestisce gli eventi di entrambe le finestre.
  void process_events();

  // Disegna un fotogramma completo nelle due finestre.
  void draw(
      Flock const& flock,
      std::vector<Predator> const& predators);
};

} // namespace boids

#endif