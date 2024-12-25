#include "tcp_receiver.hh"
#include <iostream>
using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if(writer().has_error()) {
    return;
  }
  if(message.RST) {
    reader().set_error();
    return;
  }

  if(!ISN_.has_value()) {
    if (message.SYN) {
      ISN_ = message.seqno;
    } else {
      return;
    }
  }
  uint64_t absolute_seqno =  message.seqno.unwrap(ISN_.value(), reassembler_.writer().bytes_pushed() + 1);
  uint64_t stream_index = absolute_seqno - (message.SYN ? 0 : 1);
  reassembler_.insert(stream_index, std::move(message.payload), message.FIN);
}

TCPReceiverMessage TCPReceiver::send() const
{

  TCPReceiverMessage msg;
  msg.RST = writer().has_error();
  if(ISN_.has_value()) {
    msg.ackno = ISN_.value() + writer().bytes_pushed() + writer().is_closed() + 1;
    std::cout << writer().bytes_pushed() << std::endl;
  }
  
  msg.window_size = writer().available_capacity() > UINT16_MAX ? UINT16_MAX : writer().available_capacity();
  return msg;
}
