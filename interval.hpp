#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include "global.hpp"

class interval
{
public:
    double min;
    double max;

    interval() : min(+infinity), max(-infinity) {}
    interval(double min, double max) : min(min), max(max) {}
    interval(const interval& a, const interval& b)
      : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    bool is_contain(double x) const { return min <= x && x <= max; }
    bool is_surround(double x) const { return min < x && x < max; }
    double clamp(double x)const{
        if(x<min)   return min;
        if(x>max)   return max;
        return x;
    }
    double size() const{
        return max - min;
    }
    interval expand(double delta) const{
        auto t = delta / 2;
        return interval(min - t, max + t);
    }
    static const interval empty;
    static const interval universe;
};

const interval empty(+infinity, -infinity);
const interval universe(-infinity, +infinity);

#endif