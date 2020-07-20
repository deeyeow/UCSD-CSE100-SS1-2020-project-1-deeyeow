/**
 * TODO: file header
 *
 * Author:
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
    explicit BitInputStream(istream& is) : in(is){};

    void fill();

    unsigned int readBit();
};

#endif