#ifndef __STREAM_H
#define __STREAM_H
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
#include <string>
#include <boost/thread/future.hpp>
#include <msgpack.hpp>
#include "ITransportClient.h"

namespace HeliosTv {

class Stream
{
    public:
                Stream(ITransportClient *transport, std::string &uri);
        virtual ~Stream(void);

                boost::unique_future<int>   connect();
                boost::unique_future<int>   authorize(int token);
                std::size_t                 read(boost::asio::mutable_buffers_1 buffer);
                std::size_t                 available();
                boost::asio::mutable_buffers_1 get_data_buffer();

    private:
        ITransportClient *transport;
        std::string &uri;

};

} // namespace


#endif /* __STREAM_H */

