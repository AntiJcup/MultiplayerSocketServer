#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>

typedef boost::uuids::uuid socket_io_session_id_t;
typedef boost::hash<socket_io_session_id_t> socket_io_session_id_hasher_t;