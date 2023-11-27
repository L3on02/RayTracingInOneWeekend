#ifndef INTERVAL_HH
#define INTERVAL_HH

#include "rtweekend.hh"

/**
 * @brief The interval class represents a closed interval of real numbers.
 *        It is used to represent the valid parameter values for a ray and also contains some utility functions.
 */
class interval
{
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // default intervall is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    double clamp(double x) const
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    static const interval empty, universe;
};

const static interval empty(+infinity, -infinity);
const static interval universe(-infinity, +infinity);

#endif