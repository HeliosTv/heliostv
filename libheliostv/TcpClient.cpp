/*
*****************************************************************************
*                 Copyright (c) 2015 SoftAtHome
*
*     The computer program contained herein contains proprietary
*     information which is the property of SoftAtHome.  The program
*     may be used and/or copied only with the written permission of
*     SoftAtHome or in accordance with the terms and conditions
*     stipulated in the agreement/contract under which the programs
*     have been supplied.
*****************************************************************************/
/**
* @file     TcpClient.cpp
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include "heliostv/TcpClient.h"

namespace HeliosTv {

TcpClient::TcpClient(void) : io_service()
{
}

TcpClient::~TcpClient(void)
{
}

void TcpClient::Initialize(std::string host, int port)
{
	char port_str[10] = "";

 	socket = new boost::asio::ip::tcp::socket(io_service);

  	boost::asio::ip::tcp::resolver resolver(io_service);

	sprintf(port_str, "%d", port);
  	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), host, port_str);

	iterator = resolver.resolve(query);
}

boost::unique_future<int> TcpClient::connect(void)
{
	boost::promise<int> promise;
	boost::unique_future<int> future;

	future=promise.get_future();

	boost::asio::connect(*socket, iterator);

	return future;
}

std::size_t TcpClient::read(boost::asio::mutable_buffers_1 buffer)
{
	std::size_t size;
	size = boost::asio::read(*socket, buffer);

	return size;
}

std::size_t TcpClient::write(boost::asio::mutable_buffers_1 buffer)
{
	std::size_t size;
	size = boost::asio::write(*socket, buffer);

	return size;
}

boost::asio::ip::tcp::socket* TcpClient::get_socket()
{
	return socket;
}


} // namespace
