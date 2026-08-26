#ifndef VECTOR3_HPP
#define VECTOR3_HPP

namespace boids {
struct Vector3 {
  double x;
  double y;
  double z;
};

Vector3 operator+(Vector3 const& l, Vector3 const& r);
Vector3 operator-(Vector3 const& l, Vector3 const& r);
Vector3 operator*(Vector3 const& v, double scalar);
Vector3 operator/(Vector3 const& v, double scalar);

bool operator==(Vector3 const& a, Vector3 const& b);

double norm(Vector3 const& v);
double distance(Vector3 const& a, Vector3 const& b);
double scalar_product(Vector3 const& a, Vector3 const& b); 
}

#endif