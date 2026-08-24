#include "render.hpp"

#include <cstddef>
#include <iomanip>
#include <sstream>

namespace boids {

/* Il namespace anonimo rende le costanti seguenti
accessibili solamente all'interno di render.cpp.
In questo modo le costanti non sono visibili
negli altri file del progetto*/
namespace {

/*Distanza, espressa in pixel, fra i bordi dei pannelli
e i bordi esterni della finestra*/
constexpr float margin = 30.F;
/* Raggio, espresso in pixel, del cerchio utilizzato per rappresentare ciascun boid*/
constexpr float boid_radius = 4.F;

/*Colore interno dei boid*/
sf::Color const boid_color{
    80,
    210,
    255
};

/*Colore contorno dei boid*/
sf::Color const boid_outline_color{
    225,
    245,
    255
};

/*Colore per i bordi*/
sf::Color const border_color{
    70,
    85,
    105
};

/*Colore per lo sfondo*/
sf::Color const background_color{
    12,
    18,
    28
};

} 
/*Costruttore della classe Render.
 width:larghezza della finestra in pixel
 height: altezza della finestra in pixel
 space: spazio toroidale della simulazione
 Il costruttore crea la finestra 
 */
Render::Render(
    unsigned int width,
    unsigned int height,
    ToroidalSpace const& space)

 /*window_ viene costruita specificando:
    - larghezza e altezza
    - titolo iniziale della finestra*/
    : window_{
          sf::VideoMode{width, height},
          "Boids - piano XY e piano YZ"},

/* Memorizza un riferimento costante allo stesso spazio toroidale utilizzato dalla simulazione*/
      space_{space},

/*costruzione dei bordi attraverso i loro vertici */
      border_xy_{sf::LineStrip, 5},
      border_yz_{sf::LineStrip, 5},
      central_line_{sf::Lines, 2}
{
/* Limita il programma a 60 fotogrammi al secondo.*/
    window_.setFramerateLimit(60);

/*questo non ho ben capito cosa faccia, 
l'oracolo dice: Impedisce che mantenere premuto un tasto generi numerosi eventi KeyPressed consecutivi*/
    window_.setKeyRepeatEnabled(false);

  /* SFML utilizza valori float per rappresentare le coordinate grafiche.
     width e height vengono quindi convertiti
     esplicitamente da unsigned int a float*/
    float const window_width =
        static_cast<float>(width);

    float const window_height =
        static_cast<float>(height);

/*calcola la metà della larghezza della finestra*/
    float const half_width =
        window_width / 2.F;

    /* Costruzione del bordo del piano XY, nella metà sinistra*/

    border_xy_[0].position = {
        margin,
        margin};

    border_xy_[1].position = {
        half_width - margin,
        margin};

    border_xy_[2].position = {
        half_width - margin,
        window_height - margin};

    border_xy_[3].position = {
        margin,
        window_height - margin};

    border_xy_[4].position =
        border_xy_[0].position;

    /*Costruzione del bordo del piano YZ, nella metà destra*/

    border_yz_[0].position = {
        half_width + margin,
        margin};

    border_yz_[1].position = {
        window_width - margin,
        margin};

    border_yz_[2].position = {
        window_width - margin,
        window_height - margin};

    border_yz_[3].position = {
        half_width + margin,
        window_height - margin};

    border_yz_[4].position =
        border_yz_[0].position;

    /* Imposta lo stesso colore per entrambi i bordi*/

    for (std::size_t i = 0;
         i < border_xy_.getVertexCount();
         ++i) {
        border_xy_[i].color = border_color;
        border_yz_[i].color = border_color;
    }

      /*Costruzione della linea verticale centrale.
       Il primo vertice viene collocato al centro
       del bordo superiore della finestra*/
    central_line_[0].position = {
        half_width,
        0.F};

    /*Il secondo vertice viene collocato al centro
      del bordo inferiore della finestra*/
    central_line_[1].position = {
        half_width,
        window_height};

    /* Assegna il colore ai due vertici della linea centrale*/
    central_line_[0].color = border_color;
    central_line_[1].color = border_color;
}

/*Controlla se la finestra è ancora aperta.
Restituisce:
- true se la finestra è aperta;
- false se la finestra è stata chiusa*/
bool Render::is_open() const
{
    return window_.isOpen();
}

/* Legge e gestisce gli eventi della finestra.
La funzione permette di:
- chiudere la finestra con il suo pulsante;
- chiudere la finestra premendo Esc;
- segnalare la pressione della barra spaziatrice.
Restituisce true quando viene premuta la barra spaziatrice.
 */
bool Render::process_events()
{
    /*Oggetto nel quale SFML inserisce temporaneamente ogni evento*/
    sf::Event event{};

    /*Questa variabile diventa true quando viene premuta la barra spaziatrice*/
    bool toggle_pause = false;
    /*pollEvent() estrae un evento dalla coda. Il ciclo continua finché sono presenti eventi ancora da elaborare*/
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window_.close();
            }

