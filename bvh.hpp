#ifndef BVH_HPP
#define BVH_HPP

#include"global.hpp"
#include"hittable.hpp"
#include"hittable_list.hpp"

#include<algorithm>

class bvh_node: public hittable{
public:
    bvh_node(const hittable_list &scene):bvh_node(scene.objects,0,scene.objects.size()){}
    bvh_node(const std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end){
        int axis = static_cast<int>(random_double(0, 3));

        auto comparator = (axis==0 ? box_x_compare:((axis==1) ? box_y_compare:box_z_compare));

        size_t span = end - start;

        if(span == 1)
            left = right = objects[start];
        else if(span == 2){
            if(comparator(objects[start], objects[start+1])){
                left = objects[start];
                right = objects[start + 1];
            }else{
                right = objects[start];
                left = objects[start + 1];
            }
        }else{
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + span / 2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }

        box = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& record)const override{
        if(!box.hit(r, ray_t))
            return false;
        bool hit_left = left->hit(r, ray_t, record);
        bool hit_right = right->hit(r, interval(ray_t.min,hit_left?record.t:ray_t.max), record);

        return hit_left || hit_right;
    }
    aabb bounding_box() const override { return box; }
private: 
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb box;

    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int index){
        return a->bounding_box().axis(index).min < b->bounding_box().axis(index).min;
    }
    static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b){
        return box_compare(a, b, 0);
    }
    static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b){
        return box_compare(a, b, 1);
    }
    static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b){
        return box_compare(a, b, 2);
    }
};

#endif