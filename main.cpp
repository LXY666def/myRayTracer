#include "color.hpp"
#include "global.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "vec3.hpp"
#include "material.hpp"
#include "bvh.hpp"

int main()
{
    // scene
    hittable_list scene;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    scene.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).norm() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    scene.add(make_shared<sphere>(center,center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    scene.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    scene.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    scene.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    scene.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    scene.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    //scene = hittable_list(make_shared<bvh_node>(scene));

    camera eye(16.0/9.0, 400, ANTIALIASING_TRUE);

    eye.samples_per_pixel = 100;
    eye.max_depth         = 50;

    eye.fov     = 20;
    eye.eye_pos = point3(13,2,3);
    eye.lookat   = point3(0,0,0);
    eye.uphead      = vec3(0,1,0);

    eye.defocus_angle = 0.02;
    eye.focus_distance    = 10.0;
    eye.initialize();

    //eye.print_createInfo();
    //eye.debug(scene);

    eye.render(scene);
}
