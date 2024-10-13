#ifndef COLOR_HPP
#define COLOR_HPP

#include "vec3.hpp"
#include "interval.hpp"

#include <iostream>
#include <fstream>

using color = vec3;

inline double linear2gamma(double linear_component){
    return sqrt(linear_component);
}

void write_color(std::ofstream &out, const color &pixel_color, int samples_per_pixel){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // linear space to gamma space
    r = linear2gamma(r);
    g = linear2gamma(g);
    b = linear2gamma(b);

    // Write the translated [0,255] value of each color component.
    static const interval intensity(0.0, 0.999);
    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}

#endif