 #ifndef RAY_HPP
#define RAY_HPP

#include"vec3.hpp"

class ray{
public:
    ray(){}
    ray(const point3 &origin, const vec3 &direction, double time = 0.0):orig(origin), dir(direction),time(time){
        dir = normalize(dir);
    }

    point3 get_origin() const {return orig;}
    vec3 get_direction() const {return dir;}
    double get_time() const { return time; }

    point3 cast(double t) const {return orig + t*dir;}
private:
    point3 orig;
    vec3 dir;
    double time;
};

#endif