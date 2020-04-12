#include <iostream>
#include <cmath>

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#define SQUARE(V) ((V) * (V))

static const float INF = 1e+8F;
static const float EPSILON = 1e-5F;
static const float PI = 3.14159265358979323846F;

static inline bool eqf(float a, float b, float tolerance = EPSILON)
{
    return std::fabs(a - b) <= tolerance;
}

struct vec2
{
    vec2() : x(0.0F), y(0.0F) {}
    vec2(float x, float y) : x(x), y(y) {}

    float x, y;

    void operator=(const vec2& v)
    {
        x = v.x;
        y = v.y;
    }
};

std::ostream& operator<<(std::ostream& o, const vec2& v)
{
    o << '(' << v.x << ", " << v.y << ')';
    return o;
}

struct segment
{
    vec2 a, b;

    segment(vec2 a, vec2 b) : a(a), b(b) {}

    void operator=(const segment& v)
    {
        a = v.a;
        b = v.b;
    }
};

std::ostream& operator<<(std::ostream& o, const segment& v)
{
    o << v.a << " -> " << v.b;
    return o;
}

struct line
{
    float slope, offset;

    line(float slope, float offset) : slope(slope), offset(offset) {}

    bool intersect(const segment& s, vec2& point) const
    {
        float m = eqf(s.a.x, s.b.x) ? (s.a.y > s.b.y ? INF : -INF) : (s.a.y - s.b.y) / (s.a.x - s.b.x);

        if(!eqf(slope, m))
        {
            float b = s.a.y - (m * s.a.x);
            float x = (offset - b) / (m - slope);

            if(std::min(s.a.x, s.b.x) <= x && x <= std::max(s.a.x, s.b.x))
            {
                point = vec2(x, slope * x + offset);
                return true;
            }
        }

        return false;
    }

    vec2 closest() const
    {
        float x = (-offset * slope) / (SQUARE(slope) + 1);
        return vec2(x, slope * x + offset);
    }

    float toAngle() const
    {
        vec2 c = closest();
        float s = std::atan2(c.y, c.x);

        return s;
    }

};

std::ostream& operator<<(std::ostream& o, const line& v)
{
    o << "y = " << v.slope << "x" << (v.offset < 0.0F ? " - " : " + ") << (v.offset < 0.0F ? -v.offset :  + v.offset);
    return o;
}

// https://stackoverflow.com/a/2259502
static vec2 rotate(const vec2& p, float angle)
{
    float s = std::sin(angle);
    float c = std::cos(angle);
    return vec2(p.x * c - p.y * s, p.x * s + p.y * c);
}

#endif // GEOMETRY_HPP
