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
#include <fstream>
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
   static token_list* Instantiate()
      {
        static token_list *list = NULL;
        if(list == NULL)
        {
            list = new token_list();
        }
        return list;
      }


   void get()
      { 
        for (int i = 0; i<token.size() ;i++) 
        {
           std::cout << token[i] << " " << std::endl;
        }
      }

   bool is_present(int data)
      { 
        for (int i = 0; i<token.size() ;i++) 
        {
          if (data == token[i])
          {
            return TRUE;
          }
        }
        return FALSE;
      }

   void add (int data) 
      { 
        token.push_back(data);
      }

   void remove (int data) 
      { 
        for (int i = 0; i<token.size() ;i++) 
        {
          if (data == token[i])
          {
            token.erase(token.begin()+i);
          }
        }
      }

   void remove_all () 
      { 
        token.erase(token.begin(),token.end());
      }
};
/***********************************************************************************/




/***********************************************************************************/
/********************************* Select Channel **********************************/
const char* get_channel (int channel)
{
   if (channel == 1) return "TF1";
   else if (channel == 2) return "France 2";
   else if (channel == 3) return "France 3";
   else if (channel == 4) return "Canal+";
   else if (channel == 5) return "France 5";
   else if (channel == 6) return "M6";
   else if (channel == 7) return "ARTE";
   else if (channel == 8) return "D8";
   else if (channel == 9) return "W9";
   else if (channel == 10) return "TMC";
   else if (channel == 11) return "NT1";
   else if (channel == 12) return "NRJ12";
   else if (channel == 13) return "LCP";
   else if (channel == 14) return "France 4";
   else if (channel == 15) return "BFM TV";
   else if (channel == 16) return "i>TELE";
   else if (channel == 17) return "D17";
   else if (channel == 18) return "Gulli";
   else if (channel == 19) return "France Ô";
   else if (channel == 20) return "HD1";
   else if (channel == 21) return "L'Equipe 21";
   else if (channel == 22) return "6ter";
   else if (channel == 23) return "NUMERO 23";
   else if (channel == 24) return "RMC";
   else if (channel == 25) return "CHERIE25";
   else if (channel == 32) return "IDF1";
   else if (channel == 33) return "France 24";
   else if (channel == 34) return "BFM Business Paris";
   else if (channel == 41) return "PARIS PREMIERE";
   else if (channel == 42) return "CANAL+ SPORT";
   else if (channel == 43) return "CANAL+ CINEMA";
   else if (channel == 45) return "PLANETE+";
   else if (channel == 48) return "LCI";
   else if (channel == 51) return "TF1 HD";
   else if (channel == 52) return "France 2 HD";
   else if (channel == 56) return "M6HD";
   else if (channel == 57) return "ARTE HD";
   else return "";
}
/***********************************************************************************/




/***********************************************************************************/
/*********************************** get_info() ************************************/
char* get_info(int channel_number)
{
  char *info = (char*) malloc(60 * sizeof(char));
  strcpy(info, "");

  std::ifstream channels;
  channels.open("/home/sah0260/Documents/heliostv/src/Channels.conf");
  char channel_str[1000] = "";
  if (channels.is_open())
  {
    while (!channels.eof())
    {
      channels >> channel_str;
      char *temp;
      temp = strtok (channel_str, ":");

      while (temp != NULL)
      {
        if (!strcmp(get_channel(channel_number), temp))
        {
	  strcat(info, temp);
          strcat(info, ":");
          temp = strtok (NULL, ":");
          strcat(info, temp);
          strcat(info, ":");

          int i = 0;
	  for (i=0;i<9;i++)
	  {
            temp = strtok (NULL, ":");
	  }

          while (temp != NULL)
	  {
	    strcat(info, temp);
            temp = strtok (NULL, ":");
	    strcat(info, ",");
          }

        }
        temp = strtok (NULL, ":");
      }

    }
  }
  channels.close();

  return info;
}
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
      new_list = token_list::Instantiate();

      msgpack::unpack(&msg, data_, bytes_transferred);
      msgpack::type::tuple<int> dst;
      msg.get().convert(&dst);

      char answer[10] = "NOK";
      int client_n = 0;
      int test = dst.get<0>();

      fd = socket_.native_handle();
      std::cout << "fd : " << fd << std::endl;

      if (test==0)
      {
        client_n = rand()%1000000+1;
        strcpy(answer, "OK");
        while(new_list->is_present(client_n))
        {
          client_n = rand()%1000000+1;
        }
        new_list->add(client_n);
      }

      new_list->get();

      msgpack::type::tuple<int, std::string> pack(client_n, answer);
      msgpack::pack(sbuf, pack);

      boost::asio::async_write(socket_,
          boost::asio::buffer(sbuf.data(), sbuf.size()),
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
      fd(0)
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
      new_list = token_list::Instantiate();

      new_list->get();

      msgpack::unpack(&msg, data_, bytes_transferred);
      msgpack::type::tuple<int, int, std::string> dst;
      msg.get().convert(&dst);

      char answer[6] = "NOK";
      int test = dst.get<0>();

      fd = socket_.native_handle();
      std::cout << "fd : " << fd << std::endl;

      if (test==1 && new_list->is_present(dst.get<1>()))
      {
        strcpy(answer, "OK");

	char* info;
	info = get_info(atoi(dst.get<2>().c_str()));
        std::cout << "info : " << info << std::endl;

        msgpack::type::tuple<std::string> pack(answer);
        msgpack::pack(sbuf, pack);
        boost::asio::async_write(socket_,
            boost::asio::buffer(sbuf.data(), sbuf.size()),
            boost::bind(&session_streaming::handle_write, this,
              boost::asio::placeholders::error));
        pipeline(info, "localhost", port_, fd);
      }
      else
      {
        msgpack::type::tuple<std::string> pack(answer);
        msgpack::pack(sbuf, pack);
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
    new_list = token_list::Instantiate();

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

