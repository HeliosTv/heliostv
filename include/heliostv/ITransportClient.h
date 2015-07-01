#ifndef __ITRANSPORTCLIENT_H
#define __ITRANSPORTCLIENT_H

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
* @file     ITransportClient.h
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/thread/future.hpp>

namespace HeliosTv {

class TcpClient;

class ITransportClient
{
    public:
                ITransportClient(void);
        virtual ~ITransportClient(void);

	virtual	boost::unique_future<int> connect() = 0;

	virtual std::size_t read(boost::asio::mutable_buffers_1 buffer) = 0;

	virtual std::size_t write(boost::asio::mutable_buffers_1 buffer) = 0;

	virtual boost::asio::ip::tcp::socket* get_socket() = 0;
};

} // namespace


#endif /* __ITRANSPORTCLIENT_H */

