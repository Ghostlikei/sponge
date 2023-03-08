#include "byte_stream.hh"
#include <iostream>
// # define BS_DEBUG
// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

// template <typename... Targs>
// void DUMMY_CODE(Targs &&... /* unused */) {}
using namespace std;
/**
 * @warning size == src.size()
*/
static inline void write_helper(std::vector<char>& target,
                                const std::string& src,
                                size_t vec_start_index,
                                size_t str_start_index,
                                size_t size)
{
    for(size_t i = 0; i < size; i++) {
        target[vec_start_index + i] = src[str_start_index + i];
    }
    return;
}

static inline void mstrcpy(std::string& target,
                            const std::vector<char>& src,
                            size_t str_start_index,
                            size_t size)
{
    for (size_t i = 0; i < size; i++) {
        target += src[str_start_index+i];
    }
}

ByteStream::ByteStream(const size_t capacity) : 
    _capacity(capacity),
    _size(0),
    _buffer({}),
    _write_end(0),
    _read_end(0),
    _input(true) {
    _buffer.resize(capacity);
}

size_t ByteStream::write(const string &data) {
    if (_size == _capacity) {
        return 0;
    }
    size_t string_size = data.size();
    // judge whether truncate string input
    size_t remaining = _capacity - _size;
    // True if truncated, false at normal case
    bool truncate_flag = string_size > remaining;
    // Preparation
    size_t written = 0;
    size_t write_index = _write_end % _capacity;
    // Case3: chase write
    if ((write_index) < (_read_end % _capacity)) {
        written = string_size - truncate_flag * (string_size - remaining);

        write_helper(_buffer, data, write_index, 0, written);

        _size += written;
        _write_end += written;
#ifdef BS_DEBUG
        cout << "written: " << written <<endl;
        assert(written > 0);
#endif
        return written;
    } else if (_capacity - write_index >= string_size) {
        // Case1: normal write
        written = string_size;
        write_helper(_buffer, data, write_index, 0, written);
        _size += written;
        _write_end += written;
#ifdef BS_DEBUG
        cout << "written: " << written <<endl;
        assert(written > 0);
#endif
        return written;
    } else {
        // Case2: cross edge write
        // fill we to edge segment first
        written = _capacity - write_index;
        write_helper(_buffer, data, write_index, 0, written);
        _size += written;
        _write_end += written;
        // goto case 3
        assert(_write_end % _capacity == 0);
        size_t chase = string_size - written 
                    - truncate_flag * (string_size - remaining);
        write_helper(_buffer, data, 0, written, chase);
        _size += chase;
        _write_end += chase;
#ifdef BS_DEBUG
        cout << "written + chase: " << written + chase<<endl;
        assert(written > 0);
#endif
        return written + chase;
    }
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string ret;
    if (eof()) return ret;
    size_t read_index = _read_end % _capacity;
    size_t write_index = _write_end % _capacity;

    size_t consumed = 0;
    if (read_index < write_index) {
        // Case1: read_index < write_index (normal peek)
        consumed = len > _size ? _size : len;
        mstrcpy(ret, _buffer, read_index, consumed);
        return ret;
    } else if (_capacity - read_index >= len) {
        // Case3: right peek
        consumed = len;
        mstrcpy(ret, _buffer, read_index, consumed);
        return ret;
    } else {
        // Case2: cross-edge peek;
        consumed = _capacity - read_index;
        mstrcpy(ret, _buffer, read_index, consumed);
        // Goto case1
        size_t cross = len - consumed >= _size ? _size : len - consumed;
        mstrcpy(ret, _buffer, 0, cross);
        return ret;
    }
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (_write_end == _read_end) return;
    size_t read_index = _read_end % _capacity;
    size_t write_index = _write_end % _capacity;

    size_t consumed = 0;
    if (read_index < write_index) {
        // Case1: read_index < write_index (normal read)
        consumed = len > _size ? _size : len;
        _read_end += consumed;
        _size -= consumed;
        return;
    } else if (_capacity - read_index > len) {
        // Case3: right read
        consumed = len;
        _read_end += len;
        _size -= len;
        return;
    } else {
        // Case2: cross-edge read;
        consumed = _capacity - read_index;
        _read_end += consumed;
        _size -= consumed;
        // Goto case1
        size_t cross = len - consumed >= _size ? _size : len - consumed;
        _read_end += cross;
        _size -= cross;
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string ret = peek_output(len);
    pop_output(len);
    return ret;
}

void ByteStream::end_input() {
    _input = false;
}

bool ByteStream::input_ended() const { 
//     return (_size == _capacity) && ((_write_end % _capacity) == (_read_end % _capacity));
    return !_input;
}

size_t ByteStream::buffer_size() const {
    return _size;
}

bool ByteStream::buffer_empty() const {
    return _size == 0;
}

bool ByteStream::eof() const {
    return (_write_end == _read_end) && input_ended();
}

size_t ByteStream::bytes_written() const {
    return _write_end;
}

size_t ByteStream::bytes_read() const {
    return _read_end;
}

size_t ByteStream::remaining_capacity() const { 
    return _capacity - _size;
}
