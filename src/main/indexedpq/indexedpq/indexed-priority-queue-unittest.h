#ifndef __INDEXEDPQ_TEST_INDEXED_PRIORITY_QUEUE_UNITTEST_H__
#define __INDEXEDPQ_TEST_INDEXED_PRIORITY_QUEUE_UNITTEST_H__

#include "indexedpq/indexed-priority-queue.h"
#include <cppunit/extensions/HelperMacros.h>
#include <random>
#include <functional>
#include <sys/time.h>

template<typename T>
class RandomUtil
{
public:
    RandomUtil() { engine.seed(time(NULL)); }
    RandomUtil(uint64_t seed) { engine.seed(seed); }

    static T RandomInt64(T min, T max) {
        std::uniform_int_distribution<T> distribution(min, max);
        return distribution(engine);
    }
    static void SetSeed(uint64_t seed) { engine.seed(seed); }
private:
    static std::default_random_engine engine;
};

template<typename T> std::default_random_engine RandomUtil<T>::engine;

class TimeUtil
{
public:
    static int64_t CurrentTimeInMicroSeconds() {
        struct timeval tval;
        gettimeofday(&tval, NULL);
        return (tval.tv_sec * 1000000LL + tval.tv_usec);
    }
};

class IndexedPriorityQueueTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IndexedPriorityQueueTest);
    CPPUNIT_TEST(NormalTest);
    CPPUNIT_TEST(TopNRemoveDuplicateTest);
    CPPUNIT_TEST_SUITE_END();
public:
    IndexedPriorityQueueTest() {}
    ~IndexedPriorityQueueTest() {}
    void setUp() {}
    void tearDown() {}
public:

    void NormalTest();
    void TopNRemoveDuplicateTest();

private:
    void InternalTopNRemoveDuplicateTest(std::ostream& os, size_t docCount , size_t fingerCount , size_t topN, size_t runTimes);
};


#endif //__INDEXEDPQ_TEST_INDEXED_PRIORITY_QUEUE_UNITTEST_H__
