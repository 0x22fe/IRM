#include <fstream>

#include "IRM.hpp"

// #define TEST_VERIFY
#define TEST_K
#define TEST_GENERAL
#define TEST_L
// #define TEST_DEBUG

#define BIL(v) (((double)(v)) / 1e9)
#define MIL(v) (((double)(v)) / 1e6)
#define RATE(n, d) (((double)(n)) / ((double)(d)))
#define RATEB(n, d) (((double)(n)) / BIL(d))

#ifdef TEST_VERIFY

static bool test()
{
    line l = randomLines(1, 100.0F)[0];
    l.slope = 1.5F;
    l.offset = 3.0F;

    segment s = randomSegments(1, 100.0F)[0];
    s.a = vec2(5.0F, 10.0F);
    s.b = vec2(10.0F, 10.0F);

    vec2 p;

    if(l.intersect(s, p))
        std::cout << p << std::endl;
    else
        std::cout << "No intersection" << std::endl;

    std::cout << l << std::endl;
    std::cout << s << std::endl;

    std::cout << l.closest() << std::endl;
    std::cout << l.toAngle() << std::endl;

    std::cout << IRM::lineToTransformAngle(l) << std::endl;
    std::cout << IRM::transformLine(l) << std::endl;

    return true;
}

static bool benchtree()
{
    std::vector<Interval<float, int>> intervals;

    for(auto i = 0; i < 1000000; i++)
    {
        auto v = vec2(1.0F, 2.0F + ((float)(i) / 100000.0F));
        intervals.push_back(Interval<float, int>(v.x, v.y, i));
    }

    IntervalTree<float, int> tree(std::move(intervals));

    float qp = 4.3F;

    auto start = now();
    auto sz = tree.findOverlapping(qp - EPSILON, qp + EPSILON).size();
    auto end = now();

    std::cout << "Standard Interval test took: " << (end - start) / 1000 << "us for " << sz << " results" << std::endl;

    return true;
}

#endif

