#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "HCTree.hpp"

using namespace std;
using namespace testing;

class SimpleHCTreeFixture : public ::testing::Test {
  protected:
    HCTree tree;

  public:
    SimpleHCTreeFixture() {
        // initialization code here
        vector<unsigned int> freqs(256);
        freqs['a'] = 2;
        freqs['b'] = 3;
        tree.build(freqs);
    }
};

TEST_F(SimpleHCTreeFixture, TEST_ENCODE) {
    ostringstream os;
    tree.encode('a', os);
    ASSERT_EQ(os.str(), "");
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE) {
    istringstream is("1");
    ASSERT_EQ(tree.decode(is), ' ');
}

TEST_F(SimpleHCTreeFixture, TEST_DELETE_HCNODE) {
    HCNode* root1 = new HCNode(1, 0);
    HCTree::deleteHCNode(root1);
    ASSERT_EQ(root1->count, NULL);
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE_NO_BUILD) {
    // make it possible to create a HCTree without calling build
    // HCTree(HCNode* root);

    // create some HCNodes that are linked to each other, to create HCTree

    // now we can call decode on that HCTree and assert that the right thing
    // happens
}
