#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include "rtweekend.hh"

#include "hittable.hh"

class triangle : public hittable {
  public:
    triangle(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> m) : Q(_Q), u(_u), v(_v), mat(m) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n , n);
    }


    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto denom = dot(normal, r.direction());
        if(fabs(denom) < 1e-8) // ray is parallel to plane
            return false;
        
        auto t = (D - dot(normal, r.origin())) / denom;
        if(!ray_t.contains(t))
            return false;

        auto intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if(!is_interior(alpha, beta, rec));
        
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record &rec) const {
        if(a + b < 0 || a + b > 1)
            return false;
        
        rec.u = a;
        rec.v = b;
        return true;
    }

  private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    vec3 normal;
    double D;
    vec3 w;
};

#endif