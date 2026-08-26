cmake_minimum_required(VERSION 3.28)

project(boids VERSION 0.1.0 LANGUAGES CXX)

# Cerca SFML e i moduli necessari per la grafica.
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Abilita i test e rende disponibile BUILD_TESTING
include(CTest)

# Utile per VS Code e clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Usa C++20 senza estensioni specifiche del compilatore
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warning richiesti dalla consegna
string(APPEND CMAKE_CXX_FLAGS
       " -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion"
       " -Wshadow -Wimplicit-fallthrough -Wextra-semi -Wold-style-cast"
       " -fno-omit-frame-pointer")

# Asserzioni della standard library
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  string(APPEND CMAKE_CXX_FLAGS
         " -D_GLIBCXX_ASSERTIONS")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  string(APPEND CMAKE_CXX_FLAGS
         " -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE")
endif()

# Sanitizer in modalità Debug
string(APPEND CMAKE_CXX_FLAGS_DEBUG
       " -fsanitize=address,undefined")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  string(APPEND CMAKE_CXX_FLAGS
         " -D_GLIBCXX_SANITIZE_STD_ALLOCATOR")
endif()

string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG
       " -fsanitize=address,undefined")

# Programma principale
add_executable(boids
    main.cpp
    vector3.cpp
    boid.cpp
    flock.cpp
    ToroidalSpace.cpp
    Predator.cpp
    wild.cpp
    render.cpp
)

target_link_libraries(boids PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# Unit test
if (BUILD_TESTING)

  add_executable(boids.test
      boids.test.cpp
      vector3.cpp
      boid.cpp
      flock.cpp
      ToroidalSpace.cpp
      Predator.cpp
      wild.cpp
  )

  add_test(
      NAME boids.test
      COMMAND boids.test
  )

endif()
