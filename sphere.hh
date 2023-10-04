#ifndef SPHERE_HH
#define SPHERE_HH

#include "hittable.hh"
#include "vec3.hh"

class sphere : public hittable {
    public:
        sphere(point3 _center, double _radius) : center(_center), radius(_radius) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = r.origin() - center; // vector from center of sphere to ray origin
            
            // t^2*v⋅v  +  2tv⋅(A−C)  +  (A−C)⋅(A−C)−r2 = 0
            // a = v⋅v   b = 2v⋅(A-C)    c = (A-C)⋅(A-C)-r2
            // 2 is factored out of b to simplify calculation:
            auto a = r.direction().length_squared(); // dot product of ray direction with itself
            auto half_b = dot(oc, r.direction()); // dot product of vector from center to ray origin with ray direction
            auto c = oc.length_squared() - radius * radius; // same as dot product only faster
            auto discriminant = half_b*half_b - a*c; // discriminant of quadratic equation (upper part of abc formula)

            if(discriminant < 0) // if discriminant is negative, ray does not hit sphere (euqation has no real solutions)
                return false;
            auto sqrtd = sqrt(discriminant);

            // find nearest root that lies in acceptable range
            auto root = (-half_b - sqrtd) / a;
            if(!ray_t.surrounds(root)) {
                root = (-half_b + sqrtd) / a;
                if(!ray_t.surrounds(root))
                    return false;
            }

            // update the hit record of this object
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);

            return true;
        }

    private:
        point3 center;
        double radius;

};



#endif