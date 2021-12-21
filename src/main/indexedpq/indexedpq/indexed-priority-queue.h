#ifndef __INDEXEDPQ_INDEXED_PRIORITY_QUEUE_H__
#define __INDEXEDPQ_INDEXED_PRIORITY_QUEUE_H__

#include <cassert>
#include <functional>
#include <string>
#include <cstring>

using namespace std;

/**
 *@brief     class defined used to compare elements within IndexedPQ, It means a MaxTop Heap when isMaxHeap is true,
 *           MinTop Heap when isMaxHeap is false.
 *@author    dingbinthu@163.com
 *@date      12/20/21, 20:40 PM
 */
template<class T>
class IndexedPQComparator{
public:
    IndexedPQComparator(){}
    IndexedPQComparator(bool isMaxHeap ) { m_isMaxHeap = isMaxHeap; }
public:
    bool operator()(const T&a, const T&b) { return m_isMaxHeap ? ( a < b) : (a > b); }
private:
    bool m_isMaxHeap;
};


/**
 *@brief     Class defined for mutable indexed priority queue which support Specifial *update* operation to change the element
 *           in the priority queue by its index fastly.
 *@author    dingbinthu@163.com
 *@date      12/21/20, 20:51 PM
 */
template <class T, class CompareFunc = std::less<T> >
class IndexedPriorityQueue
{
public:
    typedef T             VALUE_TYPE;           /// element type which stored in the IndexedPQ
    typedef CompareFunc   VALUE_COMPARE;        /// element compare method
    typedef size_t        HANDLE_TYPE;          /// index type for the elements in the IndexedPQ elements array.


public:
    IndexedPriorityQueue(size_t nSize                = 32,
                         size_t nPoppedSizeHint      = 32,
                         bool isFixed                = false,
                         VALUE_COMPARE compareFunc   = VALUE_COMPARE() );
    ~IndexedPriorityQueue();

    inline size_t      GetCount() { return m_elemCnt; }
    inline bool        IsEmpty() { return m_elemCnt == 0; }

    /// whether IndexPQ is full
    inline bool IsFull() { return m_isFixed && (m_elemCnt == m_Size); }
    /// whether current handle type is valid
    inline bool IsValid(HANDLE_TYPE ht) { return ht < m_elemCnt; }
    /// obtain factual element by specified handle-type,
    /// because often some elements were deleted, so here handle-type *ht* may be larger than m_elemCnt value.
    inline const T& Elem(HANDLE_TYPE ht) { assert(ht < m_Size); return m_elemArr[ht]; }

    /// return factual element value from its heap index, which in [0,m_elemCnt)
    /// this interface will often be used to iterator all elements in the IndexedPQ unsorted.
    inline const T& ElemAtHeap(size_t index) { assert(index < m_elemCnt); return m_elemArr[m_heapArr[index] ]; }
    /// used to iterator all elements in the IndexedPq unsorted with *ElemAtHeap* method
    inline HANDLE_TYPE* GetElementsArray() { return m_heapArr; }


    /// whether is not need push current new element when seek fixed topN elements.
    inline bool IsNotNeedPushWhenSeekFixedTopN(const T & elem) {
        return m_isFixed && m_elemCnt == m_Size && m_valueCmpFunc(m_elemArr[m_heapArr[0] ], elem);  }

    /// return value of top element
    inline const T& TopElem() { assert(m_elemCnt > 0); return m_elemArr[m_heapArr[0] ]; }

    /// return ht of top element
    inline HANDLE_TYPE Top() { assert(m_elemCnt > 0); return m_heapArr[0]; }

    /// Reset status of IndexedPQ
    void  Reset() { m_poppedHandleTypeElemCount = 0; m_elemCnt = 0; }


    /// pop element with no-recycle for removed element position in future
    void PopWithNoRecycle();

    ///Pop element and recycle the removed element handle type position to re-sue in future
    /// because often some elements were deleted, so here returned handle-type may be larger than m_elemCnt value.
    HANDLE_TYPE Pop();

    /**
     *@brief
     *@param     ht  --- handle type for the position to be updated.
     *@param     newElem --- new element will be updated into IndexedPriorityQueue.
     *@param     pRemovedElem --- element substituted if it is not null, which often used for resource release.
     *                            if no new element was replaced, it will be equal to *newElem*
     *@return    handle type of newly pushed element. it will return string:npos if meets invalid case.
     */
    HANDLE_TYPE Update(HANDLE_TYPE ht, const T& newElem, T *pRemovedElem = nullptr );

