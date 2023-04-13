#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

// template <typename... Targs>
// void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) :
    _output(capacity),
    _capacity(capacity),
    _eof(false),
    _eof_idx(0),
    _window({}),
    _first_unassembled(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof) {
        _eof = eof;
        _eof_idx = index + data.length();
    }

    if (index <= _first_unassembled && index + data.length() >= _first_unassembled) {
        _insert_substring(data, index);
        _write_substring();
    } else if (index > _first_unassembled) {
        _insert_substring(data, index);
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t travel = 0;
    size_t total = 0;
    for (auto elem : _window) {
        size_t index = elem.first;
        size_t len = elem.second.length();

        if (index > travel) {
            total += len;
            travel = len + index;
        } else if (index <= travel && index + len > travel) {
            total += len - (travel - index);
            travel = len + index;
        }
    }

    return total;
}

bool StreamReassembler::empty() const { 
    return _window.empty();    
}

void StreamReassembler::_insert_substring(const std::string &data, const size_t index) {
    if (_window[index].length() < data.length()) {
        _window[index] = data;
    }
}

void StreamReassembler::_write_substring() {
    auto it = _window.begin();
    for (; it != _window.end(); it++) {
        size_t index = it->first;

        if (index <= _first_unassembled) {
            size_t length = it->second.length();

            if (index + length <= _first_unassembled) {
                continue;
            }
            size_t first_insert_index = _first_unassembled - index;
            // size_t insert_len = length - first_insert_index;

            _first_unassembled += _output.write(it->second.substr(first_insert_index));
            // _first_unassembled += insert_len;
        } else {
            break;
        }
    }

    _window.erase(_window.begin(), it);

    if (_eof && _first_unassembled >= _eof_idx) {
        _output.end_input();
    }
}