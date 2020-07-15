#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "HCTree.hpp"

using namespace std;
using namespace testing;

class SimpleHCQueueFixture : public ::testing::Test {
  protected:
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;

  public:
    SimpleHCQueueFixture() {}
};

TEST_F(SimpleHCQueueFixture, TEST_PQ_COUNT) {
    pq.push(new HCNode(2, 0));
    pq.push(new HCNode(1, 0));

    HCNode* popped1 = pq.top();
    pq.pop();
    HCNode* popped2 = pq.top();
    cout << *popped1 << ", " << *popped2 << endl;
    ASSERT_TRUE(*popped2 < *popped1);
}

TEST_F(SimpleHCQueueFixture, TEST_PQ_SYMBOL) {
    pq.push(new HCNode(1, 0));
    pq.push(new HCNode(1, 1));

    HCNode* popped1 = pq.top();
    pq.pop();
    HCNode* popped2 = pq.top();
    cout << *popped1 << ", " << *popped2 << endl;
    ASSERT_TRUE(*popped2 < *popped1);
}
