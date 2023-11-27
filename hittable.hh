#ifndef HITTABLE_HH
#define HITTABLE_HH

#include "rtweekend.hh"

class material;

/**
 * @brief The hit_record class stores information about a ray-object intersection.
 */
class hit_record
{
public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face; // Indicates if the ray hit the front face of the object.

    /**
     * @brief Sets the face normal and front_face flag based on the ray direction and outward normal.
     *
     * @param r The incident ray.
     * @param outward_normal The outward normal of the intersected object.
     */
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

/**
 * @brief The hittable class represents an abstract object that can be intersected by a ray.
 */
class hittable
{
public:
    virtual ~hittable() = default;

    /**
     * @brief Determines if a ray intersects with the object and records the intersection details in the hit_record.
     *
     * @param r The incident ray.
     * @param ray_t The interval of valid parameter values for the ray.
     * @param rec The hit_record to store the intersection details.
     * @return true if the ray intersects with the object, false otherwise.
     */
    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0; // = 0 ~> every child class needs to implement this
};

#endif