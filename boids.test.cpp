#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "wild.hpp"
#include <cmath>
#include <vector>

TEST_CASE ("Vector3 addition, norm and distance")
{
   boids::Vector3 const a{1., 2., 0.};
   boids::Vector3 const b{1., 1., 0.};

    auto const c = a + b;

    CHECK(c.x == doctest::Approx(2.));
    CHECK(c.y == doctest::Approx(3.));
    CHECK(c.z == doctest::Approx(0.));
    CHECK(boids::distance (a, b) == doctest::Approx(1.));
    CHECK(boids::norm (c) == doctest::Approx(3.60555));
}

TEST_CASE ("boids setting and refresh")
{
   boids::Vector3 const a{1., 2., 0.};
   boids::Vector3 const b{1., 1., 0.};
   boids::ToroidalSpace space{1000., 1000., 1000.};
   boids::Boid c{a, b, 1., 10., space}; 

    CHECK(c.position() == a); //è pericoloso usare == se si fa un calcolo in quanto si fanno dei calcoli su dei double, nel caso meglio usare l'approx
    CHECK(c.velocity() == b);
    CHECK(c.view_angle() == doctest::Approx(1.));
    CHECK(c.max_speed() == doctest::Approx(10.));
   
   c.set_position (b);
   c.set_velocity (a);

    CHECK (c.position() == b);
    CHECK (c.velocity() == a);

}

TEST_CASE ("Predator setting")
{
   boids::Vector3 const a{1., 2., 0.};
   boids::Vector3 const b{1., 1., 0.};
   boids::ToroidalSpace space{1000., 1000., 1000.};
   boids::Predator p{a, b, 1., true, space};
    
   CHECK(p.hungry() == true);//ho testato solo le robe che predator ha in piu rispetto a boid
   CHECK(p.timer() == doctest::Approx(0.));

   p.set_hungry(false);
   p.set_timer(3.);

   CHECK(p.hungry() == false);
   CHECK(p.timer() == doctest::Approx(3.));

}

TEST_CASE("flock update") {

    boids::ToroidalSpace space{1000., 1000., 1000.};
    double const max_speed = 10.;
    
    SUBCASE("update without neighbours") { //funzionamento base
        boids::Boid const bird{boids::Vector3{1., 1., 0.}, boids::Vector3{1., 0., 0.}, 1., max_speed, space}; //creo boid con posizione e velocità
        std::vector<boids::Boid> birds{bird}; //lo metto in un vector perchè flock vuole un vectro come primo parametro
        boids::Flock flock{ birds, 2., 1., 1., 1., 1., space};
        flock.update(1.); //l'argomento di update è il dt
        auto const& updated_bird = flock.boids()[0]; //recupera il boid dopo update: mi permette di vedere i boid nel flock creato
        CHECK(updated_bird.position().x == doctest::Approx(2.));
        CHECK(updated_bird.position().y == doctest::Approx(1.));
        CHECK(updated_bird.position().z == doctest::Approx(0.));
        
        CHECK(updated_bird.velocity().x == doctest::Approx(1.));
        CHECK(updated_bird.velocity().y == doctest::Approx(0.));
        CHECK(updated_bird.velocity().z == doctest::Approx(0.));
    }

    SUBCASE("update with neighbours inside view angle") { //controlla indirettamente separation, alignment, cohesion (quindi anche toroidal_displacement) e visible
        boids::Boid const bird1{boids::Vector3{0., 0., 0.}, boids::Vector3{1., 0., 0.}, 1., max_speed, space}; 
        boids::Boid const bird2{boids::Vector3{1., 0., 0.}, boids::Vector3{0., 0., 0.}, 1., max_speed, space};
        std::vector<boids::Boid> birds{bird1, bird2};
        boids::Flock flock{ birds, 5., 2., 1., 1., 1., space}; //d=5 in modo tale che siano considerati vicini, ds=2 per far entrare in gioco separation
        flock.update(1.);
        CHECK(flock.boids()[0].velocity().x == doctest::Approx(0.)); //con vel iniziale=1, separation=-1, alignment=-1, cohesion=1
        CHECK(flock.boids()[1].velocity().x !=  doctest::Approx(0.)); //piccione fermo vede tutto
    }

     SUBCASE("update with neighbours outside view angle") { //controlla indirettamente visible (boid dentro distanza minima dall'altro ma fuori angolo di visione)
        boids::Boid const bird1{boids::Vector3{0., 0., 0.}, boids::Vector3{1., 0., 0.}, 1., max_speed, space};
        boids::Boid const bird2{boids::Vector3{-1., 0., 0.}, boids::Vector3{0., 0., 0.}, 1., max_speed, space};
        std::vector<boids::Boid> birds{bird1, bird2};
        boids::Flock flock{ birds, 5., 0.5, 1., 1., 1., space}; //ds= 0.5 cosi non interviene separation
        flock.update(1.);
        CHECK(flock.boids()[0].velocity().x == doctest::Approx(1.));
        CHECK(flock.boids()[0].velocity().y == doctest::Approx(0.));
        CHECK(flock.boids()[0].velocity().z == doctest::Approx(0.));
    }

    SUBCASE("update across boundary") {//controlla indirettamente wrap_position e quindi anche toroidal_shortcut
       boids::Boid const bird{boids::Vector3{499., 0., 0.}, boids::Vector3{2., 0., 0.}, 1., max_speed, space}; 
       std::vector<boids::Boid> birds{bird};
       boids::Flock flock{ birds, 1., 1., 1., 1., 1., space};
       flock.update(1.);
       CHECK(flock.boids()[0].position().x == doctest::Approx(-499.));
    }

}

