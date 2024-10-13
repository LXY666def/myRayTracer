#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <cmath>
#include <limits>
#include <memory>

// usings
using std::make_shared;
using std::shared_ptr;

// constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// utility functions
inline double deg2rad(double degree) { return degree / 180.0 * pi; }
inline double rad2deg(double radian) { return radian / pi * 180.0; }


// common headers
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"

// macro
#define ANTIALIASING_TRUE true
#define ANTIALIASING_FALSE false

#endif