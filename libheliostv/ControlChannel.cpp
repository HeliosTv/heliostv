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
* @file     ControlChannel.cpp
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include "heliostv/ControlChannel.h"

namespace HeliosTv {

ControlChannel::ControlChannel(ITransportClient *transport, IChannelFactory *factory, int features) : 
				transport(transport),
				factory(factory),
				features(features)
{
}

ControlChannel::~ControlChannel(void)
{
}

boost::unique_future<HeliosTv::Stream*> ControlChannel::newStream(std::string &uri, int token)
{
        std::cout << "newStream" << std::endl;
	boost::promise<HeliosTv::Stream*> promise;
	boost::unique_future<HeliosTv::Stream*> future;

	future=promise.get_future();

	boost::unique_future<HeliosTv::Stream*> future1 = factory->newStreamChannel(uri, token);

	HeliosTv::Stream *s;
	s = future1.get();

	promise.set_value(s);

	return future;
}

boost::unique_future<int> ControlChannel::connect()
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

boost::unique_future<int> ControlChannel::write()
{
	boost::promise<int> promise;
	boost::unique_future<int> future;

	future=promise.get_future();

	// REQUEST control
	msgpack::sbuffer sbuf_control;
	char temp_control[max_length];

	msgpack::type::tuple<int> req_control(0);

	msgpack::pack(sbuf_control, req_control);
	transport->write(boost::asio::buffer(sbuf_control.data(), sbuf_control.size()));

	return future;
}


boost::unique_future<int> ControlChannel::read()
{
	boost::promise<int> promise;
	boost::unique_future<int> future;

	future=promise.get_future();

	// REPLY control
	char reply_control[10];
	size_t reply_length_control = transport->read(boost::asio::buffer(reply_control, 9));

	int ident = 0;

	msgpack::unpacked msg_control;
        msgpack::type::tuple<int, std::string> rep_control;

	msgpack::unpack(&msg_control, reply_control, reply_length_control);
	msg_control.get().convert(&rep_control);

	ident = rep_control.get<0>();

	promise.set_value(ident);

	return future;
}


} // namespace
