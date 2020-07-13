#ifndef HCNODE_HPP
#define HCNODE_HPP

#include <iostream>

typedef unsigned char byte;

using namespace std;

/** A class, instances of which are nodes in an HCTree.
 */
class HCNode {
  public:
    unsigned int count;  // the freqency of the symbol
    byte symbol;         // byte in the file we're keeping track of
    HCNode* c0;          // pointer to '0' child
    HCNode* c1;          // pointer to '1' child
    HCNode* p;           // pointer to parent

    /* Constructor that initialize a HCNode */
    HCNode(unsigned int count, byte symbol, HCNode* c0 = 0, HCNode* c1 = 0,
           HCNode* p = 0)
        : count(count), symbol(symbol), c0(c0), c1(c1), p(p) {}
};

/* For printing an HCNode to an ostream. Possibly useful for debugging */
ostream& operator<<(ostream&, const HCNode&) __attribute__((weak));
ostream& operator<<(ostream& stm, const HCNode& n) {
    stm << "[" << n.count << "," << (int)(n.symbol) << "]";
    return stm;
}

/* Comparator of HCNode pointer. In priority queue, HCNode ptr with lower count
 * has higher priority, and if count is the same, HCNode ptr with larger symbol
 * has higher prioruty.
 */
struct HCNodePtrComp {
    /* TODO */
    bool operator()(HCNode*& lhs, HCNode*& rhs) const { return false; }
};
#endif  // HCNODE_HPP
