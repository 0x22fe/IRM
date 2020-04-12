#include <cassert>

#include <Geometry.hpp>
#include <Utility.hpp>

#include <IntervalTree.hpp>

class IRM
{
public:

    IRM(unsigned int k, const std::vector<segment>& segs) : sz(0), k(k), segments(segs)
    {
        assert(k != 0);
        generate();
    }

    inline size_t count()
    {
        return segments.size();
    }

    inline size_t rawSize()
    {
        return sz;
    }

    size_t querySize(const line& l)
    {
        vec2 p(lineToTransformAngle(l), transformLine(l));
        unsigned int n = (unsigned int)(std::floor((p.x * (float)(k)) / PI));

        return t[n].findOverlappingIntersect(p.y - EPSILON, p.y + EPSILON, l);
    }

    std::vector<Interval<float, segment*>> query(const line& l)
    {
        vec2 p(lineToTransformAngle(l), transformLine(l));
        unsigned int n = (unsigned int)(std::floor((p.x * (float)(k)) / PI));

        std::vector<Interval<float, segment*>> s = t[n].findOverlapping(p.y - EPSILON, p.y + EPSILON);

        vec2 o;
        std::vector<Interval<float, segment*>>::iterator i;

        for (i = s.begin(); i != s.end();)
        {
            if (!l.intersect(*((*i).value), o))
                i = s.erase(i);
            else
                ++i;
        }

        return s;
    }
    
    size_t insert(const std::vector<segment>& segs)
    {
        size_t startlen = segments.size();
        segments.insert(segments.end(), segs.begin(), segs.end());
        t.clear();
        generate();
        return startlen;
    }
    
    void remove(size_t from)
    {
        segments.erase(segments.begin() + from);
        t.clear();
        generate();
    }

    static float lineToTransformAngle(const line& l)
    {
        float a = l.toAngle();
        if(0.0F <= a && a <= PI)
            return PI - a;
        else
            return std::fabs(a);
    }

    static float transformLine(const line& l)
    {
        return rotate(l.closest(), lineToTransformAngle(l)).x;
    }

private:

    void generate()
    {
        const float c = PI / ((float)(k));
        const size_t len = segments.size();

        for(size_t i = 0; i < k; i++)
        {
            float bmin = ((float)(i)) * c, bmax = ((float)(i + 1)) * c;
            std::vector<Interval<float, segment*>> tmp;

            for(size_t u = 0; u < len; u++)
            {
                vec2 xmin = xBound(bmin, segments[u]), xmax = xBound(bmax, segments[u]);
                vec2 bounds = vec2(std::min(xmin.x, xmax.x), std::max(xmin.y, xmax.y));

                tmp.push_back(Interval<float, segment*>(bounds.x, bounds.y, &segments[u]));
            }

            t.push_back(IntervalTree<float, segment*>(std::move(tmp)));
        }

        sz += sizeof(Interval<float, segment*>) * k * segments.size();
    }

    static vec2 xBound(float theta, const segment& s)
    {
        segment e = segment(rotate(s.a, theta), rotate(s.b, theta));
        return vec2(std::min(e.a.x, e.b.x), std::max(e.a.x, e.b.x));
    }

    size_t sz;
    const unsigned int k;
    std::vector<segment> segments;
    std::vector<IntervalTree<float, segment*>> t;
};
