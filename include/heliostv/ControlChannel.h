#ifndef __CONTROLCHANNEL_H
#define __CONTROLCHANNEL_H

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
* @file     ControlChannel.h
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include <cstdlib>
#include <string>

#include <boost/thread/future.hpp>

#include <msgpack.hpp>

#include "ITransportClient.h"
#include "IChannelFactory.h"
#include "Stream.h"

#define max_length			1024

namespace HeliosTv {

class ControlChannel
{
    public:
                ControlChannel(ITransportClient *transport, IChannelFactory *factory, int features);
        virtual ~ControlChannel(void);

		boost::unique_future<HeliosTv::Stream*> newStream(std::string &uri, int token);

		boost::unique_future<int> connect();

		boost::unique_future<int> write();

		boost::unique_future<int> read();

    private:
		ITransportClient *transport;
		IChannelFactory *factory;
		int features;

};

} // namespace


#endif /* __CONTROLCHANNEL_H */

