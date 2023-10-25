#ifndef SPHERE_CUH
#define SPHERE_CUH

#include "hittable.cuh"

class sphere: public hittable  {
    public:
        __device__ sphere() {}
        __device__ sphere(vec3 cen, float r, vec3 color, float luminance, float reflect) : center(cen), radius(r), color(color), luminance(luminance), reflect(reflect)  {};
        __device__ virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        vec3 center;
        float radius;
        vec3 color;
        float luminance;
        float reflect;
};

__device__ bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.color = color;
            rec.luminance = luminance;
            rec.reflect = reflect;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.color = color;
            rec.luminance = luminance;
            rec.reflect = reflect;
            return true;
        }
    }
    return false;
}


#endif