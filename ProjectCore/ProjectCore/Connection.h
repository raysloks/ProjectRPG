#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>

#include <functional>
#include <mutex>
#include <thread>

#include <queue>
#include <set>

class Packet
{
public:
	boost::asio::streambuf buffer;
	boost::asio::ip::udp::endpoint endpoint;
};

class Connection
{
public:
	Connection(void);
	Connection(const boost::asio::ip::udp::endpoint& endpoint);

	~Connection(void);

	void Receive(size_t n);

	void OnReceive(const boost::system::error_code& e, size_t n, std::shared_ptr<Packet> packet);
	
	void Send(const std::shared_ptr<Packet>& packet);
	void SendTo(const std::shared_ptr<Packet>& packet, const boost::asio::ip::udp::endpoint& endpoint);

	void OnSend(const boost::system::error_code& e, size_t n, const std::shared_ptr<Packet>& packet);
	
	boost::asio::io_service service_;
	boost::asio::ip::udp::socket socket_;

	std::set<std::shared_ptr<Packet>> send_buf;
	std::queue<std::shared_ptr<Packet>> recv_buf;

	std::mutex mutex;

private:
	std::thread t;
	boost::asio::io_service::work work;

	void start(void);
};

#endif