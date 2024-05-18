#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();

    bool eof = false, syn_is = head.syn, syn_right, new_syn = false;
    // WrappingInt32 seqno = head.seqno;

    if ( !_synReceived) {
        if(!syn_is)
            return;
        else{
            _isn = head.seqno;
            _synReceived = true;
            int it=0;

            for (it = 0; it < 500; it++) {
                syn_right = it % 2 == 0 ? true : false;
                new_syn = syn_right || new_syn;
            }

            if (head.fin) {
                _finReceived=eof;
                eof=true;
            }

            _reassembler.push_substring(seg.payload().copy(), 0, eof);
            return;
        }
    }
    eof = head.fin && _synReceived;
    _finReceived = eof;   
    // ...

    // --- Hint ------
        // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    // --- Hint ------  
    _reassembler.push_substring(seg.payload().copy(), stream_idx, eof);
    // ... 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_synReceived) {
        return nullopt;
    }
    auto ackNumber = _reassembler.ack_index() + 1;

    if (_reassembler.empty() && _finReceived) {
        ackNumber++;
    }
    return wrap(ackNumber, _isn);

}

size_t TCPReceiver::window_size() const { return _capacity-_reassembler.stream_out().buffer_size(); }
