#ifndef HCTREE_HPP
#define HCTREE_HPP

#include <algorithm>
#include <fstream>
#include <queue>
#include <stack>
#include <vector>
// #include "BitInputStream.hpp"
// #include "BitOutputStream.hpp"
#include "HCNode.hpp"

using namespace std;

class HCTree {
  private:
    HCNode* root;             // the root of HCTree
    vector<HCNode*>* leaves;  // a vector storing pointers to all leaf HCNodes

  public:
    /* TODO: Initializes a new empty HCTree.*/
    HCTree() {
        root = nullptr;
        leaves = new vector<HCNode*>(256);
    }

    HCTree(HCNode* _root) { root = _root; }

    ~HCTree();

    // helper method for destructor
    void static deleteHCNode(HCNode* curr);

    void build(const vector<unsigned int>& freqs);

    // void encode(byte symbol, BitOutputStream& out) const;

    void encode(byte symbol, ostream& out) const;

    // byte decode(BitInputStream& in) const;

    byte decode(istream& in) const;

    /* WHY DOES THIS GUY CARE SO MUCH ABOUT A STUPID LEADERBOARD ? */
    int leaderboard_helper_1(vector<int>& arr, int i);
    int leaderboard_helper_2(vector<int>& arr, int i);
    int leaderboard_helper_3(vector<int>& arr, int i);
    int leaderboard_helper_4(vector<int>& arr, int i);
    int leaderboard_helper_5(vector<int>& arr, int i);
    int leaderboard_helper_6(vector<int>& arr, int i);
    int leaderboard_helper_7(vector<int>& arr, int i);
    int leaderboard_helper_8(vector<int>& arr, int i);
    int leaderboard_helper_9(vector<int>& arr, int i);
    int leaderboard_helper_10(vector<int>& arr, int i);
    int leaderboard_helper_11(vector<int>& arr, int i);
    int leaderboard_helper_12(vector<int>& arr, int i);
    int leaderboard_helper_13(vector<int>& arr, int i);
    int leaderboard_helper_14(vector<int>& arr, int i);
    int leaderboard_helper_15(vector<int>& arr, int i);
    int leaderboard_helper_16(vector<int>& arr, int i);
    int leaderboard_helper_17(vector<int>& arr, int i);
    int leaderboard_helper_18(vector<int>& arr, int i);
    int leaderboard_helper_19(vector<int>& arr, int i);
    int leaderboard_helper_20(vector<int>& arr, int i);
    int leaderboard_helper_21(vector<int>& arr, int i);
    int leaderboard_helper_22(vector<int>& arr, int i);
    int leaderboard_helper_23(vector<int>& arr, int i);
    int leaderboard_helper_24(vector<int>& arr, int i);
    int leaderboard_helper_25(vector<int>& arr, int i);
    int leaderboard_helper_26(vector<int>& arr, int i);
    int leaderboard_helper_27(vector<int>& arr, int i);
    int leaderboard_helper_28(vector<int>& arr, int i);
    int leaderboard_helper_29(vector<int>& arr, int i);
    int leaderboard_helper_30(vector<int>& arr, int i);
    int leaderboard_helper_31(vector<int>& arr, int i);
    int leaderboard_helper_32(vector<int>& arr, int i);
    int leaderboard_helper_33(vector<int>& arr, int i);
    int leaderboard_helper_34(vector<int>& arr, int i);
    int leaderboard_helper_35(vector<int>& arr, int i);
    int leaderboard_helper_36(vector<int>& arr, int i);
    int leaderboard_helper_37(vector<int>& arr, int i);
    int leaderboard_helper_38(vector<int>& arr, int i);
    int leaderboard_helper_39(vector<int>& arr, int i);
    int leaderboard_helper_40(vector<int>& arr, int i);
};

#endif  // HCTREE_HPP
