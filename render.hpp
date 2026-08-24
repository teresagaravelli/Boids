#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>

#include "ToroidalSpace.hpp"
#include "flock.hpp"

namespace boids {

class Render {    //questa classe gestisce: la finestra, gli eventi da tastiera (pausa e esc),la trasformazione delle coordinate, il disegno dei due piani, il disegno dei boid 
 private: 
    enum class Plane {  //Questa enumerazione permette di specificare su quale piano si vuole disegnare un boid
        XY,
        YZ
    };

    sf::RenderWindow window_; //E' la finestra nella quale viene gestita la simulazione

    ToroidalSpace const& space_;

    sf::VertexArray border_xy_;       //Questi due oggetti rappresentano i rettangoli che delimitano i piani 
    sf::VertexArray border_yz_;
    sf::VertexArray central_line_;   //Linea verticale che divide la finestra in due parti


    /* Converte una posizione tridimensionale di tipo Vector3
     in una posizione bidimensionale di tipo sf::Vector2f.
     position contiene le coordinate x, y e z del boid.
     plane specifica quale proiezione utilizzare:
     Il valore restituito contiene le coordinate in pixel
     La funzione è const perché non modifica gli attributi della classe Render.
     */
    sf::Vector2f screen_position( 
        Vector3 const& position,
        Plane plane) const;
    
    /* Disegna un singolo boid nel piano specificato.
    plane indica se il boid deve essere rappresentato
    nel piano xy oppure nel piano yz.
    La funzione legge la posizione del boid, la converte
    in coordinate grafiche e disegna un punto circolare
     */
    void draw_boid(
        Boid const& boid,
        Plane plane);

    /*Aggiorna il testo mostrato nella barra del titolodella finestra.
    flock viene utilizzato per ricavare il numero di boid.
    paused indica se la simulazione è in pausa.
    simulation_time rappresenta il tempo raggiunto dalla simulazione.
     */
    void update_title(
        Flock const& flock,
        bool paused,
        double simulation_time);

 public:
  /*Costruttore della classe Render.
  width indica la larghezza della finestra in pixel.
  height indica l'altezza della finestra in pixel.
  space è lo spazio toroidale utilizzato dalla simulazione.
  Il costruttore crea la finestra e inizializza i bordi */
    Render(
        unsigned int width,
        unsigned int height,
        ToroidalSpace const& space);

      /* Restituisce true se la finestra grafica è ancora aperta.
     Restituisce false dopo che la finestra è stata chiusa.
     La funzione è const perché controlla lo stato della
     finestra senza modificare l'oggetto Render.
     */
    bool is_open() const;
   
    /*Gestisce gli eventi generati dalla finestra e dalla tastiera.
    In particolare:
    - chiude la finestra quando viene premuto il pulsante di chiusura;
    - chiude la finestra quando viene premuto Esc;
    - restituisce true quando viene premuta la barra spaziatrice.
    Il valore true permette al main di attivare o disattivare la pausa della simulazione.
     */
    bool process_events();
 
    /*Disegna un fotogramma completo della simulazione.
    flock contiene tutti i boid da rappresentare.
    paused indica se la simulazione è in pausa.
    simulation_time rappresenta il tempo corrente della simulazione.
    La funzione:
    1. cancella il fotogramma precedente;
    2. disegna i bordi dei due piani;
    3. disegna la linea centrale;
    4. disegna i boid nel piano xy;
    5. disegna i boid nel piano yz;
    6. aggiorna il titolo della finestra;
    7. mostra il nuovo fotogramma.
     */
    void draw(
        Flock const& flock,
        bool paused,
        double simulation_time);
};

}

#endif