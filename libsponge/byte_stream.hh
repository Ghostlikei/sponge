#ifndef SPONGE_LIBSPONGE_BYTE_STREAM_HH
#define SPONGE_LIBSPONGE_BYTE_STREAM_HH

#define SPONGE_LIBSPONGE_BYTE_STREAM_DEBUG

#include <string>
#include <vector>
#include <assert.h>

#pragma once

/**
 * @brief Trying to design a "Circular buffer"
 * write-end, read_end can fully use the memory of buffer
 * Cause it is used within single-thread, we don't need to make
 * operations atomic.
*/

//! \brief An in-order byte stream.

//! Bytes are written on the "input" side and read from the "output"
//! side.  The byte stream is finite: the writer can end the input,
//! and then no more bytes can be written.
class ByteStream {
  private:
    // Your code here -- add private members as necessary.

    // Hint: This doesn't need to be a sophisticated data structure at
    // all, but if any of your tests are taking longer than a second,
    // that's a sign that you probably want to keep exploring
    // different approaches.

    /**
     * @brief length of buffer memory
    */
    size_t _capacity;
    /**
     * @brief record byte occupied, simplify implementation of interfaces 
    */
    size_t _size;
    /**
     * @note We need a "fixed length" buffer memory
     * Temporarily, use std::vector for "safe allocate"
    */ 
    std::vector<char> _buffer;
    /**
     * @note Maintain a write-end and read_end to manipulate write() and read()
     * This is enlightened by std::iterator
    */
    size_t _write_end;
    size_t _read_end;

    bool _input;

    bool _error{};  //!< Flag indicating that the stream suffered an error.

  public:
    //! Construct a stream with room for `capacity` bytes.
    ByteStream(const size_t capacity);

    //! \name "Input" interface for the writer
    //!@{

    /**
     * @return the number of bytes accepted into the stream
     * @param string data to be written
     * @note There would be 3 cases in circular buffer:
     * Case1: (normal write)
     * Case2: (cross-edge write)
     * Case3: (chase write)
    */
    size_t write(const std::string &data);

    //! \returns the number of additional bytes that the stream has space for
    size_t remaining_capacity() const;

    //! Signal that the byte stream has reached its ending
    /**
     * @note Not very clear what is this doing
    */
    void end_input();

    //! Indicate that the stream suffered an error.
    void set_error() { _error = true; }
    //!@}

    //! \name "Output" interface for the reader
    //!@{

    //! Peek at next "len" bytes of the stream
    //! \returns a string
    /**
     * @note peek() contains two senarios:
     * Case1: read_index < write_index (normal peek)
     * Case2: read_index >= write_index (cross-edge peek), eof excluded
     * Case3: right remaining > len (right peek)
    */
    std::string peek_output(const size_t len) const;

    //! Remove bytes from the buffer
    /**
     * @note pop() contains two senarios:
     * Case1: read_index < write_index (normal read)
     * Case2: read_index >= write_index (cross-edge read), eof excluded
     * Case3: right remaining > len (right read)
    */
    void pop_output(const size_t len);

    //! Read (i.e., copy and then pop) the next "len" bytes of the stream
    //! \returns a string
    std::string read(const size_t len);

    //! \returns `true` if the stream input has ended
    bool input_ended() const;

    //! \returns `true` if the stream has suffered an error
    bool error() const { return _error; }

    //! \returns the maximum amount that can currently be read from the stream
    size_t buffer_size() const;

    //! \returns `true` if the buffer is empty
    bool buffer_empty() const;

    //! \returns `true` if the output has reached the ending
    bool eof() const;
    //!@}

    //! \name General accounting
    //!@{

    //! Total number of bytes written
    size_t bytes_written() const;

    //! Total number of bytes popped
    size_t bytes_read() const;
    //!@}
};

#endif  // SPONGE_LIBSPONGE_BYTE_STREAM_HH
