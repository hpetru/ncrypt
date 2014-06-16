#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>
#include "chat_message.hpp"
#include "public_ncrypt.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message::Encript> chat_message_queue;

class Client
{
  public:
    Client(boost::asio::io_service& io_service,
        tcp::resolver::iterator endpoint_iterator,
        public_ncrypt::Key& key)
      : io_service_(io_service),
        socket_(io_service),
        key_(key)
    {
      tcp::endpoint endpoint = *endpoint_iterator;
      socket_.async_connect(endpoint,
        boost::bind(&Client::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
    }

    void write(chat_message::Encript msg)
    {
      io_service_.post(boost::bind(&Client::do_write, this, msg));
    }

    void close()
    {
      do_close();
    }

  private:
    void handle_connect(const boost::system::error_code& error,
        tcp::resolver::iterator endpoint_iterator)
    {
      if(!error)
      {
        boost::asio::async_read_until(socket_,
            buff, chat_message::Raw::delimiter,
            boost::bind(&Client::handle_read, this,
              boost::asio::placeholders::error));
      }
      else if (endpoint_iterator != tcp::resolver::iterator())
      {
        socket_.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        socket_.async_connect(endpoint,
            boost::bind(&Client::handle_connect, this,
              boost::asio::placeholders::error, ++endpoint_iterator));
      }
    }
    void do_write(chat_message::Encript msg)
    {
      bool write_in_progress = !write_msgs_.empty();
      write_msgs_.push_back(msg);
      if(!write_in_progress)
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().get_message(),
              write_msgs_.front().get_length()),
            boost::bind(&Client::handle_write, this,
              boost::asio::placeholders::error));
      }
    }

    void handle_read(const boost::system::error_code& error)
    {
      if(!error)
      {
        std::istream is(&buff);
        std::stringstream str_stream;
        str_stream << is.rdbuf();
        std::string buff_str = str_stream.str();
        chat_message::Decript read_msg(buff_str, key_);
        std::cout << read_msg.get_message() << std::endl;

        boost::asio::async_read_until(socket_,
            buff, chat_message::Raw::delimiter,
            boost::bind(&Client::handle_read, this,
              boost::asio::placeholders::error));
      }
      else
      {
        do_close();
      }
    }

    void handle_write(const boost::system::error_code& error)
    {
      if(!error)
      {
        write_msgs_.pop_front();
        if(!write_msgs_.empty())
        {
          std::cout << write_msgs_.front().get_message() << std::endl;
          boost::asio::async_write(socket_,
              boost::asio::buffer(write_msgs_.front().get_message(),
                write_msgs_.front().get_length()),
              boost::bind(&Client::handle_write, this,
                boost::asio::placeholders::error));
        }
      }
      else
      {
        do_close();
      }
    }

    void do_close()
    {
      socket_.close();
    }

    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    chat_message_queue write_msgs_;
    boost::asio::streambuf buff;
    public_ncrypt::Key& key_;
};

int main(int argc, char* argv[])
{
  try
  {
    if(argc != 3)
    {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    public_ncrypt::Key key("key.x");
    Client client(io_service, iterator, key);

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    std::string line;

    while(std::getline(std::cin, line))
    {
      chat_message::Encript msg(line, key);
      client.write(msg);
    }

    client.close();
    t.join();
  }
  catch(std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
