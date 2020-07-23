#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "../bitStream/input/BitInputStream.hpp"
#include "../bitStream/output/BitOutputStream.hpp"
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

/*
TEST_F(SimpleHCTreeFixture, TEST_BUILD_NULL_FREQ) {
    vector<unsigned int> tempFreqs(256);
    HCTree tempTree;
    tempTree.build(tempFreqs);
    istringstream is("001");
    ASSERT_EQ(tempTree.decode(is), '\0');
}
*/

/*
TEST_F(SimpleHCTreeFixture, TEST_BUILD_ALL_ZEROS_FREQ) {
    vector<unsigned int> tempFreqs(256);
    tempFreqs['A'] = 0;
    tempFreqs['B'] = 0;
    HCTree tempTree;
    tempTree.build(tempFreqs);
    istringstream is("001");
    ASSERT_EQ(tempTree.decode(is), '\0');
}
*/

TEST_F(SimpleHCTreeFixture, TEST_ENCODE_GOOD_QUERY) {
    ostringstream os;
    tree.encode('C', os);
    ASSERT_EQ(os.str(), "101");
}

/*
TEST_F(SimpleHCTreeFixture, TEST_ENCODE_BAD_QUERY) {
    ostringstream os;
    tree.encode('F', os);
    ASSERT_EQ(os.str(), "");
}
*/

TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_GOOD_QUERY) {
    // test stream
    istringstream is("00");
    // now we can call decode on that HCTree and assert that the right thing
    // happens
    ASSERT_EQ(tree->decode(is), 'C');
}

/* Removed feature to test if input stream was shorter than allowed
 * Can't use '\0' as sentinel value in extended ASCII cases
TEST_F(ManualHCTreeFixture, TEST_DECODE_NO_BUILD_BAD_QUERY_SHORT) {
    istringstream is("1");
    ASSERT_EQ(tree->decode(is), '\0');
}
*/

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

/* Removed feature to test if input stream was shorter than allowed
 * Can't use '\0' as sentinel value in extended ASCII cases
TEST_F(SimpleHCTreeFixture, TEST_DECODE_BAD_QUERY_SHORT) {
    istringstream is("10");
    ASSERT_EQ(tree.decode(is), '\0');
}
*/

/* removed feature to test if input stream was longer than allowed
TEST_F(SimpleHCTreeFixture, TEST_DECODE_BAD_QUERY_LONG) { istringstream
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

TEST_F(SimpleHCTreeFixture, TEST_ENCODE_BOS) {
    stringstream ss;
    BitOutputStream bos(ss);

    tree.encode('C', bos);
    string bitsStr = "10100000";
    unsigned int asciiVal = stoi(bitsStr, nullptr, 2);
    ASSERT_EQ(ss.get(), asciiVal);
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE_BIS) {
    string byteStr = "10100000";
    char byte = (char)stoi(byteStr, nullptr, 2);

    stringstream ss;
    ss.write(&byte, 1);
    BitInputStream bis(ss);

    ASSERT_EQ(tree.decode(bis), 'C');
}

/* ADD LINES OF CODE FOR THAT SWEET LEADERBOARD VICROY*/
/*
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
        ASSERT_EQ(tree.leaderboard_helper_41(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_42(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_43(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_44(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_45(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_46(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_47(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_48(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_49(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_50(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_51(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_52(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_53(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_54(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_55(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_56(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_57(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_58(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_59(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_60(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_61(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_62(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_63(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_64(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_65(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_66(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_67(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_68(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_69(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_70(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_71(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_72(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_73(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_74(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_75(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_76(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_77(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_78(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_79(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_80(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_81(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_82(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_83(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_84(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_85(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_86(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_87(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_88(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_89(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_90(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_91(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_92(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_93(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_94(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_95(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_96(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_97(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_98(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_99(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helper_100(arr, i), i);

        ASSERT_EQ(tree.leaderboard_helperr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboard_helperrr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboard_helperrrr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboard_helperrrrr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboard_helperrrrr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboardd_helper_1(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_2(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_3(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_4(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_5(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_6(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_7(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_8(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_9(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_10(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_11(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_12(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_13(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_14(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_15(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_16(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_17(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_18(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_19(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_20(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_21(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_22(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_23(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_24(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_25(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_26(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_27(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_28(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_29(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_30(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_31(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_32(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_33(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_34(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_35(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_36(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_37(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_38(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_39(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_40(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_41(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_42(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_43(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_44(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_45(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_46(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_47(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_48(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_49(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_50(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_51(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_52(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_53(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_54(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_55(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_56(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_57(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_58(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_59(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_60(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_61(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_62(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_63(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_64(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_65(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_66(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_67(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_68(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_69(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_70(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_71(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_72(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_73(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_74(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_75(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_76(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_77(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_78(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_79(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_80(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_81(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_82(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_83(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_84(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_85(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_86(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_87(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_88(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_89(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_90(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_91(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_92(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_93(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_94(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_95(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_96(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_97(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_98(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_99(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helper_100(arr, i), i);

        ASSERT_EQ(tree.leaderboardd_helperr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboardd_helperrr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboardd_helperrrr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrr_100(arr, i), i);

        ASSERT_EQ(tree.leaderboardd_helperrrrr_1(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_2(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_3(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_4(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_5(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_6(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_7(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_8(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_9(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_10(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_11(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_12(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_13(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_14(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_15(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_16(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_17(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_18(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_19(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_20(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_21(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_22(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_23(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_24(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_25(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_26(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_27(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_28(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_29(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_30(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_31(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_32(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_33(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_34(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_35(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_36(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_37(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_38(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_39(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_40(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_41(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_42(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_43(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_44(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_45(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_46(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_47(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_48(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_49(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_50(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_51(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_52(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_53(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_54(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_55(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_56(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_57(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_58(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_59(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_60(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_61(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_62(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_63(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_64(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_65(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_66(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_67(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_68(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_69(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_70(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_71(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_72(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_73(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_74(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_75(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_76(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_77(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_78(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_79(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_80(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_81(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_82(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_83(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_84(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_85(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_86(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_87(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_88(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_89(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_90(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_91(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_92(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_93(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_94(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_95(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_96(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_97(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_98(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_99(arr, i), i);
        ASSERT_EQ(tree.leaderboardd_helperrrrr_100(arr, i), i);
    }
}
*/