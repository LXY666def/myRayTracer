#ifndef CAMERA_HPP
#define CAMERA_HPP

#include"vec3.hpp"
#include"ray.hpp"
#include "color.hpp"
#include "global.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include<fstream>

void print_progress(float t){
    int tt = static_cast<int>(40 * t);
    std::clog << "\rProgress: ";
    for (int i = 1; i <= 40;i++){
        if(i < tt)
            std::clog << '#';
        else
            std::clog << ' ';
    }
    std::clog << ' '<<static_cast<int>(100*t)<<'%'<<std::flush;
}

class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    bool antialiasing;
    int max_depth = 10;
    double fov = 90;    //degree
    point3 eye_pos = point3(0, 0, -1);
    point3 lookat = point3(0, 0, -1);
    vec3 uphead = vec3(0, 1, 0);    //relative, not 100% ortho to lookat direction

    double defocus_angle = 0;
    double focus_distance = 10;

    camera(){}
    camera(double ratio, double width, bool ret = true)
        :aspect_ratio(ratio),image_width(width),antialiasing(ret){}

    void render(const hittable& scene){
        std::ofstream output;
        output.open("./output.ppm", std::ios::out);
        output << "P3\n"<< image_width << ' ' << image_height << "\n255\n";
        for (int i = 0; i < image_height; i++){
            print_progress(static_cast<double>(i+1) / image_height);
            for (int j = 0; j < image_width; j++){
                color pixel_color(0, 0, 0);
                for (int sample = 1; sample <= samples_per_pixel; sample++){
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, scene);
                }
                write_color(output, pixel_color, samples_per_pixel);
            }
        }
        output.close();
    }
    void initialize(){
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1 ? 1 : image_height);

        w = normalize(-(lookat - eye_pos));  //lookat direction = -w
        u = normalize(crossProduct(uphead, w));
        v = crossProduct(w, u); //up direction, ortho to lookat direction

        auto viewport_height = 2*focus_distance*tan(deg2rad(fov/2));
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        auto viewport_u = viewport_width * u;
        auto viewport_v = -viewport_height * v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_left_upper = eye_pos + focus_distance * (-w) - viewport_u / 2 - viewport_v / 2;
        pixel_origin = viewport_left_upper + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_distance * tan(deg2rad(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

        if(!antialiasing)
            samples_per_pixel = 1;
    }
    void print_createInfo(){
        std::cout << "w: " << image_width << ' ' << "h: " << image_height << '\n'
                << "eye_pos: " << eye_pos << '\n'
                << "pixel_origin: " << pixel_origin << '\n'
                << "pixel_delta_u: " << pixel_delta_u << '\n'
                << "pixel_delta_v: " << pixel_delta_v << std::endl;
    }
    void debug(const hittable& scene){
        std::cout << "===========================debug 启动！===========================" << '\n';
        antialiasing = ANTIALIASING_FALSE;
        
        for (int i = 0; i < image_height; i++){
            for (int j = 0; j < image_width; j++){
                auto pixel_current = pixel_origin + (j * pixel_delta_u) + (i * pixel_delta_v);
                auto ray_dir = pixel_current - eye_pos;
                ray r(eye_pos, ray_dir);

                if(i==image_height/2 && j==image_width/2){
                    std::cout << "eye_pos: " << eye_pos << '\n'
                              << "ray_dir: " << ray_dir << "\n\n";

                    auto pixel_color = ray_color(r,2,scene,true);
                    std::cout << "pixel_color: " << pixel_color << '\n';
                    continue;
                }else{
                    continue;
                }
                auto pixel_color = ray_color(r,1,scene);
            }
        }
    }

private:
    int image_height;
    point3 pixel_origin;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u;
    vec3 v;
    vec3 w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    color ray_color(const ray &r, int depth, const hittable &scene, bool debug = false){
        hit_record record;
        if(depth <= 0)
            return color(0, 0, 0);
        
        if (scene.hit(r, interval(0.001, infinity), record)){
            ray scattered;
            color attenuation;
            if(record.mat->scatter(r,record,attenuation,scattered)){
                if(debug){
                    std::cout << "depth: " << depth << "    mat: " << record.mat->pname<<'\n';
                }
                return attenuation * ray_color(scattered,depth-1,scene,debug);
            }
            return color(0, 0, 0);
        }

        auto normalized_dir = normalize(r.get_direction());
        auto a = 0.5 * (normalized_dir.y() + 1.0);
        auto skylight = (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        if(debug){
            std::cout << "depth: 1    skylight: " << skylight.x() << ' ' << skylight.y() << ' ' << skylight.z() << '\n';
        }
        return skylight;
    }
    ray get_ray(const int i, const int j) const{
        auto pixel_current = pixel_origin + (j * pixel_delta_u) + (i * pixel_delta_v);
        auto pixel_sample = pixel_current;
        if(antialiasing)
            pixel_sample += sample_offset();

        auto ray_origin = (defocus_angle <= 0) ? eye_pos : defocus_disk_sample();
        auto ray_dir = pixel_sample - ray_origin;
        auto ray_time = random_double();
        
        return ray(ray_origin, ray_dir, ray_time);
    }
    vec3 sample_offset() const{
        auto offset_x = -0.5 + random_double();
        auto offset_y = -0.5 + random_double();
        return offset_x * pixel_delta_u + offset_y * pixel_delta_v;
    }
    point3 defocus_disk_sample() const{
        auto p = random_in_disk();
        return eye_pos + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};


#endif