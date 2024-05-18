#include "stream_reassembler.hh"
#include <iterator>
#include <map>
#include <limits>
// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
 :  stream{}, 
    curr_index(0),
    overflow_index(0),
    _capacity(capacity), 
    _output(capacity),
    eof_index(std::numeric_limits<streamsize>::max()) {}



//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    overflow_index = _capacity + curr_index - _output.buffer_size();
    for (size_t it = 0; it < data.size() && (it + index) < overflow_index; ++it) {
        stream[index + it] = data[it];
    }

    if (eof) {
        eof_index = index + data.size();
    }

    for (size_t idx = curr_index; idx < eof_index; ++idx) {
        auto it = stream.find(idx);
        if (it != stream.end()) {
            _output.write(string(1, stream[curr_index]));
            stream.erase(it);
            ++curr_index;
        } else {
            break;
        }
    }

    if (curr_index >= eof_index) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { 
    return stream.size();
}

bool StreamReassembler::empty() const {  return stream.empty(); }

size_t StreamReassembler::ack_index() const { 
   return curr_index;
}