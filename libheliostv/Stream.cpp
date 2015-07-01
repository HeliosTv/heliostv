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
* @file     Stream.cpp
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/
#include "heliostv/Stream.h"

namespace HeliosTv {

Stream::Stream(ITransportClient *transport, std::string &uri) : transport(transport),
								uri(uri)
{
}

Stream::~Stream(void)
{
}

boost::unique_future<int> Stream::connect()
{
	boost::promise<int> promise;
	boost::unique_future<int> future;

	future=promise.get_future();

	try
	{
	  transport->connect();
	}
	catch (boost::system::system_error const& e)
	{
	  std::cout << "Warning: could not connect : " << e.what() << std::endl;
	}

	return future;
}

boost::unique_future<int> Stream::authorize(int token)
{
	boost::promise<int> promise;
	boost::unique_future<int> future;

	future=promise.get_future();

    	// REQUEST STREAM
  	msgpack::sbuffer sbuf_stream;

    	msgpack::type::tuple<int, int, std::string> req_stream(1, token, uri);

    	msgpack::pack(sbuf_stream, req_stream);
    	transport->write(boost::asio::buffer(sbuf_stream.data(), sbuf_stream.size()));


    	// REPLY STREAM
    	char reply_stream[16];
   	size_t reply_length_stream = transport->read(boost::asio::buffer(reply_stream, 16));

    	msgpack::unpacked msg_stream;
    	msgpack::type::tuple<std::string> rep_stream;

    	msgpack::unpack(&msg_stream, reply_stream, reply_length_stream);
    	msg_stream.get().convert(&rep_stream);

	return future;
}

std::size_t Stream::read(boost::asio::mutable_buffers_1 buffer)
{
   	size_t size = transport->read(buffer);

	return size;
}

std::size_t Stream::available()
{
	std::size_t size;

	size = (transport->get_socket())->available();
	
	return size;
}

boost::asio::mutable_buffers_1 Stream::get_data_buffer()
{
	char* data = (char*)malloc(8196);
	boost::asio::mutable_buffers_1 buffer = boost::asio::buffer(data, 8196);
	transport->read(buffer);

	return buffer;
}

} // namespace
