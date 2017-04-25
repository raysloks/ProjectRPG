#ifndef PACKET_H
#define PACKET_H

#include <boost/asio.hpp>

class Packet
{
public:
	Packet();
	~Packet();

	boost::asio::streambuf buffer;
	boost::asio::ip::udp::endpoint endpoint;
};

#endif