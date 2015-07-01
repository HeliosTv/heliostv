#ifndef __TCPCHANNELFACTORY_H
#define __TCPCHANNELFACTORY_H

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
* @file     TcpChannelFactory.h
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include "ControlChannel.h"
#include "TcpClient.h"
#include "IChannelFactory.h"
#include "Stream.h"

namespace HeliosTv {

class TcpChannelFactory: public IChannelFactory
{
    public:
                TcpChannelFactory(void);
        virtual ~TcpChannelFactory(void);

		ControlChannel* newControlChannel(int features);
		boost::unique_future<HeliosTv::Stream*> newStreamChannel(std::string &uri, int token);

		void setHost(std::string &host);
		void setControlPort(int port);
		void setDataPort(int port);

		static TcpChannelFactory* Instantiate(void);

    private:
		std::string host;
		int port_control;
		int port_stream;
};

} // namespace


#endif /* __TCPCHANNELFACTORY_H */

