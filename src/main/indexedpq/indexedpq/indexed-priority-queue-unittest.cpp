#include "indexedpq/indexed-priority-queue-unittest.h"
#include <cppunit/TestAssert.h>
#include <algorithm>
#include <random>
#include <ctime>
#include <vector>
#include <map>

using namespace std;
CPPUNIT_TEST_SUITE_REGISTRATION(IndexedPriorityQueueTest);


void IndexedPriorityQueueTest::NormalTest()
{
    ///1
    IndexedPriorityQueue<uint32_t> pq;
    CPPUNIT_ASSERT_EQUAL((size_t)0, pq.GetCount());
    CPPUNIT_ASSERT(pq.IsEmpty());
    vector<uint32_t> arr;
    for (uint32_t i = 0; i < 5000; ++i) {
        uint32_t value = rand();
        pq.Push(value);
        CPPUNIT_ASSERT(pq.IsHeap());
        arr.push_back(value);
    }
    CPPUNIT_ASSERT_EQUAL(arr.size(), pq.GetCount());
    CPPUNIT_ASSERT(!pq.IsEmpty());
    sort(arr.begin(), arr.end());
    for (vector<uint32_t>::reverse_iterator rit = arr.rbegin(); rit != arr.rend(); ++rit) {
        CPPUNIT_ASSERT_EQUAL(*rit, pq.TopElem());
        pq.Pop();
        CPPUNIT_ASSERT(pq.IsHeap());
    }


    ///2
    IndexedPriorityQueue<uint32_t, greater<uint32_t> > pq2;
    CPPUNIT_ASSERT_EQUAL((size_t)0, pq2.GetCount());
    CPPUNIT_ASSERT(pq2.IsEmpty());
    arr.clear();
    for (uint32_t i = 0; i < 50000; ++i) {
        uint32_t value = rand();
        pq2.Push(value);
        CPPUNIT_ASSERT(pq2.IsHeap());
        arr.push_back(value);
    }
    for (uint32_t i = 0; i < 20000; ++i)
    {
        uint32_t toReplace = pq2.TopElem();
        uint32_t newValue = toReplace - 20000;
        pq2.ReplaceTopElem(newValue);
        CPPUNIT_ASSERT(pq2.IsHeap());
        for (size_t j = 0; j < arr.size(); ++j) {
            if (arr[j] == toReplace) {
                arr[j] = newValue;
                break;
            }
        }
    }
    CPPUNIT_ASSERT_EQUAL(arr.size(), pq2.GetCount());
    CPPUNIT_ASSERT(!pq2.IsEmpty());
    sort(arr.begin(), arr.end());
    for (vector<uint32_t>::iterator it = arr.begin(); it != arr.end(); ++it) {
        CPPUNIT_ASSERT_EQUAL(*it, pq2.TopElem());
        pq2.Pop();
        CPPUNIT_ASSERT(pq2.IsHeap());
    }

    ///3
    uint32_t count = 200;
    typedef IndexedPQComparator<uint64_t> Comparator;
    typedef IndexedPriorityQueue<uint64_t, Comparator> Queue;
    Comparator comparator(true);
    Queue pq3(count,count,true, comparator);
    std::vector<uint64_t> candi;
    for(uint32_t i =0;i < count*2;i++){
        pq3.Push(i);
        CPPUNIT_ASSERT(pq3.IsHeap());
    }
    for(uint32_t i = 0;i < count;i++){
        CPPUNIT_ASSERT_EQUAL(uint64_t(count-i-1), pq3.TopElem());
        pq3.Pop();
        CPPUNIT_ASSERT(pq3.IsHeap());
    }

    ///4
    class IntPointerCmp{
    public:
        bool operator()(const uint64_t* a , const uint64_t* b){
            if(*a > *b) return true;
            else return false;
        }
    };
    count = 20;
    IndexedPriorityQueue<uint64_t*, IntPointerCmp> pq4(count,count,true, IntPointerCmp());
    uint64_t* candi2 = new uint64_t[count*2];
    for(uint32_t i = 0 ;i < count*2;i++){
        candi2[i] = 2*count - i - 1;
        pq4.Push(candi2 + i);
        CPPUNIT_ASSERT(pq4.IsHeap());
    }

    for(uint32_t i = 0;i < count;i++){
        CPPUNIT_ASSERT_EQUAL(uint64_t(i + count), *(pq4.TopElem()));
        pq4.Pop();
        CPPUNIT_ASSERT(pq4.IsHeap());
    }
    delete []candi2;
}


