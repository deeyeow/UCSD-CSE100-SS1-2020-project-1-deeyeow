#include "HCTree.hpp"

/* TODO: Delete all objects on the heap to avoid memory leaks. */
HCTree::~HCTree() {
    deleteHCNode(root);
    delete leaves;
}

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
        if (freqs[i] != 0) pq.push(new HCNode(freqs[i], (unsigned char)i));
    }

    // account for when freqs vector is all 0s
    if (pq.empty()) return;

    // account for when freqs vector only has 1 entry (can't set root to have a
    // value)
    if (pq.size() == 1) {
        HCNode* node = pq.top();
        root = new HCNode(node->count, node->symbol);

        root->c0 = node;
        root->c1 = nullptr;

        leaves->at(node->symbol) = node;
        node->p = root;

        return;
    }

    // organize priority queue using huffman encoding, until only one element
    // left
    while (pq.size() > 1) {
        // if more than one element in priority queue, need to keep popping,
        // combining, and pushing back
        HCNode* smaller = pq.top();
        pq.pop();
        HCNode* larger = pq.top();
        pq.pop();

        // if popped node is a leaf, add to leaves vector
        if (smaller->c0 == nullptr && smaller->c1 == nullptr)
            leaves->at(smaller->symbol) = smaller;
        // leaves[smaller->symbol] = smaller;
        if (larger->c0 == nullptr && larger->c1 == nullptr)
            leaves->at(larger->symbol) = larger;
        // leaves[larger->symbol] = larger;

        // combined node has count of c0 + c1, and symbol of c0
        HCNode* combined =
            new HCNode(smaller->count + larger->count, smaller->symbol);

        // set children
        combined->c0 = smaller;
        combined->c1 = larger;

        // set parent
        smaller->p = combined;
        larger->p = combined;

        // push back into priority queue
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

// struct to find element in a vector of pointers
struct getSymbol {
    unsigned char symbol;

    getSymbol(unsigned char _symbol) : symbol(_symbol) {}

    bool operator()(const HCNode* node) const {
        if (node == nullptr) return false;

        return node->symbol == symbol;
    }
};

void HCTree::encode(byte symbol, BitOutputStream& out) const {
    // get leaf, then traverse up tree until hit root, adding '0' or
    // '1' to a stack depending if left/right child
    HCNode* prev = leaves->at(symbol);
    HCNode* curr = prev->p;
    stack<unsigned int> stack;

    while (curr != nullptr) {
        if (curr->c0 == prev)
            stack.push(0);
        else
            stack.push(1);

        prev = curr;
        curr = curr->p;
    }

    // pop all from stack to reveal encoded letter in correct order
    // push results to outstream
    while (!stack.empty()) {
        out.writeBit(stack.top());
        stack.pop();
        // out.printBuf();
    }
}

/**
 * TODO: Write the encoding bits of the given symbol to ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must have been called
 * beforehand to create the HCTree.
 */

void HCTree::encode(byte symbol, ostream& out) const {
    /*
    // huffman tree should have been built by now, as well as leaves vector
    // check if symbol exists (is a leaf/in leaves vector)
    if (find_if(leaves->begin(), leaves->end(), getSymbol(symbol)) ==
        leaves->end())
        return;
    */

    // get leaf, then traverse up tree until hit root, adding '0' or
    // '1' to a stack depending if left/right child
    HCNode* prev = leaves->at(symbol);
    HCNode* curr = prev->p;
    stack<unsigned char> stack;

    while (curr != nullptr) {
        if (curr->c0 == prev)
            stack.push('0');
        else
            stack.push('1');

        prev = curr;
        curr = curr->p;
    }

    // pop all from stack to reveal encoded letter in correct order
    // push results to outstream
    while (!stack.empty()) {
        out.write((char*)&stack.top(), 1);
        stack.pop();
    }
}

/**
 * TODO: Decode the sequence of bits (represented as a char of either '0' or
 * '1') from the istream to return the coded symbol. For this function to work,
 * build() must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(BitInputStream& in) const {
    // check if tree has at least 1 node
    // if (root == nullptr) return '\0';

    unsigned int i;
    HCNode* curr = root;

    // keep reading in from stream, until eof
    while (1) {
        // get next char from instream
        i = in.readBit();

        // if '0', traverse left
        if (i == 0) curr = curr->c0;
        // else, traverse right
        else
            curr = curr->c1;

        // if leaf node, found letter and return (stops at first leaf node
        // found)
        if (curr->c0 == nullptr && curr->c1 == nullptr) return curr->symbol;
    }
}

/**
 * TODO: Decode the sequence of bits (represented as char of either '0' or '1')
 * from istream to return the coded symbol. For this function to work, build()
 * must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(istream& in) const {
    // check if tree has at least 1 node
    // if (root == nullptr) return '\0';

    unsigned char c;
    HCNode* curr = root;

    // keep reading in from stream, until eof
    while (1) {
        // get next char from stream
        c = (unsigned char)in.get();

        // if '0', traverse left
        if (c == '0') curr = curr->c0;
        // else, traverse right
        else
            curr = curr->c1;

        // if leaf node, found letter and return (stops at first leaf node
        // found)
        if (curr->c0 == nullptr && curr->c1 == nullptr) return curr->symbol;
    }
}

/* HERE BE DRAGONS */

/*  Thought experiment:
    Is it better to call more methods, or create longer method definitions?
   Surely, there comes a point when the cost/benefit of each approach
   intersects. Where exactly is this threshold? Or is it always more optimal to
   adopt one strategy over the other?


    Cost (input) of modifying N (number of methods) and S (length of each
   method):
    = (Method Calls + 1 line of work) * (Method Length + N lines of work)
    = (N + 1) * (S + N)
    = (NS + N)

    Code coverage (output) of increasing N and S are equivalent, so:
   = We can assume both are an arbitrary number, let's say 1.

    Efficiency is defined as output divided by input, so:
   = 1/(NS+N)

    Indeed, graphing Z = 1/(XY+X), we see in the gradient that the partial
   derivative wrt X is larger than Y by a constant amount.

    So it is ALWAYS more efficient to add the number of methods (N) than it
   is to update each method's definition (S), by N amount. (integral of a
   constant is N)
*/
