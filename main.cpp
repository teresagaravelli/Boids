/*MAIN MASSIMO 
#include "Predator.hpp"
#include "ToroidalSpace.hpp"
#include "flock.hpp"
#include "wild.hpp"
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

int main()
{
  int number_boids{};
  int number_predators{};

  double d{};
  double ds{};
  double s{};
  double a{};
  double c{};

  double max_speed{50};

  boids::ToroidalSpace space{1000., 1000., 1000.};

  double dt{0.05};
  int steps{};

  std::cout << "Boids'number: ";
  std::cin >> number_boids;

  std::cout << "Predators' number: ";
  std::cin >> number_predators;

  std::cout << "Perception distance d (0 < d): "; // da rendere tutto in inglese
  std::cin >> d;

  std::cout << "Separation distance ds (0 < ds << d): ";
  std::cin >> ds;

  std::cout << "Separation factor (0 < s): ";
  std::cin >> s;

  std::cout << "Alignement factor a (0 < a): ";
  std::cin >> a;

  std::cout << "Cohesion factor c (0 < c): ";
  std::cin >> c;

  std::cout
      << "Numero di passi temporali: "; // quante volte si aggiorna il sistema
  std::cin >> steps;

  // CONTROLLO INPUT

  if (std::cin.fail()) {
    std::cerr << "Error: invalid input.\n";
    return 1;
  }

  for (std::size_t i = 0; i <= 3; ++i) {
    if (number_boids >= 2 && steps >0 && ds > 0 && d > 0 && s > 0 && a > 0 && c > 0 && ds < 0.3*d) {
        break; // se va tutto bene il ciclo si interrompe e il programma prosegue
    }
    if (i == 3) {
      std::cerr << "Error: too many attempts.\n";
      return 1; // fallisce dopo che l'input è sbagliato al terzo tentativo
    }

    if (number_boids < 2) {
      std::cout << "Error, at least two boids are needed, insert the "
                   "parameter again: ";
      std::cin >> number_boids;
    }

    if (d <= 0.) {
      std::cout
          << "Error, d must be greater than 0, insert the parameter again: ";
      std::cin >> d;
    }

    if (ds <= 0. || ds > 0.3 * d) {
      std::cout << "Error, ds must be greater than 0 and way lower than d, "
                   "insert the parameter again: ";
      std::cin >> ds;
    }

    if (s <= 0.) {
      std::cout
          << "Error, s must be greater than 0, insert the parameter again: ";
      std::cin >> s;
    }
    if (a <= 0.) {
      std::cout
          << "Error, a must be greater than 0, insert the parameter again: ";
      std::cin >> a;
    }

    if (c <= 0.) {
      std::cout
          << "Error, c must be greater than 0, insert the parameter again: ";
      std::cin >> c;
    }

    if (steps <= 0) {
      std::cout << "Error, number of steps must be greater than 0, insert the "
                   "parameter again: ";
      std::cin >> steps;
    }
  }
  // GENERATORE DI NUMERI CASUALI

  std::random_device rd;
  std::default_random_engine generator{rd()};

  std::uniform_real_distribution<double> random_boid_x{-space.Lx() / 2.,
                                                       space.Lx() / 2.};
  std::uniform_real_distribution<double> random_boid_y{-space.Ly() / 2.,
                                                       space.Ly() / 2.};
  std::uniform_real_distribution<double> random_boid_z{-space.Lz() / 2.,
                                                       space.Lz() / 2.};

  std::uniform_real_distribution<double> random_velocity_boid{
      -max_speed / std::sqrt(3.),
      max_speed
          / std::sqrt(
              3.)}; // così il modulo max dell'oggetto è pari a max_speed

  std::uniform_real_distribution<double> random_predator_x{-space.Lx() / 2.,
                                                           space.Lx() / 2.};
  std::uniform_real_distribution<double> random_predator_y{-space.Ly() / 2.,
                                                           space.Ly() / 2.};
  std::uniform_real_distribution<double> random_predator_z{-space.Lz() / 2.,
                                                           space.Lz() / 2.};

  std::uniform_real_distribution<double> random_velocity_predator{
      -75 / std::sqrt(3.), 75 / std::sqrt(3.)};

  // CREAZIONE DEI BOID

  std::vector<boids::Boid> initial_boids;
  initial_boids.reserve(static_cast<std::size_t>(
      number_boids)); // capacità del vettore, la dimensione è 0 però. Reserve
                      // preferisce un contatore ad un int, dettaglio per
                      // togliere il warning

  std::vector<boids::Predator> initial_predators;
  initial_predators.reserve(static_cast<std::size_t>(number_predators));

  for (int i = 0; i < number_boids; ++i) {
    boids::Vector3 position{random_boid_x(generator), random_boid_y(generator),
                            random_boid_z(generator)};

    boids::Vector3 velocity{
        random_velocity_boid(generator), random_velocity_boid(generator),
        random_velocity_boid(
            generator)}; // Evita che la velocita iniziale superi max_speed
    initial_boids.emplace_back(position, velocity, 2.0 * M_PI,
                               max_speed, space); 
  }

  for (int i = 0; i < number_predators; ++i) {
    boids::Vector3 position{random_predator_x(generator),
                            random_predator_y(generator),
                            random_predator_z(generator)};

    boids::Vector3 velocity{
        random_velocity_predator(generator),
        random_velocity_predator(generator),
        random_velocity_predator(
            generator)}; // Evita che la velocita iniziale superi max_speed
    initial_predators.emplace_back(position, velocity, 4.0 * M_PI / 9.0,
                                   true, space); // serve ad aggiungere i piccioni al
                                          // vettore di piccioni da sinistra(?)
  }

  // CREAZIONE DELLO STORMO
  boids::Flock flock{initial_boids, d, ds, s, a, c, space};
  boids::Wild Wild{flock, initial_predators, space};

  // SIMULAZIONE

  std::cout << std::fixed
            << std::setprecision(4); // usa la notazione decimale normale, e non
                                     // scientifica (aka e04 etc.), ed i numeri
                                     // stampati hanno 4 cifre dopo la virgola

  std::cout << "\n"; // stampo prima perché così associo al tempo 0 la
                     // generazione iniziale
  std::cout << "Time: "
            << "Mean distance: "
            << "Std deviation of distance: "
            << "Mean speed: "
            << "Std deviation of speed:"
            << " Alive boids:\n";

  for (int step = 0; step <= steps; ++step) {
    double const time = step * dt;

    std::cout << time << ' ' << flock.mean_distance() << ' '
              << flock.distance_stddev() << ' ' << flock.mean_speed() << ' '
              << flock.speed_stddev() << ' ' << flock.boids().size() << '\n';

    if (step < steps) {
      Wild.update_all(dt);
    }
  }

  return 0;
}*/