class FakeItem {
public:
    bool operator!=(const FakeItem & ref) {
        return !(*this == ref);
    }
    bool operator==(const FakeItem & ref) {
        if (this != &ref) {
            return (m_id == ref.m_id ) && (m_score == ref.m_score) && (m_flag == ref.m_flag );
        }
        return true;
    }

    std::string ToString() {
        char buf[64];
        snprintf(buf,64,"%-20lu%-20lu%-20lu",m_score,m_id,m_flag);
        return buf;
    }
public:
    uint64_t   m_id;
    uint64_t   m_score;
    uint64_t   m_flag; ///used to remove duplicate
};

class FakeItemMgr {
public:
    static void sGenFlags(size_t count, vector<uint64_t >& flags );
    static void sGenScores(size_t count, vector<uint64_t >& scores);
    static void sGenItems(size_t itemCount,std::size_t flagCount, std::vector<FakeItem>& itemsVec);
};

void FakeItemMgr::sGenFlags (size_t count, vector<uint64_t >& flags ) {
    flags .resize(count);
    flags .clear();
    set<int64_t> sets;
    for (size_t i = 0; i < count; ++i) {
        int64_t t;
        do {
            t = RandomUtil<int64_t>::RandomInt64(1e1,1e8);
        } while (sets.find(t) != sets.end());
        flags .push_back(t);
        sets.insert(t);
    }
}

void FakeItemMgr::sGenScores(size_t count, vector<uint64_t >& scores) {
    scores.clear();
    for (size_t i = 0; i < count; ++i) {
        int64_t t = RandomUtil<int64_t>::RandomInt64(1e4,1e8);
        scores.push_back(t);
    }
}

void FakeItemMgr::sGenItems(size_t itemCount,std::size_t flagCount, std::vector<FakeItem>& itemsVec) {
    itemsVec.resize(itemCount);
    itemsVec.clear();
    vector<uint64_t > flags, scores;
    sGenFlags (flagCount,flags);
    sGenScores(itemCount, scores);
    for (size_t i = 0; i < itemCount; ++i) {
        FakeItem item;
        item.m_id = i;
        item.m_score = scores[i];
        size_t x = RandomUtil<int64_t>::RandomInt64(0,flagCount-1);
        item.m_flag = flags[x];
        itemsVec[i] = item;
    }
}

/// test remove duplicate items and obtain TOPN item use IndexedPriorityQueue
void IndexedPriorityQueueTest::TopNRemoveDuplicateTest() {
    vector<size_t> itemCounts, flagsCounts, topNs;
    itemCounts.push_back(2e4);
    itemCounts.push_back(2e3);
    itemCounts.push_back(2e2);

    flagsCounts.push_back(2e3);
    flagsCounts.push_back(2e2);
    flagsCounts.push_back(2e1);

    topNs.push_back(4e2);
    topNs.push_back(3e2);
    topNs.push_back(1e3);
    topNs.push_back(80);

    size_t runTimes = 2;
    std::string resultFile = "result.txt";
    std::ofstream  os(resultFile);
    assert(os);
    for (size_t i = 0; i < itemCounts.size(); ++i) {
        for (size_t j = 0; j < flagsCounts.size(); ++j) {
            for (size_t k = 0; k < topNs.size(); ++k) {
                InternalTopNRemoveDuplicateTest(os,itemCounts[i],flagsCounts[j],topNs[k],runTimes);
            }
        }
    }
}

class FakeItemComparator {
public:
    bool operator() (const FakeItem & lhs, const FakeItem & rhs) {
        if (lhs.m_score != rhs.m_score) {
            return lhs.m_score > rhs.m_score;
        }
        else return lhs.m_id < rhs.m_id;
    }
};

