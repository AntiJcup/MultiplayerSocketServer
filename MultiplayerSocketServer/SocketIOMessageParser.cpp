#include "SocketIOMessageParser.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/asio/streambuf.hpp>

std::shared_ptr<google::protobuf::WrapperMessage> SocketIOMessageParser::TryParseMessage(std::size_t size, boost::beast::flat_buffer& buffer)
{
	google::protobuf::io::ArrayInputStream array_input_stream(
		boost::asio::buffer_cast<const char*>(buffer.cdata()),
		size);
	google::protobuf::io::CodedInputStream coded_input_stream(&array_input_stream);

	auto message = std::make_shared<google::protobuf::WrapperMessage>();
	message->ParseFromCodedStream(&coded_input_stream);

	buffer.consume(buffer.size()); //Consume

	return message;
}
