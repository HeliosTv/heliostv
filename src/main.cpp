//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <msgpack.hpp>
#include <tuple>
#include <include/heliostv.h>
#include <gst/gst.h>



using boost::asio::ip::tcp;



/***********************************************************************************/
/********************************** class session **********************************/
class session
{
public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      msgpack::unpack(&msg, data_, bytes_transferred);
      msgpack::type::tuple<int, std::string> dst;
      msg.get().convert(&dst);

      char answer[6] = "NOK", app[64];
      int test = std::get<0>(dst);

      if (test==0)
      {
        strcpy(answer, "OK");
        msgpack::pack(sbuf, answer);
        boost::asio::async_write(socket_,
            boost::asio::buffer(sbuf.data(), bytes_transferred),
            boost::bind(&session::handle_write, this,
              boost::asio::placeholders::error));
        std::cout << std::get<1>(dst) << std::endl;
        pipeline(std::get<1>(dst).c_str(), "localhost", 6005);
      }
      else
      {
        msgpack::pack(sbuf, answer);
        boost::asio::async_write(socket_,
            boost::asio::buffer(sbuf.data(), bytes_transferred),
            boost::bind(&session::handle_write, this,
              boost::asio::placeholders::error));
      }
    }
    else
    {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

  msgpack::sbuffer sbuf;
  msgpack::unpacked msg;
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};
/***********************************************************************************/




/***********************************************************************************/
/********************************** class server ***********************************/
class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept()
  {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }
 
  void handle_accept(session* new_session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start();
    }
    else
    {
      delete new_session;
    }

    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};
/***********************************************************************************/




/***********************************************************************************/
/************************************** main ***************************************/
int main(int argc, char* argv[])
{
  gst_init (&argc, &argv);
  
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
/***********************************************************************************/

