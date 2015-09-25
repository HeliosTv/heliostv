#ifndef __ITRANSPORTCLIENT_H
#define __ITRANSPORTCLIENT_H
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
#include <boost/asio.hpp>
#include <boost/thread/future.hpp>

namespace HeliosTv {

class TcpClient;

class ITransportClient
{
    public:
                                            ITransportClient(void);
        virtual                             ~ITransportClient(void);

        virtual boost::unique_future<int>   connect() = 0;
        virtual std::size_t                 read(boost::asio::mutable_buffers_1 buffer) = 0;
        virtual std::size_t                 write(boost::asio::mutable_buffers_1 buffer) = 0;
        virtual boost::asio::ip::tcp::socket* get_socket() = 0;
};

} // namespace


#endif /* __ITRANSPORTCLIENT_H */