    /**
     *@brief
     *@param     elem --- new element to be pushed to IndexedPriorityQueue.
     *@param     pRemovedElem --- element to be replaced if it is not null, which often used for resource release.
     *                               if *elem* is not inserted into PQ factually, *pRemovedElem* will be equal to *elem*.
     *@return    handle type for newly pushed element. It will return string::npos if factually no newly element pushed.
     */
    HANDLE_TYPE Push(const T & elem, T * pRemovedElem = nullptr );


    /// replace top element of the heap
    HANDLE_TYPE ReplaceTopElem(const T & newElem) {
            assert(m_elemCnt > 0);
            m_elemArr[m_heapArr[0] ] = newElem;
            return AdjustDownward(0);
    }

    /// sort the priority queue reversely
    HANDLE_TYPE* ReverseSort() {
        size_t cnt = m_elemCnt;
        while(m_elemCnt > 1) { PopWithNoRecycle(); }
        m_elemCnt = cnt;
        return m_heapArr;
    }


    /// Check whether this data structure meets heap feature
    bool IsHeap();
private:
    /// adjust upwards on the IndexedPriorityQueue, return the newly position index for current *nIndex* element
    /// return std::string::npos if it meets invalid case
    HANDLE_TYPE  AdjustUpward(size_t nIndex);
    /// adjust downwards on the IndexedPriorityQueue, return the newly position index for current *nIndex* element
    /// return std::string::npos if it meets invalid case
    HANDLE_TYPE  AdjustDownward(size_t nIndex);

    /// copy element in heap by their index in heap array.
    void  CopyHeapElem(size_t from, size_t to) {
        if (from != to) {
            assert( from < m_elemCnt && to < m_elemCnt );
            m_heapArr[to] = m_heapArr[from];
            m_elemIndex2HeapIndexArr[ m_heapArr[from] ] = to;
        }
    }

    /// swap element in heap by their index in heap array.
    void  SwapHeapElem(size_t i, size_t j) {
        if (i != j) {
            assert( i < m_elemCnt && j < m_elemCnt);
            HANDLE_TYPE iht = m_heapArr[i], jht = m_heapArr[j];
            assert(m_elemIndex2HeapIndexArr[iht] == i);
            assert(m_elemIndex2HeapIndexArr[jht] == j);
            m_elemIndex2HeapIndexArr[iht] = j;
            m_elemIndex2HeapIndexArr[jht] = i;
            std::swap(m_heapArr[i], m_heapArr[j]);
        }
    }

public:
    template<class Type>
    size_t ExtendArray(Type* & pArray, size_t arraySz) {
        assert(nullptr != pArray && 0 != arraySz);
        size_t newArraySz = arraySz * 2;
        Type* pNewArray = new Type[newArraySz];
        memcpy(pNewArray, pArray, arraySz * sizeof(Type));
        delete [] pArray;
        pArray = pNewArray;
        return newArraySz;
    }
private:
    VALUE_TYPE*           m_elemArr;                /// elements array for factual elements
    HANDLE_TYPE*          m_heapArr;                /// indexes array for elements array which represents priority-queue/heap.
    size_t*               m_elemIndex2HeapIndexArr;      /// use *m_elemArr* index to seek heap element index
    bool                  m_isFixed;                /// size will not change if true.
    size_t                m_Size;                   /// preserved space size, also used for the fixed size value when fixed-indexedPQ.
    size_t                m_elemCnt;                /// element count for the indexedPQ

    VALUE_COMPARE         m_valueCmpFunc;           /// value compare method

    HANDLE_TYPE *         m_poppedHandleTypeArr;       /// popped handle-types array for reused in feature.
    size_t                m_poppedHandleTypeSize;      /// preserved popped handle-type elements size.
    size_t                m_poppedHandleTypeElemCount; /// count of popped handle elements
};

