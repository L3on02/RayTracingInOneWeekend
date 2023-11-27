#ifndef MATERIAL_HH
#define MATERIAL_HH

#include "rtweekend.hh"
#include "hittable.hh"

using color = vec3;

/**
 * @brief The material class represents an abstract material that is used to determine how rays interact with the surface of an object.
 */
class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
};

/**
 * @brief the lambertian class represents a diffuse material.
 *
 * It scatters rays in random directions in the hemisphere around the normal of the surface.
 */
class lambertian : public material
{
public:
    lambertian(const color &a) : albedo(a) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

/**
 * @brief the lambertian class represents a reflective material.
 *
 * It calculates the reflection of the incident ray and scatters the ray in that direction.
 * The fuzz parameter controls the amount of randomness in the reflection. A fuzz value of 0 results in a perfect reflection.
 * The albedo parameter controls the color-attenutaion of the reflection.
 */
class metal : public material
{
public:
    metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_unit_vector());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz;
};

/**
 * @brief the lambertian class represents a transparent material.
 *
 * It calculates the refraction of the incident ray and scatters the ray in that direction.
 * The index of refraction parameter controls the amount of refraction and is derived from the density of the material.
 */
class dielectric : public material
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
        return true;
    }

private:
    double ir; // Index of Refraction

    // Schlick's polynomial relfectance approximation
    static double reflectance(double cos, double ref_idx)
    {
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 *= r0;
        return (r0 + (1 - r0) * pow((1 - cos), 5));
    }
};
#endif