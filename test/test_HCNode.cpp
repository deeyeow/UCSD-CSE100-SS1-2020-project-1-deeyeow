#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "HCTree.hpp"

using namespace std;
using namespace testing;

class SimpleHCNodeFixture : public ::testing::Test {
  protected:
    HCNode* node = new HCNode(1, 1);

  public:
    SimpleHCNodeFixture() {}
};

TEST_F(SimpleHCNodeFixture, TEST_OPERATOR_GT_COUNT) {
    HCNode* node2 = new HCNode(2, 0);
    cout << *node << ", " << *node2 << endl;
    ASSERT_TRUE(*node2 < *node);
}

TEST_F(SimpleHCNodeFixture, TEST_OPERATOR_LT_COUNT) {
    HCNode* node2 = new HCNode(0, 0);
    cout << *node << ", " << *node2 << endl;
    ASSERT_TRUE(*node < *node2);
}

TEST_F(SimpleHCNodeFixture, TEST_OPERATOR_GT_SYMBOL) {
    HCNode* node2 = new HCNode(1, 0);
    cout << *node << ", " << *node2 << endl;
    ASSERT_TRUE(*node2 < *node);
}

TEST_F(SimpleHCNodeFixture, TEST_OPERATOR_LT_SYMBOL) {
    HCNode* node2 = new HCNode(1, 2);
    cout << *node << ", " << *node2 << endl;
    ASSERT_TRUE(*node < *node2);
}

/*
TEST_F(SimpleHCNodeFixture, TEST_COMPARATOR_LT_SYMBOL) {
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;
    HCNode* node2 = new HCNode(1, 6);
    pq.push(node);
    pq.push(node2);
    ASSERT_TRUE(node < node2);
}
*/