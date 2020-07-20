/**
 * TODO: file header
 *
 * Author:
 */
#ifndef BITOUTPUTSTREAM_HPP
#define BITOUTPUTSTREAM_HPP

#include <iostream>

typedef unsigned char byte;

using namespace std;

/** TODO: class header */
class BitOutputStream {
  private:
    ostream& out;        // reference to the output stream to use
    byte buf;            // one byte buffer of bits
    unsigned int nbits;  // number of bits have been writen to buf

  public:
    // TODO: Initialize member variables.
    explicit BitOutputStream(ostream& os) : out(os){};

    void flush();

    void writeBit(unsigned int i);
};

#endif
