#ifndef HITTABLE_LIST_HPP
#define HITTABLE_LIST_HPP

#include"aabb.hpp"
#include "hittable.hpp"
#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable
{
private:
    aabb box;

public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
        box = aabb(box, object->bounding_box());
    }

    bool hit(const ray &r, interval ray_t, hit_record &record) const override
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                record = temp_rec;
            }
        }

        return hit_anything;
    }
    aabb bounding_box() const override { return box;}
};

#endif