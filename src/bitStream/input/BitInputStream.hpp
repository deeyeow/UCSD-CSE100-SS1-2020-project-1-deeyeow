/**
 * TODO: file header
 *
 * Author: Darren Yau
 */
#ifndef BITINPUTSTREAM_HPP
#define BITINPUTSTREAM_HPP

#include <iostream>

typedef unsigned char byte;

using namespace std;

class BitInputStream {
  private:
    istream& in;         // reference to the input stream to use
    byte buf;            // one byte buffer of bits
    unsigned int nbits;  // number of bits that have been writen to buf

  public:
    // TODO: Initialize member variables.
    explicit BitInputStream(istream& is) : in(is) {
        buf = in.get();
        nbits = 0;
    };

    void fill();

    unsigned int readBit();

    void printBuffer();
};

#endif