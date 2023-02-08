#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity) {}

uint64_t TCPSender::bytes_in_flight() const { return _next_seqno - _outgoing; }

void TCPSender::fill_window() {
    TCPSegment segment{};
    
    // If the Sender is just created, send an SYN segment
    if (_next_seqno == 0) {
        segment.header().syn = true;
        segment.header().seqno = wrap(_next_seqno, _isn);
        _next_seqno += 1;
        _segments_out.push(segment);
        return;
    }

    size_t buffer_size = _stream.buffer_size();
    
    // If the buffer is not empty, we write new segment as long as possible.
    if (buffer_size != 0) {
        size_t size;

        size = min(buffer_size, TCPConfig::MAX_PAYLOAD_SIZE);
        size = min(size, size_t(_outgoing_max - _next_seqno));
        segment.header().seqno = wrap(_next_seqno, _isn);
        segment.payload() = Buffer(_stream.read(size));
        _next_seqno += size;
        // If the buffer is empty now, we might need to send a FIN.
        if (_next_seqno < _outgoing_max && _stream.input_ended() && _stream.buffer_size() == 0) {
            segment.header().fin = true;
            _next_seqno += 1;
        }
        _segments_out.push(segment);
    }

    // If the buffer is empty, we might need to send a FIN.
    if (_next_seqno < _outgoing_max && _stream.input_ended() && _stream.buffer_size() == 0) {
        segment.header().fin = true;
        _next_seqno += 1;
        _segments_out.push(segment);
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t _ackno = unwrap(ackno, _isn, _next_seqno);
    if (_ackno <= _outgoing) return;
    if (_ackno > _next_seqno) return;

    _outgoing = _ackno;
    _outgoing_max = _next_seqno + window_size;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) { DUMMY_CODE(ms_since_last_tick); }

unsigned int TCPSender::consecutive_retransmissions() const { return {}; }

void TCPSender::send_empty_segment() {}
