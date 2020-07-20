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
};

#endif  // HCTREE_HPP
