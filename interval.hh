#ifndef INTERVAL_HH
#define INTERVAL_HH

#include "rtweekend.hh"

class interval {
    public:
        double min, max;

        interval() : min(+infinity), max(-infinity) {} // default intervall is empty

        interval(double _min, double _max) : min(_min), max(_max) {}
        
        bool contains(double x) const {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const {
            return min < x && x < max;
        }

        static const interval empty, universe;
};

const static interval empty   (+infinity, -infinity);
const static interval universe(-infinity, +infinity);

#endif