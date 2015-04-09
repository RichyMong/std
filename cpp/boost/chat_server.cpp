//
// server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

//----------------------------------------------------------------------
//----------------------------------------------------------------------

class session
  : public boost::enable_shared_from_this<session>
{
public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service), io_service_(io_service), deadline_(io_service)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    deadline_.expires_from_now(boost::posix_time::seconds(5));
    deadline_.async_wait(boost::bind(&session::timeouted, shared_from_this(), boost::asio::placeholders::error));

    boost::asio::async_read(socket_,
        boost::asio::buffer(new char[100], 100),
        boost::bind(
          &session::handle_read, shared_from_this(),
          boost::asio::placeholders::error));
  }

  void timeouted(const boost::system::error_code& error) {
      if (!error) {
          std::cout << time(NULL) << ": timeout " << socket_.remote_endpoint() << std::endl;
          socket_.close();
      } else {
          std::cout << "timeout error " << error.message() << std::endl;
      }
  }

  void handle_read(const boost::system::error_code& error)
  {
    deadline_.cancel();
    if (!error)
    {
    }
    else
    {
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
    }
    else
    {
    }
  }

private:
  tcp::socket socket_;
  boost::asio::io_service& io_service_;
  boost::asio::deadline_timer deadline_;
};

typedef boost::shared_ptr<session> session_ptr;

//----------------------------------------------------------------------

class server
{
public:
  server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint)
    : io_service_(io_service),
      acceptor_(io_service, endpoint)
  {
    start_accept();
  }

  void start_accept()
  {
    session_ptr new_session(new session(io_service_));
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(session_ptr session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      std::cout << time(NULL) << ": start session " << session->socket().remote_endpoint() << std::endl;
      session->start();
      sessions_.push_back(session);
    }

    start_accept();
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  std::list<session_ptr> sessions_;
};

typedef boost::shared_ptr<server> server_ptr;
typedef std::list<server_ptr> server_list;

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    server_list servers;
    for (int i = 1; i < argc; ++i)
    {
      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
      server_ptr the_server(new server(io_service, endpoint));
      servers.push_back(the_server);
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