void IndexedPriorityQueueTest::InternalTopNRemoveDuplicateTest( std::ostream& os,
                                                                size_t itemCount,
                                                                size_t flagCount,
                                                                size_t topN,
                                                                size_t runTimes ) {

    std::vector<FakeItem> itemsVec, topNItemsVec, allTopItemsVec;
    std::map<uint64_t,size_t> flag2PqHtMap;
    FakeItemMgr::sGenItems(itemCount,flagCount,itemsVec);

    FakeItemComparator itemComparator;
    IndexedPriorityQueue<FakeItem, FakeItemComparator>  queue(topN,128,true);
    uint64_t bruteForceTime = 0, pqTime = 0, totalRealTopN = 0;
    int64_t tBegin = 0, tEnd = 0;
    for (size_t times = 0; times < runTimes; ++times) {
        queue.Reset();
        flag2PqHtMap.clear();
        tBegin = TimeUtil::CurrentTimeInMicroSeconds();
        cout << "---------------------------------now begin---------------------------" << endl;
        for (size_t i = 0; i < itemCount; ++i) {
            FakeItem & item = itemsVec[i];
            if (queue.IsNotNeedPushWhenSeekFixedTopN(item)) {
                continue;
            }
            auto it = flag2PqHtMap.find(item.m_flag);
            if (it == flag2PqHtMap.end()) {
                ///not exists
                FakeItem oldItem;
                oldItem.m_flag = string::npos;
                auto ht = queue.Push(item, &oldItem);
                if (queue.IsValid(ht)) {
                    if (oldItem.m_flag == string::npos) {
                        ///newly pushed
                        flag2PqHtMap.emplace(item.m_flag,ht);
                    }
                    else {
                        ///replaced
                        auto oldIt = flag2PqHtMap.find(oldItem.m_flag);
                        auto oldFht = flag2PqHtMap.extract(oldIt);
                        oldFht.key() = item.m_flag;
                        oldFht.mapped() = ht;
                        flag2PqHtMap.insert(std::move(oldFht));
                    }
                }
            }
            else {
                ///exists
                auto ht = it->second;
                if (itemComparator(item,queue.Elem(ht))) {
                    queue.Update(ht,item);
                }
            }
        }
        topNItemsVec.resize(queue.GetCount());
        topNItemsVec.clear();
        auto sortedItemsVec = queue.ReverseSort();
        for (size_t i = 0; i < queue.GetCount(); ++i) {
            topNItemsVec.push_back(queue.Elem(sortedItemsVec [i]));
        }

        tEnd = TimeUtil::CurrentTimeInMicroSeconds();
        cout << "-------------itemCount:["<< itemCount <<"],flagCount:["<< flagCount <<"],topN:["<< topN <<"],Totally consumed time:[" << (tEnd - tBegin)/1000<<"]ms." << endl;
        pqTime += (tEnd-tBegin);

        ///Brute force to compute results
        map<uint64_t,FakeItem > flag2itemsAllMap;
        tBegin = TimeUtil::CurrentTimeInMicroSeconds();
        for (size_t i = 0; i < itemCount; ++i) {
            FakeItem & item = itemsVec[i];
            auto it = flag2itemsAllMap.find(item.m_flag);
            if (it == flag2itemsAllMap.end() ) {
                flag2itemsAllMap.emplace(item.m_flag,item);
            }
            else if ( itemComparator(item, it->second) ) {
                flag2itemsAllMap[item.m_flag] = item;
            }
        }
        allTopItemsVec.resize(flag2itemsAllMap.size());
        allTopItemsVec.clear();
        for (auto it : flag2itemsAllMap) {
            allTopItemsVec.push_back(it.second);
        }
        std::sort(allTopItemsVec.begin(), allTopItemsVec.end(), itemComparator );
        tEnd = TimeUtil::CurrentTimeInMicroSeconds();
        cout << "---------itemCount:["<< itemCount <<"],flagCount:["<< flagCount <<"],topN:["<< topN <<"], All top items consumed time:[" << (tEnd - tBegin)/1000<<"]ms." << endl;
        bruteForceTime += (tEnd - tBegin);
        size_t realTopN = topN;
        if (allTopItemsVec.size() < topN) {
            realTopN = allTopItemsVec.size();
        }
        totalRealTopN += realTopN;
        CPPUNIT_ASSERT_EQUAL(topNItemsVec.size(),queue.GetCount());
        CPPUNIT_ASSERT_EQUAL(realTopN,queue.GetCount());
        char buf[256];
        for (size_t i = 0; i < realTopN; ++i) {
            snprintf(buf,256,"for itemCount:%zu,flagCount:%zu,factualTopN:%zu,topN:%zu, the top %zuth element,allTopItem:[%s], item from queue:[%s]",
                     itemCount,flagCount, realTopN, topN, i, allTopItemsVec[i].ToString().c_str(),
                     topNItemsVec[i].ToString().c_str());
            CPPUNIT_ASSERT_MESSAGE(string(buf),allTopItemsVec[i] == topNItemsVec[i]);
        }
    }

    float nTimes = bruteForceTime/(pqTime?pqTime:1);
    bruteForceTime = bruteForceTime/runTimes/1000;
    pqTime = pqTime /runTimes/1000;

    os << itemCount << "," << flagCount << "," << topN << "," << totalRealTopN/runTimes
       <<"," << bruteForceTime << "," << pqTime << "," << nTimes << endl;
}

