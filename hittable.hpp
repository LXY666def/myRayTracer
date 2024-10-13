#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include "ray.hpp"
#include "aabb.hpp"

class material;

class hit_record
{
public:
    point3 hit_point;
    vec3 normal;
    double t;
    bool front_face;
    shared_ptr<material> mat;
    
    void set_normal(const vec3 &ray_dir, const vec3 &outward_normal)
    {
        front_face = (dotProduct(ray_dir, outward_normal) < 0);
        normal = front_face ? outward_normal : -outward_normal;
        normal = normalize(normal);
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_record &record) const = 0;
    virtual aabb bounding_box() const = 0;
};

#endif