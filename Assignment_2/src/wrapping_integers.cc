#include "wrapping_integers.hh"

#include <iostream>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number

WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { 

    uint32_t sequence = n & 0xFFFFFFFF; 
    sequence += isn.raw_value();
    sequence %= 0x100000000;
    return WrappingInt32(sequence);

}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    int32_t diff = n.raw_value() - wrap(checkpoint, isn).raw_value();
    int64_t refresh = checkpoint + diff;
    if (diff > 0 && refresh < checkpoint) {
        refresh += 0x100000000;
    } else if (diff < 0 && refresh < 0) {
        refresh += 0x100000000;
    }
    return static_cast<uint64_t>(refresh);
}
