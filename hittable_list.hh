#ifndef HITTAVBLE_LIST_HH
#define HITTAVBLE_LIST_HH

#include "hittable.hh"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr; // makes sure an object that is referenced multiple times gets safely deleted

class hittable_list : public hittable
{
public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); } // create list with an initial object

    // clear the list
    void clear()
    {
        objects.clear();
    }

    // add an hittable object to the list of objects
    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
    }

    // determine wheter ray hits and object from the hittable list and if so which one is the first it hits (since it then bounces off that)
    bool hit(const ray &r, double ray_tmin, double ray_tmax, hit_record &rec) const override
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto clostest_so_far = ray_tmax;

        for (const auto &object : objects)
        { // for each object use the hit function of that object to see wheter its hit or not
            if (object->hit(r, ray_tmin, clostest_so_far, temp_rec))
            {
                hit_anything = true;
                clostest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }
};

#endif