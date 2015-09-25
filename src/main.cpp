/*
*****************************************************************************
* HeliosTv
* Copyright (C) 2015  SoftAtHome
*
* Authors:
*   Fabien Felio <fabien dot felio at softathome dot com>
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
/*********************************** get_info() ************************************/
char* get_info(std::string &channelName)
{
    /// @todo : use code from dvbbasebin
    char *info = (char*) malloc(60 * sizeof(char));
    strcpy(info, "");

    std::ifstream channels;
    std::string channels_path(getenv("HOME"));
    channels_path += "/Channels.conf";
    channels.open(channels_path.c_str());
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
                if (!strcmp(channelName.c_str(), temp))
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
                    new_list->get();

                    msgpack::type::tuple<int, std::string> pack(client_n, answer);
                    msgpack::pack(sbuf, pack);
                }

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


class session_streaming
{
    public:
        session_streaming(boost::asio::io_service& io_service)
            : socket_(io_service),
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
                    info = get_info(dst.get<2>());
                    std::cout << "info : " << info << std::endl;

                    msgpack::type::tuple<std::string> pack(answer);
                    msgpack::pack(sbuf, pack);
                    boost::asio::async_write(socket_,
                            boost::asio::buffer(sbuf.data(), sbuf.size()),
                            boost::bind(&session_streaming::handle_write, this,
                                boost::asio::placeholders::error));
                    pipeline(info, fd);
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
};


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
            session_streaming* new_session_streaming = new session_streaming(io_service_);
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

