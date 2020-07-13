#include "BitOutputStream.hpp"

/**
 * TODO: Write the buffer to the output stream,
 * and then clear the buffer to allow further use.
 * You may use std::fill() to zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation that
 * may cause a timeout.
 */
void BitOutputStream::flush() {}

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
void BitOutputStream::writeBit(unsigned int i) {}
