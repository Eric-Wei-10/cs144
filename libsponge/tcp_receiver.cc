#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    TCPHeader header = seg.header();
    string data = seg.payload().copy();
    size_t str_idx;

    if (!header.syn && !syn_recv) return;

    if (header.syn && !syn_recv) {
        syn_recv = true;
        isn = header.seqno;
        _reassembler.push_substring(data, 0, header.fin);
        // Special case
        if (header.fin) {
            fin_recv = true;
        }
        return;
    }

    str_idx = unwrap(header.seqno, isn, _reassembler.next()) - syn_recv;
    _reassembler.push_substring(data, str_idx, header.fin);

    if (header.fin) {
        fin_recv = true;
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!syn_recv) {
        return nullopt;
    } else {
        return wrap(syn_recv + _reassembler.next() + (fin_recv && _reassembler.unassembled_bytes() == 0), isn);
    }
}

size_t TCPReceiver::window_size() const {
    return _capacity - _reassembler.stream_out().buffer_size();
}
