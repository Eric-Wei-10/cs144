#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) :
    _output(capacity), _capacity(capacity), _idx_str(), _next(0), 
    _eof(false), _last_byte(0xffffffff) {}

size_t StreamReassembler::total_size() {
    return unassembled_bytes() + _output.buffer_size();
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (data.length() == 0 && !eof) return;

    if (eof) {
        _eof = eof;
        _last_byte = index + data.length();
    }
    
    map<size_t, string>::iterator it;

    it = _idx_str.find(index);
    if (it == _idx_str.end()) {
        _idx_str[index] = data;
    } else {
        if (it->second.length() < data.length()) {
            _idx_str[index] = data;
        }
    }

    it = _idx_str.begin();
    for (; it != _idx_str.end(); it++) {
        if (it->first < _next) {
            size_t item_length = it->second.length();
            if (it->first + item_length <= _next) {
                continue;
            } else {
                _next += _output.write(it->second.substr(_next - it->first));
            }
        } else if (it->first == _next) {
            _next += _output.write(it->second);
        } else {
            break;
        }
    }
    _idx_str.erase(_idx_str.begin(), it);

    if (_eof && _next == _last_byte) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t next = _next;
    size_t unassembled = 0;
    for (auto it: _idx_str) {
        if (it.first < next) {
            if (it.first + it.second.length() <= next) {
                continue;
            } else {
                unassembled += (it.first + it.second.length() - next);
                next = it.first + it.second.length();
            }
        } else {
            unassembled += it.second.length();
            next = it.first + it.second.length();
        }
    }
    return unassembled; 
}

bool StreamReassembler::empty() const { 
    return _output.buffer_empty() && unassembled_bytes() == 0; 
}
