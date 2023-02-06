#include "wrapping_integers.hh"
#define int32_mask 0xffffffff

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return isn + n;
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
    uint64_t _n = n.raw_value();
    uint64_t _isn = isn.raw_value();
    uint64_t first32 = checkpoint & 0xffffffff00000000;
    uint64_t last32;
    uint64_t res;
    uint64_t aux = 0x0000000100000000; // 1 << 32
    if (int64_t(_n) - int64_t(_isn) >= 0) {
        last32 = uint64_t(_n - _isn);
    } else {
        last32 = uint64_t((_n + aux) - _isn);
    }
    if (last32 > checkpoint) {
        return last32;
    } else {
        res = first32 | last32;
        if (res == checkpoint) {
            return res;
        } else if (res < checkpoint) {
            return ((res + aux) - checkpoint) > (checkpoint - res)? res: (res + aux);
        } else {
            return (res - checkpoint) > (checkpoint - (res - aux))? (res - aux): res;
        }
    }
}
