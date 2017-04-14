#include "Connection.h"

Connection::Connection(void) : service_(), socket_(service_), work(service_)
{
	t = std::thread(std::bind(&Connection::start, this));
}

Connection::Connection(const boost::asio::ip::udp::endpoint& ep) : service_(), socket_(service_, ep), work(service_)
{
	t = std::thread(std::bind(&Connection::start, this));
}

Connection::~Connection(void)
{
	work.~work();
	service_.stop();
	t.join();
}

void Connection::Receive(size_t n)
{
	std::shared_ptr<Packet> packet(new Packet());
	socket_.async_receive_from(boost::asio::buffer(packet->buffer.prepare(n)), packet->endpoint, std::bind(&Connection::OnReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, packet));
}

void Connection::OnReceive(const boost::system::error_code& e, size_t n, std::shared_ptr<Packet> packet)
{
	if (!e)
	{
		//std::cout << "R" << e.message() << std::endl;

		packet->buffer.commit(n);
		mutex.lock();
		recv_buf.push(packet);
		mutex.unlock();

		//std::cout << "received " << n << " bytes." << std::endl;
	}
	Receive(65507);
}

void Connection::Send(const std::shared_ptr<Packet>& packet)
{
	mutex.lock();
	send_buf.insert(packet);
	mutex.unlock();
	//std::cout << "sending to " << packet->endpoint << std::endl;
	socket_.async_send_to(boost::asio::buffer(packet->buffer.data()), packet->endpoint, std::bind(&Connection::OnSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, packet));
}

void Connection::SendTo(const std::shared_ptr<Packet>& packet, const boost::asio::ip::udp::endpoint& ep)
{
	packet->endpoint = ep;
	Send(packet);
}

void Connection::OnSend(const boost::system::error_code& e, size_t n, const std::shared_ptr<Packet>& packet)
{
	if (!e)
	{
		//std::cout << "S" << e.message() << std::endl;
		//std::cout << "sent " << n << " bytes." << std::endl;
		//std::cout << packet->buffer.size() << " bytes in packet." << std::endl;
		mutex.lock();
		send_buf.erase(packet);
		mutex.unlock();
	}
}

void Connection::start(void)
{
	Receive(65507);

	service_.run();
}