TEST_CASE("flock statistics") {
    double const max_speed = 10.;
    boids::ToroidalSpace space {1000., 1000., 1000.};
    boids::Boid const bird1{boids::Vector3{0., 0., 0.}, boids::Vector3{3., 0., 0.}, 1., max_speed, space};
    boids::Boid const bird2{boids::Vector3{2., 0., 0.}, boids::Vector3{1., 0., 0.}, 1., max_speed, space};
    std::vector<boids::Boid> birds{bird1, bird2};
    boids::Flock flock{ birds, 1., 1., 1., 1., 1., space}; 
    CHECK(flock.mean_speed() == doctest::Approx(2.));
    CHECK(flock.speed_stddev() == doctest::Approx(1.));
    CHECK(flock.mean_distance() == doctest::Approx(2.));
    CHECK(flock.distance_stddev() == doctest::Approx(0.));

}

TEST_CASE("wild"){
    boids::ToroidalSpace const space {1000., 1000., 1000.};

    SUBCASE("eaten"){
       {boids::Boid const b{{5., 0., 0.}, {1., 0., 0.}, 1., 10., space};//boid vicino, quindi viene mangiato
        std::vector<boids::Boid> birds{b};
        boids::Flock flock{birds, 100., 10., 1., 1., 1., space};
        boids::Predator const p{{0., 0., 0.}, {1., 0., 0.}, 1., true, space};
        std::vector<boids::Predator> predators{p};
        boids::Wild wild{flock, predators, space};
        wild.eaten();

        CHECK(wild.flock().boids().size() == 0);
        CHECK(wild.predators()[0].hungry() == false);}

       {boids::Boid const b{{50., 0., 0.}, {1., 0., 0.}, 1., 10., space};//boid lontano, quindi non viene mangiato
        std::vector<boids::Boid> birds{b};
        boids::Flock flock{birds, 100., 10., 1., 1., 1., space};
        boids::Predator const p{{0., 0., 0.}, {1., 0., 0.}, 1., true, space};
        std::vector<boids::Predator> predators{p};
        boids::Wild wild{flock, predators, space};
        wild.eaten();

        CHECK(wild.flock().boids().size() == 1);
        CHECK(wild.predators()[0].hungry() == true);}
    }

    SUBCASE("hunt_speed"){
        boids::Boid const b{{10., 0., 0.}, {1., 0., 0.}, 1., 10., space};
        std::vector<boids::Boid> birds{b};
        boids::Flock flock{birds, 100., 10., 1., 1., 1., space};
        boids::Predator const p{{0., 0., 0.}, {0., 1., 0.}, 3.14, true, space};
        std::vector<boids::Predator> predators{p};
        boids::Wild wild{flock, predators, space};
        wild.hunt_speed(0);

        CHECK(wild.predators()[0].velocity().x == doctest::Approx(67.5));
        CHECK(wild.predators()[0].velocity().y == doctest::Approx(0.));
        CHECK(wild.predators()[0].velocity().z == doctest::Approx(0.));
    }

    SUBCASE("escape"){
        boids::Boid const b{{0., 0., 0.}, {1., 0., 0.}, 3.14, 10., space};
        std::vector<boids::Boid> birds{b};
        boids::Flock flock{birds, 100., 10., 1., 1., 1., space};
        boids::Predator const p{{10., 0., 0.}, {0., 0., 0.}, 3.14, true, space};
        std::vector<boids::Predator> predators{p};
        boids::Wild wild{flock, predators, space};
        wild.escape();

        CHECK(wild.flock().boids()[0].velocity().x == doctest::Approx(-8.));
        CHECK(wild.flock().boids()[0].velocity().y == doctest::Approx(0.));
        CHECK(wild.flock().boids()[0].velocity().z == doctest::Approx(0.));
         
    }

    SUBCASE("refresh_predators"){
        std::vector<boids::Boid> birds{};
        boids::Flock flock{birds, 100., 10., 1., 1., 1., space};
        boids::Predator const p{{0., 0., 0.}, {1., 0., 0.}, 3.14, false, space};
        std::vector<boids::Predator> predators{p};
        boids::Wild wild{flock, predators, space};

        wild.refresh_predators(1.);//testa che funzioni quando ancora è sazio
        CHECK(wild.predators()[0].hungry() == false);
        CHECK(wild.predators()[0].timer() == doctest::Approx(1.));
        CHECK(wild.predators()[0].max_speed() == doctest::Approx(25.));

        CHECK(wild.predators()[0].velocity().x == doctest::Approx(25. / std::sqrt(8101.)));
        CHECK(wild.predators()[0].velocity().y == doctest::Approx(0.));
        CHECK(wild.predators()[0].velocity().z == doctest::Approx(-90. * 25. / std::sqrt(8101.)));

        wild.refresh_predators(4.);
        CHECK(wild.predators()[0].hungry() == true);
        CHECK(wild.predators()[0].timer() == doctest::Approx(0.));
        CHECK(wild.predators()[0].max_speed() == doctest::Approx(75.));
        CHECK(boids::norm(wild.predators()[0].velocity()) <= 75.); //controllo che velocità generata dopo essere tornato affamato non superi vmax

    }

    SUBCASE("ceiling_correction"){
        boids::Boid const b{{0., 0., 475.}, {1., 0., 2.}, 1., 10., space};//boid vicino al soffitto
        std::vector<boids::Boid> birds{b};
        boids::Flock flock{birds, 100., 10., 1., 1., 1., space};

        boids::Predator const p{{0., 0., -475.}, {1., 0., -2.}, 3.14, true, space};//predatore vicino al pavimento (cosi senza due subcase diversi controllo sia soffitto che pavimento)
        std::vector<boids::Predator> predators{p};
        boids::Wild wild{flock, predators, space};

        wild.ceiling_correction();

        CHECK(wild.flock().boids()[0].velocity().z == doctest::Approx(1.));
        CHECK(wild.predators()[0].velocity().z == doctest::Approx(-1.));
    }
}
