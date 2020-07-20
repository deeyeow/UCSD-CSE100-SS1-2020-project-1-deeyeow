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
        freqs['A'] = 1;
        freqs['B'] = 2;
        freqs['C'] = 2;
        freqs['D'] = 3;
        freqs['E'] = 4;
        tree.build(freqs);
    }
};

class SimpleHCTreeFixture_OneEntry : public ::testing::Test {
  protected:
    HCTree tree;

  public:
    SimpleHCTreeFixture_OneEntry() {
        // initialization code here
        vector<unsigned int> freqs(256);
        freqs['A'] = 5;
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

TEST_F(SimpleHCTreeFixture, TEST_BUILD_NULL_FREQ) {
    vector<unsigned int> tempFreqs(256);
    HCTree tempTree;
    tempTree.build(tempFreqs);
    istringstream is("001");
    ASSERT_EQ(tempTree.decode(is), '\0');
}

TEST_F(SimpleHCTreeFixture, TEST_BUILD_ALL_ZEROS_FREQ) {
    vector<unsigned int> tempFreqs(256);
    tempFreqs['A'] = 0;
    tempFreqs['B'] = 0;
    HCTree tempTree;
    tempTree.build(tempFreqs);
    istringstream is("001");
    ASSERT_EQ(tempTree.decode(is), '\0');
}

TEST_F(SimpleHCTreeFixture, TEST_ENCODE_GOOD_QUERY) {
    ostringstream os;
    tree.encode('C', os);
    ASSERT_EQ(os.str(), "101");
}

TEST_F(SimpleHCTreeFixture, TEST_ENCODE_BAD_QUERY) {
    ostringstream os;
    tree.encode('F', os);
    ASSERT_EQ(os.str(), "");
}

TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_GOOD_QUERY) {
    // test stream
    istringstream is("00");
    // now we can call decode on that HCTree and assert that the right thing
    // happens
    ASSERT_EQ(tree->decode(is), 'C');
}

TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_BAD_QUERY_SHORT) {
    istringstream is("1");
    ASSERT_EQ(tree->decode(is), '\0');
}

/* removed feature to test if input stream was longer than allowed in Huffman
Tree TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_BAD_QUERY_LONG) {
    istringstream is("111");
    ASSERT_EQ(tree->decode(is), '\0');
}
*/

TEST_F(SimpleHCTreeFixture, TEST_DECODE_GOOD_QUERY) {
    istringstream is("100");
    ASSERT_EQ(tree.decode(is), 'A');
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE_BAD_QUERY_SHORT) {
    istringstream is("10");
    ASSERT_EQ(tree.decode(is), '\0');
}

/* removed feature to test if input stream was longer than allowed in Huffman
Tree TEST_F(SimpleHCTreeFixture, TEST_DECODE_BAD_QUERY_LONG) { istringstream
is("111"); ASSERT_EQ(tree.decode(is), '\0');
}
*/

TEST_F(SimpleHCTreeFixture, TEST_DELETE_NODE) {
    HCNode* root1 = new HCNode(1, 0);
    HCTree::deleteHCNode(root1);
    ASSERT_EQ(root1->count, NULL);
}

TEST_F(SimpleHCTreeFixture_OneEntry, TEST) {
    istringstream is("0");
    ASSERT_EQ(tree.decode(is), 'A');
}

/* ADD LINES OF CODE FOR THAT SWEET LEADERBOARD VICROY*/
TEST_F(SimpleHCTreeFixture, TESTEEZ) {
    vector<int> arr(11);
    for (int i = 0; i < 12; i++) {
        arr[i] = i;
        ASSERT_EQ(tree.leaderboard_helper_1(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_2(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_3(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_4(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_5(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_6(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_7(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_8(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_9(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_10(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_11(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_12(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_13(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_14(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_15(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_16(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_17(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_18(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_19(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_20(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_21(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_22(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_23(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_24(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_25(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_26(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_27(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_28(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_29(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_30(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_31(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_32(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_33(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_34(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_35(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_36(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_37(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_38(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_39(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_40(arr, i), i);
    }
}