int main()
{
    std::srand(std::time(NULL));

    std::cout << "IRM (Interval Rotation Map) Benchmark" << std::endl;
    std::cout << "Program is licensed under the MIT License. Copyright (c) 2020 Rohan A." << std::endl;
    std::cout << "This will take some time and resources - monitor your CPU and RAM usage please.\n" << std::endl;

    std::cout.precision(5);
    std::cout << std::scientific;

#ifdef TEST_VERIFY
    if(!test() || !benchtree())
        return 0;
#endif

    const auto TESTS = 10;
    const auto BOUNDS = 500.0F;

    const auto K = 100;
    const auto NUM_LINES = 10;
    const auto NUM_SEGS = 1000;

#if defined(TEST_K) || defined(TEST_GENERAL)
    const auto LENGTH = 10.0F;
#endif

#ifdef TEST_K
    const auto MINK = 1;
    const auto FACTORK = 5;
    const auto MAXK = 200 + FACTORK;
#endif

#ifdef TEST_GENERAL
    const auto MINLVL = 10000;
    const auto FACTOR = 50000;
    const auto MAXLVL = 1000000 + FACTOR;
#endif

#ifdef TEST_L
    const auto MINL = EPSILON;
    const auto FACTORL = 0.5F;
    const auto MAXL = 50.0F + FACTORL;
#endif

    vec2 trick = vec2(0.0F, 0.0F);

    // Data files

    std::ofstream kt, typical, accelerated, ltt, lta;

#ifdef TEST_K

    kt.open("k.csv");
    kt << "k,numsegs,numlines,intersections,createtime,space,querytime,inserttime,deletetime,queryrate,insertrate,deleterate" << std::endl;

    std::cout << "Running k-value tests.\nRemaining: " << std::flush;

    /* k Tests */
    for(auto k = MINK; k < MAXK; k += FACTORK)
    {
        std::cout << (((MAXK - k) / FACTORK) + 1) << "... " << std::flush;

        size_t avgconstruct = 0;
        size_t avgspace = 0;
        size_t avgquery = 0;
        size_t avgint = 0;
        size_t avginsert = 0;
        size_t avgdelete = 0;

        for(auto i = 0; i < TESTS; i++)
        {
            // Generate lines and geometry
            auto lines = randomLines(NUM_LINES, BOUNDS);
            auto segments = randomSegments(NUM_SEGS, BOUNDS, LENGTH);
            auto inserts = randomSegments(NUM_SEGS, BOUNDS, LENGTH);

#ifdef TEST_DEBUG
            std::cout << "\nGENERATED\n" << std::endl;
#endif

            // Construction
            auto acstart = now();
            IRM irm(k, segments);
            auto acend = now();

#ifdef TEST_DEBUG
            std::cout << "\nCONSTRUCTED\n" << std::endl;
#endif

            // Space
            auto sz = irm.rawSize();

            // Query
            auto astart = now();
            size_t ct = 0;
            for(auto& l : lines)
            {
                // Don't store result to make benchmark matched
                ct += irm.querySize(l);
            }
            auto aend = now();
            avgint = ct / NUM_LINES;

#ifdef TEST_DEBUG
            std::cout << "\nQUERIED\n" << std::endl;
#endif

            // Insert
            auto aistart = now();
            irm.insert(inserts);
            auto aiend = now();

            // Delete
            auto adstart = now();
            irm.remove(0);
            auto adend = now();

            avgconstruct += acend - acstart;
            avgspace += sz;
            avgquery += aend - astart;
            avginsert += aiend - aistart;
            avgdelete += adend - adstart;
        }

        avgconstruct /= TESTS;
        avgspace /= TESTS;
        avgquery /= TESTS;
        avginsert /= TESTS;
        avgdelete /= TESTS;

        kt << k << ',' << NUM_SEGS << ',' << NUM_LINES << ',' << RATE(avgint, NUM_LINES) << ',' << BIL(avgconstruct) << ',' << MIL(avgspace) << ',' <<
              BIL(avgquery) << ',' << BIL(avginsert) << ',' << BIL(avgdelete) << ',' << RATEB(NUM_LINES, avgquery) << ',' << RATEB(NUM_SEGS, avginsert) << ',' << RATEB(NUM_SEGS, avgdelete) << std::endl;
    }

    kt.close();

    std::cout << "Done\nCompleted k-value tests.\n" << std::endl;
#endif

#ifdef TEST_GENERAL

    typical.open("typical.csv");
    typical << "trial,numsegs,numlines,intersections,createtime,space,querytime,inserttime,deletetime,queryrate,insertrate,deleterate" << std::endl;

    accelerated.open("accelerated.csv");
    accelerated << "trial,k,numsegs,numlines,intersections,createtime,space,querytime,inserttime,deletetime,queryrate,insertrate,deleterate" << std::endl;

    std::cout << "Running general tests.\nRemaining: " << std::flush;

    /* General Tests */
    for(auto u = MINLVL; u < MAXLVL; u += FACTOR)
    {
        std::cout << (((MAXLVL - u) / FACTOR) + 1) << "... " << std::flush;

        size_t tavgconstruct = 0;
        size_t tavgspace = 0;
        size_t tavgquery = 0;
        size_t tavgint = 0;
        size_t tavginsert = 0;
        size_t tavgdelete = 0;

        size_t aavgconstruct = 0;
        size_t aavgspace = 0;
        size_t aavgquery = 0;
        size_t aavgint = 0;
        size_t aavginsert = 0;
        size_t aavgdelete = 0;

        for(auto i = 0; i < TESTS; i++)
        {
            // Generate lines and geometry
            auto lines = randomLines(NUM_LINES, BOUNDS);
            auto segments = randomSegments(u, BOUNDS, LENGTH);
            auto inserts = randomSegments(u, BOUNDS, LENGTH);

            // Typical test

            // No construction
            // No space

            // Query
            auto tstart = now();
            vec2 tmp = vec2();
            for(auto& l : lines)
            {
                for(auto& s : segments)
                {
                    if(l.intersect(s, tmp))
                    {
                        tavgint++;
                    }
                }
            }
            auto tend = now();

            // Insert
            auto tistart = now();
            std::vector<segment> tt;
            tt.insert(tt.end(), inserts.begin(), inserts.end());
            auto tiend = now();

            // Delete
            auto tdstart = now();
            tt.erase(tt.begin() + u, tt.end());
            auto tdend = now();

            tavgconstruct += 0;
            tavgspace += 0;
            tavgquery += tend - tstart;
            tavginsert += tiend - tistart;
            tavgdelete += tdend - tdstart;

            // Accelerated test

#ifdef TEST_DEBUG
            std::cout << "\nTYPICAL\n" << std::endl;
#endif
            // Construction
            auto acstart = now();
            IRM irm(K, segments);
            auto acend = now();

            // Space
            auto sz = irm.rawSize();

            // Query
            auto astart = now();
            for(auto& l : lines)
            {
                // Don't store result to make benchmark matched
                aavgint += irm.querySize(l);
            }
            auto aend = now();

            // Insert
            auto aistart = now();
            irm.insert(inserts);
            auto aiend = now();

            // Delete
            auto adstart = now();
            irm.remove(0);
            auto adend = now();

            aavgconstruct += acend - acstart;
            aavgspace += sz;
            aavgquery += aend - astart;
            aavginsert += aiend - aistart;
            aavgdelete += adend - adstart;

#ifdef TEST_DEBUG
            std::cout << "\nACCELERATED\n" << std::endl;
#endif
        }

        tavgint /= TESTS;
        aavgint /= TESTS;

        tavgconstruct /= TESTS;
        tavgspace /= TESTS;
        tavgquery /= TESTS;
        tavginsert /= TESTS;
        tavgdelete /= TESTS;

        aavgconstruct /= TESTS;
        aavgspace /= TESTS;
        aavgquery /= TESTS;
        aavginsert /= TESTS;
        aavgdelete /= TESTS;

        typical << ((u - MINLVL) / FACTOR) + 1 << ',' << u << ',' << NUM_LINES << ',' << RATE(tavgint, NUM_LINES) << ',' << 0 << ',' << 0 << ',' << BIL(tavgquery) << ',' << BIL(tavginsert) << ',' << BIL(tavgdelete) << ',' <<
                   RATEB(NUM_LINES, tavgquery) << ',' << RATEB(NUM_SEGS, tavginsert)  << ',' << RATEB(NUM_SEGS, tavgdelete) << std::endl;
        accelerated << ((u - MINLVL) / FACTOR) + 1 << ',' << K << ',' << u << ',' << NUM_LINES << ',' << RATE(aavgint, NUM_LINES) << ',' << BIL(aavgconstruct) << ',' << MIL(aavgspace) << ',' << BIL(aavgquery) << ',' << BIL(aavginsert) << ',' << BIL(aavgdelete) << ',' <<
                       RATEB(NUM_LINES, aavgquery) << ',' << RATEB(NUM_SEGS, aavginsert)  << ',' << RATEB(NUM_SEGS, aavgdelete) << std::endl;
    }

    typical.close();
    accelerated.close();

    std::cout << "Done\nCompleted general tests.\n" << std::endl;

#endif

#ifdef TEST_L

    ltt.open("ltt.csv");
    ltt << "l,numsegs,numlines,intersections,createtime,space,querytime,inserttime,deletetime,queryrate,insertrate,deleterate" << std::endl;

    lta.open("lta.csv");
    lta << "l,numsegs,numlines,intersections,createtime,space,querytime,inserttime,deletetime,queryrate,insertrate,deleterate" << std::endl;

    std::cout << "Running l-value tests.\nRemaining: " << std::flush;

    /* l Tests */
    for(auto l = MINL; l < MAXL; l += FACTORL)
    {
        std::cout << (unsigned int)(((MAXL - l) / FACTORL) + 1) << "... " << std::flush;

        size_t tavgconstruct = 0;
        size_t tavgspace = 0;
        size_t tavgquery = 0;
        size_t tavgint = 0;
        size_t tavginsert = 0;
        size_t tavgdelete = 0;

        size_t aavgconstruct = 0;
        size_t aavgspace = 0;
        size_t aavgquery = 0;
        size_t aavgint = 0;
        size_t aavginsert = 0;
        size_t aavgdelete = 0;

        for(auto i = 0; i < TESTS; i++)
        {
            // Generate lines and geometry
            auto lines = randomLines(NUM_LINES, BOUNDS);
            auto segments = randomSegments(NUM_SEGS, BOUNDS, l);
            auto inserts = randomSegments(NUM_SEGS, BOUNDS, l);

#ifdef TEST_DEBUG
            std::cout << "\nGENERATED\n" << std::endl;
#endif

            // Typical test

            // No construction
            // No space

            // Query
            auto tstart = now();
            vec2 tmp = vec2();
            for(auto& l : lines)
            {
                for(auto& s : segments)
                {
                    if(l.intersect(s, tmp))
                    {
                        tavgint++;
                    }
                }
            }
            auto tend = now();

            // Insert
            auto tistart = now();
            std::vector<segment> tt;
            tt.insert(tt.end(), inserts.begin(), inserts.end());
            auto tiend = now();

            // Delete
            auto tdstart = now();
            tt.erase(tt.begin() + NUM_SEGS, tt.end());
            auto tdend = now();

            tavgconstruct += 0;
            tavgspace += 0;
            tavgquery += tend - tstart;
            tavginsert += tiend - tistart;
            tavgdelete += tdend - tdstart;

#ifdef TEST_DEBUG
            std::cout << "\nTYPICAL\n" << std::endl;
#endif

            // Accelerated

            // Construction
            auto acstart = now();
            IRM irm(K, segments);
            auto acend = now();

#ifdef TEST_DEBUG
            std::cout << "\nCONSTRUCTED\n" << std::endl;
#endif

            // Space
            auto sz = irm.rawSize();

            // Query
            auto astart = now();
            for(auto& l : lines)
            {
                // Don't store result to make benchmark matched
                aavgint += irm.querySize(l);
            }
            auto aend = now();

#ifdef TEST_DEBUG
            std::cout << "\nQUERIED\n" << std::endl;
#endif

            // Insert
            auto aistart = now();
            irm.insert(inserts);
            auto aiend = now();

            // Delete
            auto adstart = now();
            irm.remove(0);
            auto adend = now();

            aavgconstruct += acend - acstart;
            aavgspace += sz;
            aavgquery += aend - astart;
            aavginsert += aiend - aistart;
            aavgdelete += adend - adstart;
        }

        tavgint /= TESTS;
        aavgint /= TESTS;

        tavgconstruct /= TESTS;
        tavgspace /= TESTS;
        tavgquery /= TESTS;
        tavginsert /= TESTS;
        tavgdelete /= TESTS;

        aavgconstruct /= TESTS;
        aavgspace /= TESTS;
        aavgquery /= TESTS;
        aavginsert /= TESTS;
        aavgdelete /= TESTS;

        ltt << l << ',' << NUM_SEGS << ',' << NUM_LINES << ',' << RATE(tavgint, NUM_LINES) << ',' << BIL(tavgconstruct) << ',' << MIL(tavgspace) << ',' << BIL(tavgquery) << ',' << BIL(tavginsert) << ',' << BIL(tavgdelete) << ',' <<
               RATEB(NUM_LINES, tavgquery) << ',' << RATEB(NUM_SEGS, tavginsert)  << ',' << RATEB(NUM_SEGS, tavgdelete) << std::endl;
        lta << l << ',' << NUM_SEGS << ',' << NUM_LINES << ',' << RATE(aavgint, NUM_LINES) << ',' << BIL(aavgconstruct) << ',' << MIL(aavgspace) << ',' << BIL(aavgquery) << ',' << BIL(aavginsert) << ',' << BIL(aavgdelete) << ',' <<
               RATEB(NUM_LINES, aavgquery) << ',' << RATEB(NUM_SEGS, aavginsert)  << ',' << RATEB(NUM_SEGS, aavgdelete) << std::endl;
    }

    ltt.close();
    lta.close();

    std::cout << "Done\nCompleted l-value tests.\n" << std::endl;
#endif

    // Ensure compiler doesn't optimize benchmarks away
    std::cout << "Ignore this: " << (trick.x - trick.y > 0.0F ? ">" : "<") << std::endl;
    std::cout << "\nBenchmark completed." << std::endl;

    return 0;
}
