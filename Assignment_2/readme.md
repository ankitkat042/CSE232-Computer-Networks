# CN Assignment 2 
> Ankit Kumar 2021015

---
## 1. ByteStream

- The ByteStream class is initialized with a specific capacity, which determines the maximum number of bytes it can hold at any given time. This is set in the constructor.
- The class maintains counters for bytes written (`bytes2write`) and bytes read (`bytes2read`), both initialized to zero. Additionally, a flag (`inputended`) is used to track if the input has ended.

`write` method:
- `write` method allows data to be written into the byte stream. The number of bytes to be written is determined based on the remaining capacity of the stream. The data is then appended to an internal buffer, and the counter for bytes written is updated.

## Reading from the Stream:
* The class provides multiple methods to facilitate reading:
    + `peek_output`: Allows users to view data from the output side without removing it. This is useful for previewing data.
    + `read`: This method combines the actions of peeking and popping. It retrieves the data and then removes it from the buffer.
    + `pop_output`: Directly removes bytes from the output side of the buffer.

## Stream Management:
- `end_input`: Marks the end of input, ensuring no more bytes can be written to the stream.
- `input_ended`: Checks if the input process has concluded.
- `eof`: Determines if the stream has reached its end, which is true when the input has ended and the buffer is empty.
- `buffer_empty`: Checks if the buffer is currently empty.
- `remaining_capacity`: Calculates the available space left in the stream, which can be utilized for writing more bytes.

Result
![image](https://github.com/ankitkat042/DES102-Assignments/assets/79627254/b3e3accb-eeaf-4d10-9d34-8bccae7f31e5)

--- 

## 2. Building a reassembler

The StreamReassembler class is designed to handle the reassembly of byte streams that are received in segments, possibly out of order. 

- The StreamReassembler class is initialized with a specific capacity, which determines the maximum number of bytes it can hold at any given time.
- It maintains a map called stream to store the received segments, indexed by their starting position in the original data.
- The curr_index keeps track of the next expected byte in the stream, while overflow_index determines the boundary beyond which bytes should be discarded.
- The _output member represents the byte stream where the reassembled data will be written.

`push_substring` method:

+ Calculates `overflow_index` to set the discard boundary.
+ Stores incoming data in `stream` up to `overflow_index`.
+ If the segment signals the end (`eof` is true), `eof_index` marks the original data's end.
+ Attempts to reassemble contiguous segments, writing to `_output` and removing from stream.
+ If all bytes up to `eof_index` are reassembled, `_output` is marked complete.

In the end:
- `unassembled_bytes` returns bytes yet to be reassembled.
- `empty` checks if any unassembled bytes remain.
- `ack_index` gives the index of the last successfully reassembled byte

--- 

## 3. TCP Receiver

![image](https://github.com/ankitkat042/DES102-Assignments/assets/79627254/9bf28c19-beef-4097-9628-fa625006a7cd)

The TCP Receiver's primary role is to accept `TCPSegments`, reassemble them in the correct order, and manage the flow of incoming data. 

### TCPReceiver Class:

+ `segment_received` Method: This method processes incoming TCPSegments.

    - If the SYN flag hasn't been received yet, the method waits for it. Once received, the Initial Sequence Number (ISN) is set.
    - The method calculates the absolute sequence number, which represents each byte's place in the stream.
    - The reassembler (`_reassembler`) is then called to push the segment's payload into the byte stream in the correct order.
    - If the FIN flag is set, it marks the end of the data.

+ `ackno` Method: Returns the acknowledgment number (ackno). This is the index of the "first unassembled" byte. If all segments have been received and reassembled, the ackno is incremented.

+ `window_size` Method: Returns the available capacity in the output ByteStream, known as the "window size". This allows the receiver to control the flow of incoming data.

WrappingInt32 Utility:

wrap Function: Transforms a 64-bit sequence number into a WrappingInt32. This is essential because TCP headers use 32-bit sequence numbers to save space, but internally, the system might use 64-bit numbers for better management.

unwrap Function: Converts a WrappingInt32 back into a 64-bit sequence number. This function ensures that the sequence number is consistent with a given checkpoint, adjusting for potential wrap-around.

### WrappingInt32 Utility:

- wrap `Function`: Transforms a 64-bit sequence number into a `WrappingInt32`. This is essential because TCP headers use 32-bit sequence numbers to save space, but internally, the system might use 64-bit numbers for better management.

- `unwrap` Function: Converts a `WrappingInt32` back into a 64-bit sequence number. This function ensures that the sequence number is consistent with a given checkpoint, adjusting for potential wrap-around.


## Final Result

![image](https://github.com/community/community/assets/79627254/4ac86e00-7fd0-498f-a9f8-cb94f4526b20)
