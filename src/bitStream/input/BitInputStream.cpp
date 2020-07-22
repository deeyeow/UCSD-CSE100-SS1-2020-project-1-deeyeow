#include "BitInputStream.hpp"

// TODO: Fill the buffer from the input stream.
void BitInputStream::fill() {
    buf = in.get();
    nbits = 0;
}

// TODO: Read the next bit from the buffer.
unsigned int BitInputStream::readBit() {
    // fill buffer if read all values
    if (nbits >= 8) fill();

    unsigned char temp = buf;
    temp = (buf >> (7 - nbits)) & 1;
    nbits++;

    return temp;
}
