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
        freqs['A'] = 2;
        freqs['B'] = 3;
        tree.build(freqs);
    }
};

class ManualHCTreeFixture : public ::testing::Test {
  protected:
    HCTree* tree;

  public:
    ManualHCTreeFixture() {
        // make it possible to create a HCTree without calling build
        // create some HCNodes that are linked to each other, to create HCTree

        // creating nodes
        HCNode* node1 = new HCNode(1, 'A');
        HCNode* node2 = new HCNode(2, 'B');
        HCNode* node3 = new HCNode(2, 'C');
        HCNode* node4 = new HCNode(3, 'D');
        HCNode* node5 = new HCNode(4, 'E');

        // linking nodes
        HCNode* combine1 =
            new HCNode(node1->count + node2->count, node1->symbol);
        combine1->c0 = node1;
        combine1->c1 = node2;

        HCNode* combine2 =
            new HCNode(node3->count + combine1->count, node3->symbol);
        combine2->c0 = node3;
        combine2->c1 = combine1;

        HCNode* combine3 =
            new HCNode(node4->count + node5->count, node4->symbol);
        combine3->c0 = node4;
        combine3->c1 = node5;

        HCNode* combine4 =
            new HCNode(combine2->count + combine3->count, combine2->symbol);
        combine4->c0 = combine2;
        combine4->c1 = combine3;

        // creating tree from node
        tree = new HCTree(combine4);
    }
};


TEST_F(SimpleHCTreeFixture, TEST_ENCODE) {
    ostringstream os;
    tree.encode('a', os);
    ASSERT_EQ(os.str(), "");
}

TEST_F(SimpleHCTreeFixture, TEST_DELETE_HCNODE) {
    HCNode* root1 = new HCNode(1, 0);
    HCTree::deleteHCNode(root1);
    ASSERT_EQ(root1->count, NULL);
}

TEST_F(SimpleHCTreeFixture, TEST_BUILD_NULL_FREQ) {
    vector<unsigned int> tempFreqs(256);
    HCTree tempTree;
    tempTree.build(tempFreqs);
    istringstream is("001");
    ASSERT_EQ(tempTree.decode(is), ' ');
}

TEST_F(SimpleHCTreeFixture, TEST_BUILD_ALL_ZEROS_FREQ) {
    vector<unsigned int> tempFreqs(256);
    tempFreqs['A'] = 0;
    tempFreqs['B'] = 0;
    HCTree tempTree;
    tempTree.build(tempFreqs);
    istringstream is("001");
    ASSERT_EQ(tempTree.decode(is), ' ');
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE) {
    istringstream is("0");
    ASSERT_EQ(tree.decode(is), 'A');
}

TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_GOOD_QUERY) {
    // test stream
    istringstream is("00");
    // now we can call decode on that HCTree and assert that the right thing
    // happens
    ASSERT_EQ(tree->decode(is), 'C');
}

TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_BAD_QUERY) {
    istringstream is("1");
    ASSERT_EQ(tree->decode(is), ' ');
}
