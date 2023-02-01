#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : buffer(capacity)
    , start(0)
    , end(0)
    , occupied(0)
    , _capacity(capacity)
    , _bytes_read(0)
    , _bytes_written(0)
    , _end_input(false)
    , _error(false) {}

size_t ByteStream::write(const string &data) {
    size_t written_cnt = 0;

    while (occupied < _capacity && written_cnt < data.length()) {
        buffer[end] = data[written_cnt++];
        end = (end + 1) % _capacity;
        _bytes_written++;
        occupied++;
    }

    return written_cnt;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string res;
    size_t cnt = 0;
    while (cnt < len && occupied > cnt) {
        res.append(1, buffer[(start + cnt) % _capacity]);
        cnt++;
    }
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t cnt = 0;
    while (cnt < len && occupied > 0) {
        start++;
        cnt++;
        _bytes_read++;
        occupied--;
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() { _end_input = true; }

bool ByteStream::input_ended() const { return _end_input; }

size_t ByteStream::buffer_size() const { return occupied; }

bool ByteStream::buffer_empty() const { return occupied == 0; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - occupied; }