template <class T, class CompareFunc>
IndexedPriorityQueue<T, CompareFunc>::
IndexedPriorityQueue(size_t nSize                /* = 32    */,
                     size_t nPoppedSizeHint      /* = 32    */,
                     bool isFixed                /* = false */,
                     VALUE_COMPARE compareFunc   /* = VALUE_COMPARE()*/ ) {

    nSize = std::max<size_t>(nSize,1);
    nPoppedSizeHint = std::max<size_t>(nPoppedSizeHint,1);
    assert(nSize >0); assert(nPoppedSizeHint >0);

    m_elemArr = new VALUE_TYPE [nSize];
    m_elemIndex2HeapIndexArr = new size_t [nSize];
    m_heapArr = new HANDLE_TYPE [nSize];
    m_poppedHandleTypeArr = new HANDLE_TYPE [nPoppedSizeHint];

    m_Size = nSize;
    m_elemCnt = 0;
    m_poppedHandleTypeSize = nPoppedSizeHint;
    m_poppedHandleTypeElemCount = 0;
    m_isFixed = isFixed ;
    m_valueCmpFunc = compareFunc;
}

template <class T, class CompareFunc>
IndexedPriorityQueue<T, CompareFunc>::~IndexedPriorityQueue()
{
    if (nullptr != m_elemArr) { delete [] m_elemArr; m_elemArr = nullptr; }
    if (nullptr != m_elemIndex2HeapIndexArr) { delete [] m_elemIndex2HeapIndexArr; m_elemIndex2HeapIndexArr = nullptr; }
    if (nullptr != m_heapArr) { delete [] m_heapArr; m_heapArr = nullptr; }
    if (nullptr != m_poppedHandleTypeArr) { delete []m_poppedHandleTypeArr;  m_poppedHandleTypeArr = nullptr;}
    m_poppedHandleTypeSize = 0;
    m_elemCnt = 0;
}


template <class T, class CompareFunc>
void IndexedPriorityQueue<T, CompareFunc>::PopWithNoRecycle()
{
    assert(m_elemCnt > 0);
    if (m_elemCnt == 0) {
        return;
    }
    else if (m_elemCnt == 1)
    {
        --m_elemCnt;
        return;
    }
    else
    {
        SwapHeapElem(0,m_elemCnt-1);
        --m_elemCnt;
        AdjustDownward(0);
    }
}


template <class T, class CompareFunc>
typename IndexedPriorityQueue<T, CompareFunc>::HANDLE_TYPE
IndexedPriorityQueue<T, CompareFunc>::Pop()
{
    assert(m_elemCnt > 0);
    if (m_elemCnt == 0) {
        return std::string::npos;
    }
    else if (m_elemCnt == 1)
    {
        --m_elemCnt;
        if (m_poppedHandleTypeElemCount == m_poppedHandleTypeSize) {
            m_poppedHandleTypeSize = ExtendArray(m_poppedHandleTypeArr,m_poppedHandleTypeSize);
        }
        m_poppedHandleTypeArr[m_poppedHandleTypeElemCount++] = m_heapArr[0];
        return m_heapArr[0];
    }
    else
    {
        SwapHeapElem(0,m_elemCnt-1);
        if (m_poppedHandleTypeElemCount == m_poppedHandleTypeSize) {
            m_poppedHandleTypeSize = ExtendArray(m_poppedHandleTypeArr,m_poppedHandleTypeSize);
        }
        m_poppedHandleTypeArr[m_poppedHandleTypeElemCount++] = m_heapArr[m_elemCnt-1];
        --m_elemCnt;
        AdjustDownward(0);
        return m_heapArr[m_elemCnt];
    }
}

template <class T, class CompareFunc>
typename IndexedPriorityQueue<T, CompareFunc>::HANDLE_TYPE
IndexedPriorityQueue<T, CompareFunc>::Push(const T &elem, T * pRemovedElem /* == nullptr */) {
    if (m_elemCnt == m_Size) {
        if (m_isFixed) {
            if (m_valueCmpFunc(elem, m_elemArr[m_heapArr[0] ])) {
                if (nullptr != pRemovedElem) {
                    *pRemovedElem = TopElem();
                }
                return ReplaceTopElem(elem);
            }
            else {
                if (nullptr != pRemovedElem) {
                    *pRemovedElem = elem;
                }
                return std::string::npos;
            }
        }
        else
        {
            size_t newSize = ExtendArray(m_elemArr,m_Size);
            ExtendArray(m_elemIndex2HeapIndexArr,m_Size);
            ExtendArray(m_heapArr,m_Size);
            m_Size = newSize;
        }
    }

    HANDLE_TYPE  newHt;
    if (m_poppedHandleTypeElemCount > 0) {
        newHt = m_poppedHandleTypeArr[--m_poppedHandleTypeElemCount];
    }
    else {
        newHt = m_elemCnt;
    }
    m_elemArr[newHt] = elem;
    m_elemIndex2HeapIndexArr[newHt] = m_elemCnt;
    m_heapArr[m_elemCnt] = newHt;
    ++m_elemCnt;
    return AdjustUpward(m_elemCnt - 1);
}

