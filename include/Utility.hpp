#include <vector>
#include <chrono>
#include <random>
#include <ctime>
#include <cstdlib>

#include <Geometry.hpp>

#ifndef UTILITY_HPP
#define UTILITY_HPP

static inline float rand(float min, float max)
{
    return min + ((float)(std::rand()) / (float)(RAND_MAX)) * (max - min);
}

static inline unsigned long int now()
{
    return (unsigned long int)(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

static std::vector<segment> randomSegments(unsigned int num, float bound, float length = 10.0F)
{
    std::mt19937 gen(std::time(NULL));
    std::uniform_real_distribution<float> rd(-bound, bound);

    std::mt19937 gen_length(std::time(NULL));
    std::uniform_real_distribution<float> rd_length(EPSILON, length);

    std::vector<segment> res;
    for(unsigned int i = 0U; i < num; i++)
    {
        vec2 aa = vec2(rd(gen), rd(gen));
        vec2 bb = vec2(aa.x + rd_length(gen_length), aa.y + rd_length(gen_length));
        res.push_back(segment(aa, bb));
    }
    return res;
}

static std::vector<line> randomLines(unsigned int num, float bound)
{
    std::mt19937 gen(std::time(NULL));
    std::uniform_real_distribution<float> rd(-PI / 2.0F, PI / 2.0F);

    std::mt19937 gen_bound(std::time(NULL));
    std::uniform_real_distribution<float> rd_bound(-bound, bound);

    std::vector<line> res;
    for(unsigned int i = 0U; i < num; i++)
        res.push_back(line(std::tan(rd(gen)), rd_bound(gen_bound)));
    return res;
}

#endif // UTILITY_HPP
