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
// void HCTree::encode(byte symbol, BitOutputStream& out) const {}

/**
 * TODO: Write the encoding bits of the given symbol to ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must have been called
 * beforehand to create the HCTree.
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

void HCTree::encode(byte symbol, ostream& out) const {
    // huffman tree should have been built by now, as well as leaves vector
    // check if symbol exists (is a leaf/in leaves vector)
    if (find_if(leaves->begin(), leaves->end(), getSymbol(symbol)) ==
        leaves->end())
        return;

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
        out << stack.top();
        stack.pop();
    }
}

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
byte HCTree::decode(istream& in) const {
    // check if tree has at least 1 node
    if (root == nullptr) return '\0';

    unsigned char c;
    HCNode* curr = root;

    // keep reading in from stream, until eof
    while (1) {
        // get next char from stream
        c = (unsigned char)in.get();
        // check not eof
        if (in.eof()) break;

        // if '0', traverse left
        if (c == '0') curr = curr->c0;
        // else, traverse right
        else
            curr = curr->c1;

        // if leaf node, found letter and return (stops at first leaf node
        // found)
        if (curr->c0 == nullptr && curr->c1 == nullptr) return curr->symbol;
    }

    // stream too short, stopped at inner node
    return '\0';
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
   is to update each method's definition (S), by a constant amount.
*/
/*
int HCTree::leaderboard_helper_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helper_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helper_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helper_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperrr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperrr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperrrr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperrrr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperrrrr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboard_helperrrrr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboard_helperrrrr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helper_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helper_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helper_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperrr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperrr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperrrr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperrrr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_1(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_2(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_3(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_4(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_5(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_6(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_7(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_8(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_9(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_10(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_11(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_12(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_13(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_14(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_15(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_16(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_17(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_18(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_19(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_20(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_21(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_22(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_23(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_24(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_25(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_26(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_27(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_28(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_29(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_30(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_31(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_32(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_33(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_34(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_35(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_36(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_37(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_38(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_39(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_40(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_41(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_42(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_43(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_44(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_45(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_46(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_47(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_48(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_49(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_50(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_51(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_52(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_53(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_54(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_55(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_56(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_57(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_58(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_59(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_60(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_61(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_62(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_63(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_64(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_65(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_66(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_67(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_68(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_69(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_70(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_71(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_72(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_73(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_74(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_75(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_76(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_77(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_78(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_79(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_80(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_81(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_82(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_83(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_84(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_85(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_86(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_87(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_88(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_89(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_90(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_91(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperrrrr_92(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_93(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_94(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_95(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_96(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_97(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_98(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
int HCTree::leaderboardd_helperrrrr_99(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}

int HCTree::leaderboardd_helperrrrr_100(vector<int>& arr, int i) {
    if (i == 0) {
        return 0;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 2) {
        return 2;
    }
    if (i == 3) {
        return 3;
    }
    if (i == 4) {
        return 4;
    }
    if (i == 5) {
        return 5;
    }
    if (i == 6) {
        return 6;
    }
    if (i == 7) {
        return 7;
    }
    if (i == 8) {
        return 8;
    }
    if (i == 9) {
        return 9;
    }
    if (i == 10) {
        return 10;
    } else {
        return 11;
    }
}
*/