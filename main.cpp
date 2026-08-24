#include "flock.hpp"
#include "ToroidalSpace.hpp"
#include "render.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <random>
#include <vector>

int main()
{
    int number_boids{};

    double d{};
    double ds{};
    double s{};
    double a{};
    double c{};

    double max_speed{};

    // double Lx{};
    // double Ly{};
    // double Lz{};

    boids::ToroidalSpace space{};

    double dt{};
    int steps{};

    std::cout << "Boid's number: ";
    std::cin >> number_boids;

    std::cout << "Distanza di percezione d: ";
    std::cin >> d;

    std::cout << "Distanza di separazione ds: ";
    std::cin >> ds;

    std::cout << "Fattore di separazione s: ";
    std::cin >> s;

    std::cout << "Fattore di allineamento a: ";
    std::cin >> a;

    std::cout << "Fattore di coesione c: ";
    std::cin >> c;

    std::cout << "Velocita massima: ";
    std::cin >> max_speed;

    // std::cout << "Dimensione Lx: ";
    // std::cin >> Lx;

    // std::cout << "Dimensione Ly: ";
    // std::cin >> Ly;

    // std::cout << "Dimensione Lz: ";
    // std::cin >> Lz;

    std::cout << "Intervallo temporale dt: ";
    std::cin >> dt;

    std::cout << "Numero di passi temporali: ";
    std::cin >> steps;

    // CONTROLLO INPUT

    if (std::cin.fail()) {
        std::cerr << "Errore: input non valido.\n";
        return 1;
    }

    if (number_boids < 2) {
        std::cerr << "Errore: servono almeno due boid.\n";
        return 1;
    }

    if (d <= 0. || s < 0. || a < 0. || a >= 1. || c < 0.) {
        std::cerr
            << "Errore: almeno uno dei parametri d, s, a, c "
            << "non e valido.\n";

        return 1;
    }

    if (ds <= 0. || ds >= d) {
        std::cerr << "Errore: deve essere 0 < ds < d.\n";
        return 1;
    }

    if (max_speed <= 0.) {
        std::cerr
            << "Errore: la velocita massima deve essere positiva.\n";

        return 1;
    }

    if (dt <= 0. || steps <= 0) {
        std::cerr
            << "Errore: dt e numero di passi devono essere positivi.\n";

        return 1;
    }

    // GENERATORE DI NUMERI CASUALI

    std::random_device rd;

    std::default_random_engine generator{
        rd()};

    std::uniform_real_distribution<double> random_x{
        -space.Lx() / 2.,
        space.Lx() / 2.};

    std::uniform_real_distribution<double> random_y{
        -space.Ly() / 2.,
        space.Ly() / 2.};

    std::uniform_real_distribution<double> random_z{
        -space.Lz() / 2.,
        space.Lz() / 2.};

    std::uniform_real_distribution<double> random_velocity{
        -max_speed / std::sqrt(3.),
        max_speed / std::sqrt(3.)};

    // CREAZIONE DEI BOID

    std::vector<boids::Boid> initial_boids;

    initial_boids.reserve(
        static_cast<std::size_t>(number_boids));

    for (int i = 0; i < number_boids; ++i) {
        boids::Vector3 position{
            random_x(generator),
            random_y(generator),
            random_z(generator)};

        boids::Vector3 velocity{
            random_velocity(generator),
            random_velocity(generator),
            random_velocity(generator)};

        initial_boids.emplace_back(
            position,
            velocity,
            std::acos(-1.0) / 3.0,
            max_speed);
    }

    // CREAZIONE DELLO STORMO

    boids::Flock flock{
        initial_boids,
        d,
        ds,
        s,
        a,
        c,
        space};

    // CREAZIONE DELLA FINESTRA GRAFICA

boids::Render render{
    1400,
    750,
    space};

    // SIMULAZIONE

    std::cout
        << std::fixed
        << std::setprecision(4);

    std::cout << '\n';

    std::cout
        << "time "
        << "mean_distance "
        << "distance_stddev "
        << "mean_speed "
        << "speed_stddev\n";

    int step = 0;
    bool paused = false;

    while (render.is_open()) {
        /*
         * Gestisce gli eventi della finestra.
         *
         * process_events() restituisce true quando viene
         * premuta la barra spaziatrice.
         */
        if (render.process_events()) {
            paused = !paused;
        }

        bool const simulation_finished =
            step > steps;

        /*
         * La simulazione viene aggiornata solamente se:
         *
         * 1. non è in pausa;
         * 2. non è stato raggiunto il numero massimo di passi.
         */
        if (!paused && !simulation_finished) {
            double const time =
                static_cast<double>(step) * dt;

            std::cout
                << time << ' '
                << flock.mean_distance() << ' '
                << flock.distance_stddev() << ' '
                << flock.mean_speed() << ' '
                << flock.speed_stddev() << '\n';

            /*
             * Disegna lo stato attuale prima di passare
             * al passo temporale successivo.
             */
            render.draw(
                flock,
                false,
                time);

            if (step < steps) {
                flock.update(dt);
            }

            ++step;
        } else {
            /*
             * Durante la pausa, oppure dopo la fine della
             * simulazione, continuiamo a disegnare la finestra.
             *
             * Questo evita che la finestra scompaia appena
             * viene raggiunto l'ultimo passo.
             */
            int const displayed_step =
                step > steps ? steps : step;

            double const time =
                static_cast<double>(displayed_step) * dt;

            render.draw(
                flock,
                true,
                time);
        }
    }

    return 0;
}