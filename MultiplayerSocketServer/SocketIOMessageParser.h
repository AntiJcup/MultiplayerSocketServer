#pragma once

#include <vector>
#include <cstddef>
#include <google/protobuf/message_lite.h>
#include <boost/beast/core.hpp>

#include "WrapperMessage.pb.h"

class SocketIOMessageParser
{
public:
	std::shared_ptr<google::protobuf::MessageLite> TryParseMessage(std::size_t size, boost::beast::flat_buffer& buffer);

private:

};

