//
// async_tcp_echo_server_streaming.cpp
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
/*************************** class token list singleton ****************************/
class token_list
{
private:
   std::vector<int> token;

   token_list () : token({})
      {
      }

   //token_list* operator= (const token_list*){}

public:
   static token_list* Instance()
      {
        static token_list *list = NULL;
        if(list == NULL)
        {
            list = new token_list();
            list->get();
        }
        std::cout << "token list has been created! \n" << std::endl;
        return list;
      }


   void get()
      { 
        std::cout << "starting get function" << std::endl;
        std::cout << "this list contains : ";
        for (int i = 0; i<token.size() ;i++) 
        {
           std::cout << token[i] << " ";
        }
        std::cout << "\n" << std::endl;
      }

   bool is_present(int data)
      { 
        std::cout << "starting is_present function" << std::endl;
        for (int i = 0; i<token.size() ;i++) 
        {
          if (data == token[i])
          {
            std::cout << "data : " << data << " is present" << std::endl;
            std::cout << "\n" << std::endl;
            return TRUE;
          }
        }
        std::cout << "data : " << data << " is not present" << std::endl;
        std::cout << "\n" << std::endl;
        return FALSE;
      }

   void add (int data) 
      { 
        std::cout << "starting add function" << std::endl;
        token.push_back(data);
        std::cout << "data : " << data << " has been added" << std::endl;
        std::cout << "\n" << std::endl;
      }

   void remove (int data) 
      { 
        std::cout << "starting remove function" << std::endl;
        for (int i = 0; i<token.size() ;i++) 
        {
          if (data == token[i])
          {
            token.erase(token.begin()+i);
          }
        }
        std::cout << "data : " << data << " has been erased" << std::endl;
        std::cout << "\n" << std::endl;
      }

   void remove_all () 
      { 
        token.erase(token.begin(),token.end());
        std::cout << "all data have been erased" << std::endl;
        std::cout << "\n" << std::endl;
      }
};
/***********************************************************************************/




/***********************************************************************************/
/****************************** class session control ******************************/
class session_control
{
public:
  session_control(boost::asio::io_service& io_service)
    : socket_(io_service),
      fd(1)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session_control::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      token_list *new_list;
      new_list = token_list::Instance();

      msgpack::unpack(&msg, data_, bytes_transferred);
      msgpack::type::tuple<int> dst;
      msg.get().convert(&dst);

      char answer[10] = "NOK";
      int client_n = 0;
      int test = std::get<0>(dst);

      std::cout << "fd : " << fd << std::endl;
      fd = socket_.native_handle();
      std::cout << "fd : " << fd << std::endl;

      new_list->get();

      if (test==0)
      {
        client_n = rand()+1;
        strcpy(answer, "OK");
        while(new_list->is_present(client_n))
        {
          client_n = rand()+1;
        }
        new_list->add(client_n);
      }

      new_list->get();

      //msgpack::type::tuple<double> pack(client_n);
      msgpack::type::tuple<int, std::string> pack(client_n, answer);
      std::cout << "client : " << std::get<0>(pack) << std::endl;
      msgpack::pack(sbuf, pack);

      boost::asio::async_write(socket_,
          boost::asio::buffer(sbuf.data(), bytes_transferred),
          boost::bind(&session_control::handle_write, this,
            boost::asio::placeholders::error));
      std::cout << std::endl;
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
          boost::bind(&session_control::handle_read, this,
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
  int fd;
  enum { max_length = 1024 };
  char data_[max_length];
};
/***********************************************************************************/

/***********************************************************************************/
/****************************** class server control *******************************/
class server_control
{
public:
  server_control(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept()
  {
    session_control* new_session_control = new session_control(io_service_);
    acceptor_.async_accept(new_session_control->socket(),
        boost::bind(&server_control::handle_accept, this, new_session_control,
          boost::asio::placeholders::error));
  }
 
  void handle_accept(session_control* new_session_control,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session_control->start();
    }
    else
    {
      delete new_session_control;
    }

    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};
/***********************************************************************************/




/***********************************************************************************/
/***************************** class session streaming *****************************/
class session_streaming
{
public:
  session_streaming(boost::asio::io_service& io_service, short port)
    : socket_(io_service),
      port_(port),
      fd(2)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session_streaming::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      token_list *new_list;
      new_list = token_list::Instance();

      new_list->get();

      msgpack::unpack(&msg, data_, bytes_transferred);
      msgpack::type::tuple<int, int, std::string> dst;
      msg.get().convert(&dst);

      char answer[6] = "NOK";
      int test = std::get<0>(dst);

      std::cout << "fd : " << fd << std::endl;
      fd = socket_.native_handle();
      std::cout << "fd : " << fd << std::endl;

      if (test==1 && new_list->is_present(std::get<1>(dst)))
      {
        strcpy(answer, "OK");

        msgpack::type::tuple<std::string> pack(answer);
        msgpack::pack(sbuf, pack);
        boost::asio::async_write(socket_,
            boost::asio::buffer(sbuf.data(), bytes_transferred),
            boost::bind(&session_streaming::handle_write, this,
              boost::asio::placeholders::error));
        pipeline(std::get<2>(dst).c_str(), "localhost", port_, fd);
      }
      else
      {
        msgpack::pack(sbuf, answer);
        boost::asio::async_write(socket_,
            boost::asio::buffer(sbuf.data(), bytes_transferred),
            boost::bind(&session_streaming::handle_write, this,
              boost::asio::placeholders::error));
      }

      std::cout << std::endl;
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
          boost::bind(&session_streaming::handle_read, this,
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
  int fd;
  enum { max_length = 1024 };
  char data_[max_length];
  short port_;
};
/***********************************************************************************/

/***********************************************************************************/
/***************************** class server streaming ******************************/
class server_streaming
{
public:
  server_streaming(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      port_(port)
  { 
    start_accept();
  }

private:
  void start_accept()
  {
    session_streaming* new_session_streaming = new session_streaming(io_service_, this->port_);
    acceptor_.async_accept(new_session_streaming->socket(),
        boost::bind(&server_streaming::handle_accept, this, new_session_streaming,
          boost::asio::placeholders::error));
  }
 
  void handle_accept(session_streaming* new_session_streaming,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session_streaming->start();
    }
    else
    {
      delete new_session_streaming;
    }

    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  short port_;
};
/***********************************************************************************/




/***********************************************************************************/
/************************************** main ***************************************/
int main(int argc, char* argv[])
{
  gst_init (&argc, &argv);
  
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: async_tcp_echo_server_streaming <port control> <port streaming>\n";
      return 1;
    }

    token_list *new_list;
    new_list = token_list::Instance();
    
/*
    new_list->get();
    if (!new_list->is_present(12))
      new_list->add(12);
    if (!new_list->is_present(10))
      new_list->add(10);
    if (!new_list->is_present(1050))
      new_list->add(1050);
    if (!new_list->is_present(12))
      new_list->add(12);
    if (!new_list->is_present(27))
      new_list->add(27);
    if (!new_list->is_present(85))
      new_list->add(85);

    new_list->get();

    if (new_list->is_present(10))
      new_list->remove(10);

    if (new_list->is_present(27))
      new_list->remove(27);

    if (new_list->is_present(855))
      new_list->remove(855);
    new_list->get();

    new_list->remove_all();
    new_list->get();
    */

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server_control server_control(io_service, atoi(argv[1]));
    server_streaming server_streaming(io_service, atoi(argv[2]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
/***********************************************************************************/