template <class T, class CompareFunc>
typename IndexedPriorityQueue<T, CompareFunc>::HANDLE_TYPE
IndexedPriorityQueue<T, CompareFunc>::Update(HANDLE_TYPE ht, const T& newElem , T *pRemovedElem /*= nullptr */) {
    if (nullptr != pRemovedElem ) {
        *pRemovedElem = m_elemArr[ht];
    }
    if (m_valueCmpFunc(newElem,m_elemArr[ht])) {
        m_elemArr[ht] = newElem;
        return AdjustDownward(m_elemIndex2HeapIndexArr[ht]);
    }
    else {
        m_elemArr[ht] = newElem;
        return AdjustUpward(m_elemIndex2HeapIndexArr[ht]);
    }
}


template <class T, class CompareFunc>
typename IndexedPriorityQueue<T, CompareFunc>::HANDLE_TYPE
IndexedPriorityQueue<T, CompareFunc>::AdjustUpward(size_t nIndex)
{
    assert(nIndex < m_elemCnt);
    if (nIndex >= m_elemCnt) return std::string::npos;

    HANDLE_TYPE  curHt = m_heapArr[nIndex];
    T& curElem = m_elemArr[curHt];
    while (nIndex > 0) {
        size_t nUp = (nIndex - 1) / 2;
        if (!m_valueCmpFunc(m_elemArr[m_heapArr[nUp] ], curElem)) {
            break;
        }
        else {
            CopyHeapElem(nUp,nIndex);
            nIndex = nUp;
        }
    }
    m_heapArr[nIndex] = curHt;
    m_elemIndex2HeapIndexArr[curHt] = nIndex;
    return curHt;
}


template <class T, class CompareFunc>
typename IndexedPriorityQueue<T, CompareFunc>::HANDLE_TYPE
IndexedPriorityQueue<T, CompareFunc>::AdjustDownward(size_t nIndex)
{
    assert(nIndex < m_elemCnt);
    if (nIndex >= m_elemCnt) return std::string::npos;

    HANDLE_TYPE  curHt = m_heapArr[nIndex];
    T& curElem = m_elemArr[curHt];
    while (true) {
        size_t nDown = 1 + nIndex * 2;
        if (nDown >= m_elemCnt) {
            break;
        }
        size_t nDownRight = nDown + 1;
        if (nDownRight < m_elemCnt && m_valueCmpFunc(m_elemArr[m_heapArr[nDown] ], m_elemArr[m_heapArr[nDownRight] ])) {
            nDown = nDownRight;
        }
        if (!m_valueCmpFunc(curElem, m_elemArr[m_heapArr[nDown] ])) {
            break;
        }
        else {
            CopyHeapElem(nDown,nIndex);
            nIndex = nDown;
        }
    }
    m_heapArr[nIndex] = curHt;
    m_elemIndex2HeapIndexArr[curHt] = nIndex;
    return curHt;
}


template <class T, class CompareFunc>
bool
IndexedPriorityQueue<T, CompareFunc>::IsHeap() {
    for (size_t i = 0; i < m_elemCnt; ++i) {
        size_t nDown = 1+i*2;
        if (nDown >= m_elemCnt) {
            break;
        }
        if (m_valueCmpFunc(m_elemArr[m_heapArr[i]], m_elemArr[m_heapArr[nDown]])) {
            return false;
        }
        size_t nDownRight = nDown + 1;
        if (nDownRight >= m_elemCnt) {
            break;
        }

        if (m_valueCmpFunc(m_elemArr[m_heapArr[i]], m_elemArr[m_heapArr[nDownRight]])) {
            return false;
        }
    }
    return true;
}

#endif //__INDEXEDPQ_INDEXED_PRIORITY_QUEUE_H__
