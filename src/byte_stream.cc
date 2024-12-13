#include "byte_stream.hh"

#include <algorithm>
#include <stdexcept>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

Reader& ByteStream::reader() { return *static_cast<Reader*>( this ); }

const Reader& ByteStream::reader() const { return *static_cast<const Reader*>( this ); }

Writer& ByteStream::writer() { return *static_cast<Writer*>( this ); }

const Writer& ByteStream::writer() const { return *static_cast<const Writer*>( this ); }

bool Writer::is_closed() const { return closed_; }

void Writer::push( string data )
{
  if ( has_error() )
  {
    return;
  }
  uint64_t bytes_to_write = min( data.size(), available_capacity() );
  buffer_ += data.substr( 0, bytes_to_write );
  bytes_pushed_ += bytes_to_write;
}

void Writer::close() { closed_ = true; }

uint64_t Writer::available_capacity() const { return capacity_ - buffer_.size(); }

uint64_t Writer::bytes_pushed() const { return bytes_pushed_; }

bool Reader::is_finished() const { return closed_ && buffer_.empty(); }

uint64_t Reader::bytes_popped() const { return bytes_popped_; }

string_view Reader::peek() const
{
  if ( has_error() )
  {
    return "";
  }
  return buffer_;
}

void Reader::pop( uint64_t len )
{
  if ( has_error() )
  {
    return;
  }
  uint64_t bytes_to_pop = min( len, buffer_.size() );
  buffer_.erase( 0, bytes_to_pop );
  bytes_popped_ += bytes_to_pop;
}

uint64_t Reader::bytes_buffered() const { return buffer_.size(); }

void read( Reader& reader, uint64_t len, string& out )
{
  out = string( reader.peek().substr( 0, len ) );
  reader.pop( len );
}