            if (event.key.code == sf::Keyboard::Space) {
                toggle_pause = true;
            }
        }
    }

    return toggle_pause;
}

sf::Vector2f Render::screen_position(
    Vector3 const& position,
    Plane plane) const
{
    sf::Vector2u const window_size =
        window_.getSize();

    double const window_width =
        static_cast<double>(window_size.x);

    double const window_height =
        static_cast<double>(window_size.y);

    double const half_width =
        window_width / 2.;

    /*Ogni piano utilizza metà della larghezza totale della finestra*/
    double const usable_width =
        half_width -
        2. * static_cast<double>(margin);

    double const usable_height =
        window_height -
        2. * static_cast<double>(margin);

    double horizontal_coordinate{};
    double vertical_coordinate{};

    double horizontal_length{};
    double vertical_length{};

    double panel_offset{};

    if (plane == Plane::XY) {
        /*Piano XY:
        coordinata orizzontale = x
        coordinata verticale   = y*/
        horizontal_coordinate =
            position.x;

        vertical_coordinate =
            position.y;

        horizontal_length =
            space_.Lx();

        vertical_length =
            space_.Ly();

        panel_offset = 0.;
    } else {
        /*Piano YZ:
        coordinata orizzontale = y
        coordinata verticale   = z*/
        horizontal_coordinate =
            position.y;

        vertical_coordinate =
            position.z;

        horizontal_length =
            space_.Ly();

        vertical_length =
            space_.Lz();

        panel_offset = half_width;
    }

    /*Porta la coordinata orizzontale nell'intervallo compreso fra 0 e 1*/
    double const normalized_horizontal =
        horizontal_coordinate /
            horizontal_length +
        0.5;

    /*Porta la coordinata verticale nell'intervallo compreso fra 0 e 1.
    Il segno viene invertito perché in SFML l'asse
    verticale dello schermo è diretto verso il basso*/
    double const normalized_vertical =
        0.5 -
        vertical_coordinate /
            vertical_length;

    float const screen_x =
        static_cast<float>(
            panel_offset +
            static_cast<double>(margin) +
            normalized_horizontal *
                usable_width);

    float const screen_y =
        static_cast<float>(
            static_cast<double>(margin) +
            normalized_vertical *
                usable_height);

    return {
        screen_x,
        screen_y};
}

void Render::draw_boid(
    Boid const& boid,
    Plane plane)
{
    /* Converte la posizione tridimensionale del boid nella sua posizione sul piano scelto*/
    sf::Vector2f const centre =
        screen_position(
            boid.position(),
            plane);

    /*Il boid viene rappresentato come un punto circolare*/
    sf::CircleShape point{
        boid_radius};

    /*L'origine del cerchio viene spostata al centro.
    In questo modo setPosition() indica il centro
   del punto, non il suo angolo superiore sinistro*/
    point.setOrigin(
        boid_radius,
        boid_radius);

    point.setPosition(centre);

    /*Tutti i boid hanno lo stesso colore*/
    point.setFillColor(
        boid_color);

    point.setOutlineColor(
        boid_outline_color);

    point.setOutlineThickness(
        1.F);

    window_.draw(point);
}

void Render::update_title(
    Flock const& flock,
    bool paused,
    double simulation_time)
{
    std::ostringstream title;

    title
        << "Sinistra: piano XY"
        << " | Destra: piano YZ"
        << " | Boids: "
        << flock.boids().size()
        << " | t = "
        << std::fixed
        << std::setprecision(1)
        << simulation_time
        << " s";

    if (paused) {
        title << " | PAUSA";
    }

    title
        << " | SPAZIO: pausa"
        << " | ESC: esci";

    window_.setTitle(
        title.str());
}

void Render::draw(
    Flock const& flock,
    bool paused,
    double simulation_time)
{
    /*Pulisce la finestra prima di disegnare il nuovo fotogramma*/
    window_.clear(
        background_color);

    /*Disegna i bordi dei due piani e la linea di separazione centrale*/
    window_.draw(
        border_xy_);

    window_.draw(
        border_yz_);

    window_.draw(
        central_line_);

    /*Disegna tutti i boid sul piano XY*/
    for (Boid const& boid : flock.boids()) {
        draw_boid(
            boid,
            Plane::XY);
    }

    /*Disegna gli stessi boid sul piano YZ*/
    for (Boid const& boid : flock.boids()) {
        draw_boid(
            boid,
            Plane::YZ);
    }

    update_title(
        flock,
        paused,
        simulation_time);

    /*Mostra il fotogramma appena disegnato*/
    window_.display();
}

}  // namespace boids