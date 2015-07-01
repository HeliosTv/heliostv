#ifndef __TcpClient_H
#define __TcpClient_H

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
* @file     TcpClient.h
* @author   R. Picard
* @date     13/05/2015
*
******************************************************************************
*/

#include <cstdlib>
#include <boost/thread/future.hpp>
#include <boost/asio.hpp>
#include <string.h>
#include "ITransportClient.h"

namespace HeliosTv {

class TcpClient: public ITransportClient
{
    public:
                TcpClient(void);
        virtual ~TcpClient(void);

		void Initialize(std::string host, int port);

		boost::unique_future<int> connect(void);

		std::size_t read(boost::asio::mutable_buffers_1 buffer);

		std::size_t write(boost::asio::mutable_buffers_1 buffer);

		boost::asio::ip::tcp::socket* get_socket();

    private:
  		boost::asio::io_service io_service;
 		boost::asio::ip::tcp::socket *socket;
  		boost::asio::ip::tcp::resolver::iterator iterator;

};

} // namespace


#endif /* __TcpClient_H */

