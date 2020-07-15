#include "HCTree.hpp"

/* TODO: Delete all objects on the heap to avoid memory leaks. */
HCTree::~HCTree() { deleteHCNode(root); }

void HCTree::deleteHCNode(HCNode* curr) {
    if (curr == nullptr) return;
    deleteHCNode(curr->c0);
    deleteHCNode(curr->c1);
    delete curr;
}

/**
 * TODO: Build the HCTree from the given frequency vector. You can assume the
 * vector must have size 256 and each value at index i represents the frequency
 * of char with ASCII value i. Only non-zero frequency symbols should be used to
 * build the tree. The leaves vector must be updated so that it can be used in
 * encode() to improve performance.
 *
 * When building the HCTree, you should use the following tie-breaking rules to
 * match the output from reference solution in checkpoint:
 *
 *    1. HCNode with lower count should have higher priority. If count is the
 * same, then HCNode with a larger ascii value symbol should have higher
 * priority. (This should be already defined properly in the comparator in
 * HCNode.hpp)
 *    2. When popping two highest priority nodes from PQ, the higher priority
 * node will be the ‘c0’ child of the new parent HCNode.
 *    3. The symbol of any parent node should be taken from its 'c0' child.
 */
void HCTree::build(const vector<unsigned int>& freqs) {
    // account for when freqs array is empty
    if (freqs.empty()) return;

    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;

    // iterate through freqs array
    for (int i = 0; i < 256; i++) {
        // if element has a frequency, push into priority queue
        if (freqs[i] != 0) pq.push(new HCNode(freqs[i], i));
    }

    // account for when freqs array is all 0s
    if (pq.empty()) return;

    // organize priority queue using huffman encoding, until only one element
    // left
    while (pq.size() > 1) {
        // if more than one element in priority queue, need to keep popping,
        // combining, and pushing back
        HCNode* smaller = pq.top();
        pq.pop();
        HCNode* larger = pq.top();
        pq.pop();

        // combined node has count of c0 + c1, and symbol of c0
        HCNode* combined =
            new HCNode(smaller->count + larger->count, smaller->symbol);
        combined->c0 = smaller;
        combined->c1 = larger;

        pq.push(combined);
    }

    // priority queue only has 1 element left, set to root
    root = pq.top();
}

/**
 * TODO: Write the encoding bits of the given symbol to the ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must be called before
 * to create the HCTree.
 */
// void HCTree::encode(byte symbol, BitOutputStream& out) const {}

/**
 * TODO: Write the encoding bits of the given symbol to ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must have been called
 * beforehand to create the HCTree.
 */
void HCTree::encode(byte symbol, ostream& out) const {}

/**
 * TODO: Decode the sequence of bits (represented as a char of either '0' or
 * '1') from the istream to return the coded symbol. For this function to work,
 * build() must have been called beforehand to create the HCTree.
 */
// byte HCTree::decode(BitInputStream& in) const { return ' '; }

/**
 * TODO: Decode the sequence of bits (represented as char of either '0' or '1')
 * from istream to return the coded symbol. For this function to work, build()
 * must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(istream& in) const { return ' '; }
