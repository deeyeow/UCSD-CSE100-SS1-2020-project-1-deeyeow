#include "BitOutputStream.hpp"

/**
 * TODO: Write the buffer to the output stream,
 * and then clear the buffer to allow further use.
 * You may use std::fill() to zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation that
 * may cause a timeout.
 */
unsigned int BitOutputStream::flush() {
    out << buf;
    // record how many padded 0s
    unsigned int tempNBits = nbits;
    buf = 0;
    nbits = 0;
    return 8 - tempNBits;
}

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
void BitOutputStream::writeBit(unsigned int i) {
    // flush buffer if full
    if (nbits >= 8) flush();

    // shit LSB left, then 'or' to update buf
    buf |= i << (7 - nbits);
    nbits++;
}

void BitOutputStream::printBuf() {
    cout << "Printing bit buffer: ";
    for (int i = 0; i < 8; i++) {
        byte temp = buf;
        cout << ((temp >> (7 - i)) & 1);
    }
    cout << endl;
}