/*MAIN MASSIMO + GRAFICA 
#include "Predator.hpp"
#include "ToroidalSpace.hpp"
#include "flock.hpp"
#include "render.hpp"
#include "wild.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

int main()
{
  int number_boids{};
  int number_predators{};

  double d{};
  double ds{};
  double s{};
  double a{};
  double c{};

  double max_speed{50};

  boids::ToroidalSpace space{1000., 1000., 1000.};

  double dt{0.05};
  int steps{};

  // LETTURA DEGLI INPUT

  std::cout << "Boids'number: ";
  std::cin >> number_boids;

  std::cout << "Predators' number: ";
  std::cin >> number_predators;

  std::cout << "Perception distance d (0 < d): ";
  std::cin >> d;

  std::cout << "Separation distance ds (0 < ds << d): ";
  std::cin >> ds;

  std::cout << "Separation factor (0 < s): ";
  std::cin >> s;

  std::cout << "Alignement factor a (0 < a): ";
  std::cin >> a;

  std::cout << "Cohesion factor c (0 < c): ";
  std::cin >> c;

  std::cout << "Numero di passi temporali: ";
  std::cin >> steps;

  // CONTROLLO DEGLI INPUT

  if (std::cin.fail()) {
    std::cerr << "Error: invalid input.\n";
    return 1;
  }

  for (std::size_t i = 0; i <= 3; ++i) {
    if (number_boids >= 2
        && steps > 0
        && ds > 0
        && d > 0
        && s > 0
        && a > 0
        && c > 0
        && ds < 0.3 * d) {

      break;
    }

    if (i == 3) {
      std::cerr << "Error: too many attempts.\n";
      return 1;
    }

    if (number_boids < 2) {
      std::cout
          << "Error, at least two boids are needed, "
          << "insert the parameter again: ";

      std::cin >> number_boids;
    }

    if (d <= 0.) {
      std::cout
          << "Error, d must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> d;
    }

    if (ds <= 0. || ds > 0.3 * d) {
      std::cout
          << "Error, ds must be greater than 0 and way lower than d, "
          << "insert the parameter again: ";

      std::cin >> ds;
    }

    if (s <= 0.) {
      std::cout
          << "Error, s must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> s;
    }

    if (a <= 0.) {
      std::cout
          << "Error, a must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> a;
    }

    if (c <= 0.) {
      std::cout
          << "Error, c must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> c;
    }

    if (steps <= 0) {
      std::cout
          << "Error, number of steps must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> steps;
    }
  }

  // GENERATORE DI NUMERI CASUALI

  std::random_device rd;
  std::default_random_engine generator{rd()};

  std::uniform_real_distribution<double> random_boid_x{
      -space.Lx() / 2.,
      space.Lx() / 2.
  };

  std::uniform_real_distribution<double> random_boid_y{
      -space.Ly() / 2.,
      space.Ly() / 2.
  };

  std::uniform_real_distribution<double> random_boid_z{
      -space.Lz() / 2.,
      space.Lz() / 2.
  };

  std::uniform_real_distribution<double> random_velocity_boid{
      -max_speed / std::sqrt(3.),
      max_speed / std::sqrt(3.)
  };

  std::uniform_real_distribution<double> random_predator_x{
      -space.Lx() / 2.,
      space.Lx() / 2.
  };

  std::uniform_real_distribution<double> random_predator_y{
      -space.Ly() / 2.,
      space.Ly() / 2.
  };

  std::uniform_real_distribution<double> random_predator_z{
      -space.Lz() / 2.,
      space.Lz() / 2.
  };

  std::uniform_real_distribution<double> random_velocity_predator{
      -75 / std::sqrt(3.),
      75 / std::sqrt(3.)
  };

  // CREAZIONE DEI BOIDS

  std::vector<boids::Boid> initial_boids;

  initial_boids.reserve(
      static_cast<std::size_t>(number_boids));

  std::vector<boids::Predator> initial_predators;

  initial_predators.reserve(
      static_cast<std::size_t>(number_predators));

  for (int i = 0; i < number_boids; ++i) {
    boids::Vector3 position{
        random_boid_x(generator),
        random_boid_y(generator),
        random_boid_z(generator)
    };

    boids::Vector3 velocity{
        random_velocity_boid(generator),
        random_velocity_boid(generator),
        random_velocity_boid(generator)
    };

    initial_boids.emplace_back(
        position,
        velocity,
        2.0 * M_PI/3,
        max_speed,
        space
    );
  }

  // CREAZIONE DEI PREDATORI

  for (int i = 0; i < number_predators; ++i) {
    boids::Vector3 position{
        random_predator_x(generator),
        random_predator_y(generator),
        random_predator_z(generator)
    };

    boids::Vector3 velocity{
        random_velocity_predator(generator),
        random_velocity_predator(generator),
        random_velocity_predator(generator)
    };

    initial_predators.emplace_back(
        position,
        velocity,
        4.0 * M_PI / 9.0,
        true,
        space
    );
  }

  // CREAZIONE DELLO STORMO

  boids::Flock flock{
      initial_boids,
      d,
      ds,
      s,
      a,
      c,
      space
  };

  // CREAZIONE DELL'AMBIENTE CON I PREDATORI

  boids::Wild Wild{
      flock,
      initial_predators,
      space
  };

  // CREAZIONE DELLE DUE FINESTRE

  boids::Renderer renderer{
      space.Lx(),
      space.Ly(),
      space.Lz()
  };

  // PREPARAZIONE DELLA STAMPA

  std::cout
      << std::fixed
      << std::setprecision(4);

  std::cout << '\n';

  std::cout
      << "Time: "
      << "Mean distance: "
      << "Std deviation of distance: "
      << "Mean speed: "
      << "Std deviation of speed:"
      << " Alive boids:\n";

  // SIMULAZIONE GRAFICA

  int step{0};

  while (renderer.is_open() && step <= steps) {
    // Controlla gli eventi ricevuti dalle due finestre.
    renderer.process_events();

    if (!renderer.is_open()) {
      break;
    }

    double const time =
        static_cast<double>(step) * dt;

    // Stampa le statistiche relative allo stato corrente.
    std::cout
        << time << ' '
        << flock.mean_distance() << ' '
        << flock.distance_stddev() << ' '
        << flock.mean_speed() << ' '
        << flock.speed_stddev() << ' '
        << flock.boids().size() << '\n';

    // Disegna boids e predatori nelle due proiezioni.
    renderer.draw(
        flock,
        Wild.predators()
    );

    // Aggiorna la simulazione a ogni iterazione.
    if (step < steps) {
      Wild.update_all(dt);
    }

    ++step;
  }

  return 0;
}*/

