#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>

typedef boost::uuids::uuid multiplayer_room_id_t;
typedef boost::hash<multiplayer_room_id_t> multiplayer_room_id_hasher_t;

class MultiplayerRoom;
typedef std::shared_ptr<MultiplayerRoom> multiplayer_room_t;

typedef google::protobuf::WrapperMessage wrapper_message_t;
