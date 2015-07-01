#ifndef __STREAM_H
#define __STREAM_H

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
* @file     Stream.h
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

namespace HeliosTv {

class Stream
{
    public:
                Stream(ITransportClient *transport, std::string &uri);
        virtual ~Stream(void);

		boost::unique_future<int> connect();

		boost::unique_future<int> authorize(int token);

		std::size_t read(boost::asio::mutable_buffers_1 buffer);

		std::size_t available();

		boost::asio::mutable_buffers_1 get_data_buffer();

    private:
		ITransportClient *transport;
		std::string &uri;

};

} // namespace


#endif /* __STREAM_H */

