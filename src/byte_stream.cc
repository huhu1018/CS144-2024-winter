#include "byte_stream.hh"

#include <algorithm>
#include <stdexcept>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

Reader& ByteStream::reader() { return *static_cast<Reader*>( this ); }

const Reader& ByteStream::reader() const { return *static_cast<const Reader*>( this ); }

Writer& ByteStream::writer() { return *static_cast<Writer*>( this ); }

const Writer& ByteStream::writer() const { return *static_cast<const Writer*>( this ); }

bool Writer::is_closed() const { return wirter_closed_; }

void Writer::push( string data )
{
  if ( has_error() )
  {
    return;
  }
  uint64_t bytes_to_write = min( data.size(), available_capacity() );
  // 使用 vector 的 insert 方法
  buffer_.insert(buffer_.end(), data.begin(), data.begin() + bytes_to_write);
  bytes_pushed_ += bytes_to_write;
}

void Writer::close() { wirter_closed_ = true; }

uint64_t Writer::available_capacity() const { return capacity_ - buffer_.size(); }

uint64_t Writer::bytes_pushed() const { return bytes_pushed_; }

bool Reader::is_finished() const { return wirter_closed_ && buffer_.empty(); }

uint64_t Reader::bytes_popped() const { return bytes_popped_; }

// 返回 string_view，避免不必要的拷贝
string_view Reader::peek() const
{
  if ( has_error() )
  {
    return "";
  }
  // 关键修改：使用 string_view 构造函数
  return string_view(buffer_.data(), buffer_.size());
}

void Reader::pop( uint64_t len )
{
  if ( has_error() )
  {
    return;
  }
  uint64_t bytes_to_pop = min( len, buffer_.size() );
  // 使用 vector 的 erase 方法
  buffer_.erase(buffer_.begin(), buffer_.begin() + bytes_to_pop);
  bytes_popped_ += bytes_to_pop;
}

uint64_t Reader::bytes_buffered() const { return buffer_.size(); }

void read( Reader& reader, uint64_t len, string& out )
{
  // 使用 string_view 避免不必要的拷贝
  string_view sv = reader.peek();
  out = string(sv.begin(), sv.begin() + min(len, sv.size()));
  reader.pop( out.size() );
}
