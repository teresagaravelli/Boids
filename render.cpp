#include "render.hpp"

#include "Predator.hpp"
#include "flock.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

namespace boids {

Renderer::Renderer(double Lx, double Ly, double Lz)
    : window_xy_{
          sf::VideoMode(800, 550),
          "Boids - proiezione XY"}
    , window_yz_{
          sf::VideoMode(800, 550),
          "Boids - proiezione YZ"}
    , boid_shape_{3.f}
    , predator_shape_{6.f}
    , Lx_{Lx}
    , Ly_{Ly}
    , Lz_{Lz}
{
  // I boids sono rappresentati da pallini bianchi.
  boid_shape_.setFillColor(sf::Color::White);

  // Per impostazione predefinita, la posizione di un CircleShape
  // corrisponde al suo angolo superiore sinistro.
  //
  // Spostando l'origine al centro, la posizione assegnata alla
  // forma coincide con il centro del pallino.
  float const boid_radius = boid_shape_.getRadius();
  boid_shape_.setOrigin(boid_radius, boid_radius);

  // I predatori sono rappresentati da pallini rossi.
  // Sono leggermente più grandi dei boids per renderli riconoscibili.
  predator_shape_.setFillColor(sf::Color::Red);

  float const predator_radius = predator_shape_.getRadius();
  predator_shape_.setOrigin(predator_radius, predator_radius);

  // Limitiamo entrambe le finestre a 60 fotogrammi al secondo.
  window_xy_.setFramerateLimit(60);
  window_yz_.setFramerateLimit(60);

  // Posizione iniziale delle due finestre sullo schermo.
  //
  // Se lo schermo non è abbastanza largo, le finestre possono
  // essere spostate manualmente.
  window_xy_.setPosition(sf::Vector2i{10, 50});
  window_yz_.setPosition(sf::Vector2i{830, 50});
}

float Renderer::coordinate_to_pixel(
    double coordinate,
    double space_length,
    unsigned int window_length)
{
  // Le coordinate della simulazione sono comprese tra:
  //
  // -space_length / 2 e +space_length / 2.
  //
  // Aggiungendo space_length / 2 le portiamo
  // nell'intervallo 0 - space_length.
  double const shifted_coordinate =
      coordinate + space_length / 2.0;

  // Dividendo per la lunghezza dello spazio otteniamo
  // una coordinata normalizzata compresa tra 0 e 1.
  double const normalized_coordinate =
      shifted_coordinate / space_length;

  // Moltiplichiamo infine per la dimensione della finestra.
  double const pixel_coordinate =
      normalized_coordinate * static_cast<double>(window_length);

  return static_cast<float>(pixel_coordinate);
}

bool Renderer::is_open() const
{
  // La simulazione grafica continua finché entrambe
  // le finestre rimangono aperte.
  return window_xy_.isOpen() && window_yz_.isOpen();
}

void Renderer::handle_events(sf::RenderWindow& window)
{
  sf::Event event;

  // pollEvent esamina tutti gli eventi in attesa.
  while (window.pollEvent(event)) {

    // Se l'utente preme il pulsante X, chiudiamo la finestra.
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
}

void Renderer::process_events()
{
  // Gli eventi delle due finestre devono essere controllati
  // separatamente.
  handle_events(window_xy_);
  handle_events(window_yz_);
}

void Renderer::draw_xy(
    Flock const& flock,
    std::vector<Predator> const& predators)
{
  sf::Vector2u const window_size = window_xy_.getSize();

  // Disegno di tutti i boids.
  for (Boid const& boid : flock.boids()) {
    Vector3 const& position = boid.position();

    // Nella proiezione XY:
    // - x viene rappresentata sull'asse orizzontale;
    // - y viene rappresentata sull'asse verticale.
    float const pixel_x =
        coordinate_to_pixel(position.x, Lx_, window_size.x);

    float const pixel_y =
        coordinate_to_pixel(position.y, Ly_, window_size.y);

    boid_shape_.setPosition(pixel_x, pixel_y);
    window_xy_.draw(boid_shape_);
  }

  // Disegno di tutti i predatori.
  for (Predator const& predator : predators) {
    Vector3 const& position = predator.position();

    float const pixel_x =
        coordinate_to_pixel(position.x, Lx_, window_size.x);

    float const pixel_y =
        coordinate_to_pixel(position.y, Ly_, window_size.y);

    predator_shape_.setPosition(pixel_x, pixel_y);
    window_xy_.draw(predator_shape_);
  }
}

void Renderer::draw_yz(
    Flock const& flock,
    std::vector<Predator> const& predators)
{
  sf::Vector2u const window_size = window_yz_.getSize();

  // Disegno di tutti i boids.
  for (Boid const& boid : flock.boids()) {
    Vector3 const& position = boid.position();

    // Nella proiezione YZ:
    // - y viene rappresentata sull'asse orizzontale;
    // - z viene rappresentata sull'asse verticale.
    float const pixel_y =
        coordinate_to_pixel(position.y, Ly_, window_size.x);

    float const pixel_z =
        coordinate_to_pixel(position.z, Lz_, window_size.y);

    boid_shape_.setPosition(pixel_y, pixel_z);
    window_yz_.draw(boid_shape_);
  }

  // Disegno di tutti i predatori.
  for (Predator const& predator : predators) {
    Vector3 const& position = predator.position();

    float const pixel_y =
        coordinate_to_pixel(position.y, Ly_, window_size.x);

    float const pixel_z =
        coordinate_to_pixel(position.z, Lz_, window_size.y);

    predator_shape_.setPosition(pixel_y, pixel_z);
    window_yz_.draw(predator_shape_);
  }
}

void Renderer::draw(
    Flock const& flock,
    std::vector<Predator> const& predators)
{
  // Cancella il fotogramma precedente riempiendo
  // entrambe le finestre di nero.
  window_xy_.clear(sf::Color::Black);
  window_yz_.clear(sf::Color::Black);

  // Disegna la configurazione corrente nelle due proiezioni.
  draw_xy(flock, predators);
  draw_yz(flock, predators);

  // Rende visibile il nuovo fotogramma.
  window_xy_.display();
  window_yz_.display();
}

} // namespace boids