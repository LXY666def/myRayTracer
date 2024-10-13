#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hittable.hpp"
#include "vec3.hpp"
#include "material.hpp"
class sphere : public hittable
{
public:
    sphere(point3 center, double radius, shared_ptr<material> material)
        : center1(center), radius(radius), mat(material),is_moving(false)
    {
        auto r_dir = vec3(radius, radius, radius);
        box = aabb(center1 - r_dir, center1 + r_dir);
    }
    sphere(point3 c1, point3 c2, double r, shared_ptr<material> mat)
        :center1(c1), radius(r), mat(mat), is_moving(true)
    {
        auto r_dir = vec3(radius, radius, radius);
        auto box1 = aabb(center1 - r_dir, center1 + r_dir);
        auto box2 = aabb(center1 - r_dir, center1 + r_dir);
        box = aabb(box1, box2);

        moving_dir = c2 - c1;
    }

    aabb bounding_box() const override { return box;}
    bool hit(const ray &r, interval ray_t, hit_record &record) const
    {
        point3 center = is_moving ? get_center(r.get_time()) : center1;
        vec3 oc = r.get_origin() - center;
        auto a = r.get_direction().norm2();
        auto half_b = dotProduct(oc, r.get_direction());
        auto c = oc.norm2() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.is_surround(root))
        {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.is_surround(root))
                return false;
        }

        record.t = root;
        record.hit_point = r.cast(root);
        vec3 outward_normal = (record.hit_point - center)/radius;
        record.set_normal(r.get_direction(), outward_normal);
        record.mat = mat;
        return true;
    }

private:
    point3 center1;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;
    vec3 moving_dir;
    aabb box;

    point3 get_center(double time) const {
        //linearly moving
        return center1 + time * moving_dir;
    }
};

#endif