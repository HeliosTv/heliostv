#ifndef __TcpClient_H
#define __TcpClient_H
/*
*****************************************************************************
* HeliosTv
* Copyright (C) 2015  SoftAtHome
*
* Authors:
*   Romain Picard <romain dot picard at softathome dot com>
*   Fabien Felio  <fabien dot felio at softathome dot com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301, USA.
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
        virtual                             ~TcpClient(void);

                void                        Initialize(std::string host, int port);
                boost::unique_future<int>   connect(void);
                std::size_t                 read(boost::asio::mutable_buffers_1 buffer);
                std::size_t                 write(boost::asio::mutable_buffers_1 buffer);
                boost::asio::ip::tcp::socket* get_socket();

    private:
                boost::asio::io_service     io_service;
                boost::asio::ip::tcp::socket *socket;
                boost::asio::ip::tcp::resolver::iterator iterator;

    };

} // namespace


#endif /* __TcpClient_H */

