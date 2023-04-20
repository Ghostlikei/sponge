#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // DUMMY_CODE(seg);

    /**
     * @todo What I need to do here is:
     * 1. if segment includes SYN, it will set the ISN, which I can utilitize to 
     * calculate the absolute index of a segment payload (It can be only set once)
     * 2. If segment includes FIN, it indicates the `eof` of the all incoming segments
     * samely, I need to store this infomation and use it.
     * 3. If segments does not include these two elements, I have to 
     */
    if (seg.header().syn && !_isn.has_value()) {
        _isn = seg.header().seqno;
    }

    if (_isn.has_value()) {
        // If isn was set, payload should be put with index = unwarp(seqno)
        _reassembler.push_substring(
            seg.payload().copy(),
            unwrap(seg.header().seqno, _isn.value(), _reassembler.first_unassembled_index()),
            seg.header().fin
        );
        _ackno = wrap(_reassembler.first_unassembled_index(), _isn.value());
    }

    _fin = _fin ? _fin : seg.header().fin;
    if (!_syn && seg.header().syn && _ackno.has_value()) {
        _ackno = WrappingInt32(_ackno.value().raw_value() + 1);
        _isn = _ackno;
        _syn = true;
    }

    if (_fin && _isn.has_value() && _reassembler.stream_out().input_ended()) {
        _ackno = WrappingInt32(_ackno.value().raw_value() + 1);
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const { return _ackno; }

size_t TCPReceiver::window_size() const { return _reassembler.stream_out().remaining_capacity(); }
