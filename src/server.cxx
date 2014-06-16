#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <streambuf>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message::Raw> chat_smessage_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message::Raw& msg) = 0;
};

typedef boost::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:

  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    std::for_each(recent_smsgs_.begin(), recent_smsgs_.end(),
        boost::bind(&chat_participant::deliver, participant, _1));
  }


  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const chat_message::Raw& msg)
  {
    recent_smsgs_.push_back(msg);
    while(recent_smsgs_.size() > max_recent_msgs)
      recent_smsgs_.pop_front();

    std::for_each(participants_.begin(), participants_.end(),
        boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
  }


private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };

  chat_smessage_queue recent_smsgs_;
};

//----------------------------------------------------------------------

class chat_session
  : public chat_participant,
    public boost::enable_shared_from_this<chat_session>
{
public:
  chat_session(boost::asio::io_service& io_service, chat_room& room)
    : socket_(io_service),
      room_(room)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    room_.join(shared_from_this());
    boost::asio::async_read_until(socket_,
        buff, chat_message::Raw::delimiter,
        boost::bind(
          &chat_session::handle_read_smessage, shared_from_this(),
          boost::asio::placeholders::error));
  }

  void deliver(const chat_message::Raw& msg)
  {
    bool write_in_progress = !write_smsgs_.empty();
    write_smsgs_.push_back(msg);
    if (!write_in_progress)
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_smsgs_.front().get_message(),
            write_smsgs_.front().get_length()),
          boost::bind(&chat_session::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
  }

  void handle_read_smessage(const boost::system::error_code& error)
  {
    if(!error)
    {
      std::istream is(&buff);
      std::stringstream str_stream;

      str_stream << is.rdbuf();
      std::string buff_str = str_stream.str();

      chat_message::Raw read_smsg(buff_str);
      room_.deliver(read_smsg);

      //std::cout << buff_str << std::endl;
      boost::asio::async_read_until(socket_,
        buff, chat_message::Raw::delimiter,
        boost::bind(
          &chat_session::handle_read_smessage, shared_from_this(),
          boost::asio::placeholders::error));
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      write_smsgs_.pop_front();
      if (!write_smsgs_.empty())
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_smsgs_.front().get_message(),
              write_smsgs_.front().get_length()),
            boost::bind(&chat_session::handle_write, shared_from_this(),
              boost::asio::placeholders::error));
      }
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

private:
  tcp::socket socket_;
  chat_room& room_;

  chat_smessage_queue write_smsgs_;

  boost::asio::streambuf buff;
};

typedef boost::shared_ptr<chat_session> chat_session_ptr;

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint)
    : io_service_(io_service),
      acceptor_(io_service, endpoint)
  {
    chat_session_ptr new_session(new chat_session(io_service_, room_));
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&chat_server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(chat_session_ptr session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      session->start();
      chat_session_ptr new_session(new chat_session(io_service_, room_));
      acceptor_.async_accept(new_session->socket(),
          boost::bind(&chat_server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    }
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  chat_room room_;
};

typedef boost::shared_ptr<chat_server> chat_server_ptr;
typedef std::list<chat_server_ptr> chat_server_list;

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    chat_server_list servers;
    for (int i = 1; i < argc; ++i)
    {
      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
      chat_server_ptr server(new chat_server(io_service, endpoint));
      servers.push_back(server);
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
