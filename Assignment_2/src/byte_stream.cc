#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */


using namespace std;

ByteStream::ByteStream(const size_t capa)
{
    cap = capa;
    bytes2write = 0;
    bytes2read = 0;
    inputended = false;
}

size_t ByteStream::write(const string &data) {
    size_t writelength = min(data.size(), remaining_capacity());
    for (size_t i = 0; i<writelength; ++i) {
        buffer.push_back(data[i]);
    }
    bytes2write += writelength;
    return writelength;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string output;
    auto it = buffer.begin();
    for (size_t i = 0; i < len && it != buffer.end(); ++i, ++it) {
        output.push_back(*it);
    }
    return output;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > buffer.size()) {
        set_error();
        return;
    }
    for (size_t i = 0; i < len; ++i) {
        buffer.pop_front();
        bytes2read++;
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    if (len > buffer.size()) {
        set_error();
        return {};
    }
    string output = peek_output(len);
    pop_output(len);
    return output;
}

void ByteStream::end_input() {
  inputended = true;
}

bool ByteStream::input_ended() const { 
  return inputended;
}

size_t ByteStream::buffer_size() const {
   return buffer.size();
}

bool ByteStream::buffer_empty() const {
  return buffer.empty();
}

bool ByteStream::eof() const {
  return inputended && buffer_empty();
}

size_t ByteStream::bytes_written() const {
  return bytes2write;
}

size_t ByteStream::bytes_read() const { 
  return bytes2read;
}

size_t ByteStream::remaining_capacity() const {
  return cap - buffer.size();
}