//MAIN MASSIMO+GRAFICA+FILES
#include "Predator.hpp"
#include "ToroidalSpace.hpp"
#include "flock.hpp"
#include "render.hpp"
#include "wild.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

int main()
{
  int number_boids{};
  int number_predators{};

  double d{};
  double ds{};
  double s{};
  double a{};
  double c{};

  double max_speed{50};

  boids::ToroidalSpace space{1000., 1000., 1000.};

  double dt{0.05};
  int steps{};

  std::cout << "Boids'number: ";
  std::cin >> number_boids;

  std::cout << "Predators' number: ";
  std::cin >> number_predators;

  std::cout << "Perception distance d (0 < d): ";
  std::cin >> d;

  std::cout << "Separation distance ds (0 < ds << d): ";
  std::cin >> ds;

  std::cout << "Separation factor (0 < s): ";
  std::cin >> s;

  std::cout << "Alignement factor a (0 < a): ";
  std::cin >> a;

  std::cout << "Cohesion factor c (0 < c): ";
  std::cin >> c;

  std::cout << "Numero di passi temporali: ";
  std::cin >> steps;

  // CONTROLLO INPUT

  if (std::cin.fail()) {
    std::cerr << "Error: invalid input.\n";
    return 1;
  }

  for (std::size_t i = 0; i <= 3; ++i) {
    if (number_boids >= 2
        && steps > 0
        && ds > 0
        && d > 0
        && s > 0
        && a > 0
        && c > 0
        && ds < 0.3 * d) {

      break;
    }

    if (i == 3) {
      std::cerr << "Error: too many attempts.\n";
      return 1;
    }

    if (number_boids < 2) {
      std::cout
          << "Error, at least two boids are needed, "
          << "insert the parameter again: ";

      std::cin >> number_boids;
    }

    if (d <= 0.) {
      std::cout
          << "Error, d must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> d;
    }

    if (ds <= 0. || ds > 0.3 * d) {
      std::cout
          << "Error, ds must be greater than 0 and way lower than d, "
          << "insert the parameter again: ";

      std::cin >> ds;
    }

    if (s <= 0.) {
      std::cout
          << "Error, s must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> s;
    }

    if (a <= 0.) {
      std::cout
          << "Error, a must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> a;
    }

    if (c <= 0.) {
      std::cout
          << "Error, c must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> c;
    }

    if (steps <= 0) {
      std::cout
          << "Error, number of steps must be greater than 0, "
          << "insert the parameter again: ";

      std::cin >> steps;
    }
  }

  // GENERATORE DI NUMERI CASUALI

  std::random_device rd;
  std::default_random_engine generator{rd()};

  std::uniform_real_distribution<double> random_boid_x{
      -space.Lx() / 2.,
      space.Lx() / 2.
  };

  std::uniform_real_distribution<double> random_boid_y{
      -space.Ly() / 2.,
      space.Ly() / 2.
  };

  std::uniform_real_distribution<double> random_boid_z{
      -space.Lz() / 2.,
      space.Lz() / 2.
  };

  std::uniform_real_distribution<double> random_velocity_boid{
      -max_speed / std::sqrt(3.),
      max_speed / std::sqrt(3.)
  };

  std::uniform_real_distribution<double> random_predator_x{
      -space.Lx() / 2.,
      space.Lx() / 2.
  };

  std::uniform_real_distribution<double> random_predator_y{
      -space.Ly() / 2.,
      space.Ly() / 2.
  };

  std::uniform_real_distribution<double> random_predator_z{
      -space.Lz() / 2.,
      space.Lz() / 2.
  };

  std::uniform_real_distribution<double> random_velocity_predator{
      -75 / std::sqrt(3.),
      75 / std::sqrt(3.)
  };

  // CREAZIONE DEI BOIDS

  std::vector<boids::Boid> initial_boids;

  initial_boids.reserve(
      static_cast<std::size_t>(number_boids));

  std::vector<boids::Predator> initial_predators;

  initial_predators.reserve(
      static_cast<std::size_t>(number_predators));

  for (int i = 0; i < number_boids; ++i) {
    boids::Vector3 position{
        random_boid_x(generator),
        random_boid_y(generator),
        random_boid_z(generator)
    };

    boids::Vector3 velocity{
        random_velocity_boid(generator),
        random_velocity_boid(generator),
        random_velocity_boid(generator)
    };

    initial_boids.emplace_back(
        position,
        velocity,
        2.0 * M_PI/3,
        max_speed,
        space
    );
  }

  // CREAZIONE DEI PREDATORI

  for (int i = 0; i < number_predators; ++i) {
    boids::Vector3 position{
        random_predator_x(generator),
        random_predator_y(generator),
        random_predator_z(generator)
    };

    boids::Vector3 velocity{
        random_velocity_predator(generator),
        random_velocity_predator(generator),
        random_velocity_predator(generator)
    };

    initial_predators.emplace_back(
        position,
        velocity,
        4.0 * M_PI / 9.0,
        true,
        space
    );
  }

  // CREAZIONE DELLO STORMO E DELL'AMBIENTE

  boids::Flock flock{
      initial_boids,
      d,
      ds,
      s,
      a,
      c,
      space
  };

  boids::Wild Wild{
      flock,
      initial_predators,
      space
  };

  // CREAZIONE DELLE DUE FINESTRE GRAFICHE

  boids::Renderer renderer{
      space.Lx(),
      space.Ly(),
      space.Lz()
  };

  // APERTURA DEI FILE CSV

  // velocities.csv conterrà due colonne:
  // time e modulo della velocità.
  std::ofstream velocity_file{
      "velocities.csv",
      std::ios::out | std::ios::trunc
  };

  // distances.csv conterrà due colonne:
  // time e distanza reciproca.
  std::ofstream distance_file{
      "distances.csv",
      std::ios::out | std::ios::trunc
  };

  if (!velocity_file.is_open()) {
    std::cerr << "Error: unable to open velocities.csv\n";
    return 1;
  }

  if (!distance_file.is_open()) {
    std::cerr << "Error: unable to open distances.csv\n";
    return 1;
  }

  velocity_file
      << std::fixed
      << std::setprecision(6);

  distance_file
      << std::fixed
      << std::setprecision(6);

  velocity_file << "time,velocity\n";
  distance_file << "time,distance\n";

  // SIMULAZIONE

  std::cout
      << std::fixed
      << std::setprecision(4);

  std::cout << "\n";

  std::cout
      << "Time: "
      << "Mean distance: "
      << "Std deviation of distance: "
      << "Mean speed: "
      << "Std deviation of speed:"
      << " Alive boids:\n";

  int step{0};

  // Con dt = 0.05, 100 passi corrispondono
  // a 5 secondi simulati.
  int const data_save_period{100};

  while (renderer.is_open() && step <= steps) {
  renderer.process_events();

  if (!renderer.is_open()) {
    break;
  }

  double const time =
      static_cast<double>(step) * dt;

  // Solo il salvataggio è condizionato.
  if (step % data_save_period == 0) {
    std::vector<boids::Boid> const& current_boids =
        flock.boids();

    // Salvataggio velocità.
    for (std::size_t i = 0;
         i < current_boids.size();
         ++i) {

      double const velocity =
          boids::norm(current_boids[i].velocity());

      velocity_file
          << time << ','
          << velocity << '\n';
    }

    // Salvataggio distanze.
    for (std::size_t i = 0;
         i < current_boids.size();
         ++i) {

      for (std::size_t j = i + 1;
           j < current_boids.size();
           ++j) {

        boids::Vector3 const displacement =
            space.toroidal_displacement(
                current_boids[i].position(),
                current_boids[j].position());

        double const distance =
            boids::norm(displacement);

        distance_file
            << time << ','
            << distance << '\n';
      }
    }
  } // Qui deve terminare l'if del salvataggio.

  // Il disegno avviene a ogni passo.
  renderer.draw(
      flock,
      Wild.predators()
  );

  // Anche l'aggiornamento deve avvenire a ogni passo,
  // quindi deve stare fuori dall'if precedente.
  if (step < steps) {
    Wild.update_all(dt);
  }

  // Anche questo deve stare fuori dall'if del salvataggio.
  ++step;
}
  // CHIUSURA DEI FILE

  velocity_file.close();
  distance_file.close();

  std::cout
      << "Velocities saved in velocities.csv\n";

  std::cout
      << "Distances saved in distances.csv\n";

  return 0;
}