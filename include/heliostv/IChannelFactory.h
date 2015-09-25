#ifndef __ICHANNELFACTORY_H
#define __ICHANNELFACTORY_H
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

namespace HeliosTv {

class ControlChannel;
class Stream;

class IChannelFactory
{
    public:
                                                IChannelFactory(void);
        virtual                                 ~IChannelFactory(void);

        virtual ControlChannel*                 newControlChannel(int features) = 0;
        virtual boost::unique_future<Stream*>   newStreamChannel(std::string &uri, int token) = 0;

};

} // namespace


#endif /* __ICHANNELFACTORY_H */

