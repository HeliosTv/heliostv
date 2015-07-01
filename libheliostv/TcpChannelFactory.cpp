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
* @file     TcpChannelFactory.cpp
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include "heliostv/TcpChannelFactory.h"

namespace HeliosTv {

TcpChannelFactory::TcpChannelFactory(void) : host("localhost"),
					     port_control(6005),
					     port_stream(6006)
{
}

TcpChannelFactory::~TcpChannelFactory(void)
{
}

ControlChannel* TcpChannelFactory::newControlChannel(int features)
{
	HeliosTv::TcpClient *client;
	client = new HeliosTv::TcpClient();
	client->Initialize(host, port_control);

	HeliosTv::ControlChannel *control;
	control = new HeliosTv::ControlChannel(client, this, features);

	return control;
}

boost::unique_future<HeliosTv::Stream*> TcpChannelFactory::newStreamChannel(std::string &uri, int token)
{
	boost::promise<HeliosTv::Stream*> promise;
	boost::unique_future<HeliosTv::Stream*> future;

	future=promise.get_future();

        std::cout << "newStreamChannel" << std::endl;
	HeliosTv::TcpClient *client;
	client = new HeliosTv::TcpClient();
	client->Initialize(host, port_stream);

	HeliosTv::Stream *stream;
	stream = new HeliosTv::Stream(client, uri);

	stream->connect();

	stream->authorize(token);

	promise.set_value(stream);

	return future;
}

void TcpChannelFactory::setHost(std::string &host)
{
	this->host = host;
}

void TcpChannelFactory::setControlPort(int port)
{
	port_control = port;
}

void TcpChannelFactory::setDataPort(int port)
{
	port_stream = port;
}

TcpChannelFactory* TcpChannelFactory::Instantiate(void)
{
        static TcpChannelFactory *TcpChannel = NULL;
        if(TcpChannel == NULL)
        {
            TcpChannel = new TcpChannelFactory();
        }
        return TcpChannel;
}


} // namespace
