#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include"global.hpp"
#include"color.hpp"
#include"hittable.hpp"

#include<string>

using std::string;

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(const ray &r_in, const hit_record &record ,color &attenuation, ray &scattered) const = 0;

    string pname;
};

class lambertian : public material {
public:
    lambertian(const color &a, string name = "nan") : albedo(a) { pname = name; }

    bool scatter(const ray &r_in, const hit_record &record ,color &attenuation, ray &scattered)const override{
        auto scatter_dir = record.normal + random_in_sphere();
        
        if(scatter_dir.near_zero())
            scatter_dir = record.normal;
        
        scattered = ray(record.hit_point, scatter_dir, r_in.get_time());
        attenuation = albedo;
        return true;
    }
private:
    color albedo;
};

class metal : public material{
public:
    metal(const color& a, double r,string name = "nan"): albedo(a),roughness(r){pname = name;}

    bool scatter(const ray &r_in, const hit_record &record ,color &attenuation, ray &scattered)const override{
        vec3 reflected = reflect(r_in.get_direction(), record.normal);
        scattered = ray(record.hit_point, reflected+roughness*random_in_sphere(),r_in.get_time());
        attenuation = albedo;
        return (dotProduct(scattered.get_direction(), record.normal)>0);
    }
private:
    color albedo;
    double roughness;
};

class dielectric : public material{
public:
    dielectric(double i,string name = "nan"):index(i){pname = name;}

    bool scatter(const ray& r_in, const hit_record& record, color& attenuation, ray& scattered)const override{
        attenuation = color(1.0,1.0,1.0);
        double relative_n = (record.front_face ? 1.0 / index : index);

        double cos = fmin(dotProduct(-r_in.get_direction(), record.normal), 1.0);
        double sin = sqrt(1.0 - cos * cos);

        bool total_internal_reflection = relative_n * sin > 1.0;
        vec3 dir;
        if(total_internal_reflection || schlick_approx_reflect(cos,relative_n)>random_double())
            dir = reflect(r_in.get_direction(), record.normal);
        else
            dir = refract(r_in.get_direction(), record.normal, relative_n);

        scattered = ray(record.hit_point, dir, r_in.get_time());
        return true;
    }
private:
    double index;

    static double schlick_approx_reflect(double cos, double index){
        auto r0 = (1 - index) / (1 + index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cos), 5);
    }
};

#endif 