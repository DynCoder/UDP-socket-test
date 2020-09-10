#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <iterator>
#include <chrono>
#include <vector>
//#include <string.h>
#include <cstring>
#include <string>


// SEND //

int get_video_mode() {
//#ifndef NDEBUG
  return sf::Style::Default;
//#else
//  return sf::Style::Fullscreen;
//#endif
}

std::string get_time() {
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                            std::chrono::system_clock::now().time_since_epoch());
  std::string ts = std::to_string(ms.count());
  return ts;
}

class recipient {
  public:
    
    recipient(sf::IpAddress ip = sf::IpAddress::None,
              unsigned short port = 54001) {
      m_ip = ip;
      m_port = port;
    }
    
    sf::IpAddress get_ip() const { return m_ip; }
    
    unsigned short get_port() const { return m_port; }
    
  private:
    
    sf::IpAddress m_ip;
    
    unsigned short m_port;
  
};

int main() {
  
  // SETTINGS //
  
  //sf::IpAddress broadcast = "192.168.178.255";
  
  std::vector<recipient> recipients{};
  
  unsigned short dest_port = 54000;
  
  //////////////
  
  sf::RenderWindow window(sf::VideoMode(
                      static_cast<unsigned int>(300),
                      static_cast<unsigned int>(300)),
                      "Send - Networking 2", static_cast<unsigned int>(get_video_mode()));
  
  sf::UdpSocket send_socket;
  send_socket.setBlocking(true);
  
  if (send_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
    std::cout << "Could not bind sending socket!" << std::endl;
    return 1;
  }
  
  sf::UdpSocket rec_socket;
  rec_socket.setBlocking(false);
  
  if (rec_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
    std::cout << "Could not bind receiving socket!" << std::endl;
    return 1;
  }
  
  unsigned short host_port = rec_socket.getLocalPort();
  
  int color = 25;
  
  while (window.isOpen()) {
    
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type == sf::Event::MouseButtonPressed) {
        char data[] = "Hello";
        int size = 0;
        for (char c : data) {
          size++;
        }
        for (recipient r : recipients) {
          std::clog << r.get_ip() << " " << r.get_port() << std::endl;
          while (send_socket.send(data, size, r.get_ip(), r.get_port()) != sf::Socket::Done) {
            sf::Event subevent;
            while (window.pollEvent(subevent)) {
              if (subevent.type == sf::Event::Closed) window.close();
            }
            window.clear(sf::Color(100, 100, 155));
            window.display();
          }
          std::string time = get_time();
          std::cout << "Sent " << size
                    << " bytes to " << r.get_ip()
                    << " at port " << r.get_port()
                    << " at " << time << std::endl;
        }
        color = 230;
      }
    }
    
    { // Send host info
      std::string msg = "Hosting network app test on " + std::to_string(host_port);
      char data[100];
      std::strcpy(data, msg.c_str());
      int size = 0;
      for (char c : data) {
        size++;
      }
      if (send_socket.send(data, size, sf::IpAddress::Broadcast, dest_port) != sf::Socket::Done) {
        std::cout << "Could not send host info!" << std::endl;
        return 1;
      }
      std::string time = get_time();
      /*
      std::cout << "Sent " << size
                << " bytes at "
                << time << "\n"
                << std::endl;
      */
    }
    
    { // Recieve all data
      char data[100];
      std::size_t received;
      sf::IpAddress ip;
      unsigned short port;
      if (rec_socket.receive(data, 100, received, ip, port) == sf::Socket::Error) {
        std::cout << "Error receiving data!" << std::endl;
        return 1;
      }
      std::string time = get_time();
      
      if (received != 0) {
        std::cout << "Received " << received
                  << " bytes from " << ip
                  << " on port " << port
                  << " at " << time
                  << ":\n" << data
                  << "\n" << std::endl;
      }
      
      if (std::strncmp(data, "Connect network app test on ", 28) == 0) {
        unsigned short sender_port = 0;
        std::string str_port = "";
        for (unsigned int i = 28; i <= received; i++) {
          str_port += data[i];
        }
        sender_port += std::stoi(str_port);
        std::clog << "New recipient: " << ip << " on port " << sender_port << std::endl;
        recipients.push_back(recipient(ip, sender_port));
      }
    }
    
    window.clear(sf::Color(0, 0, color));
    if (color > 25) color--;
    window.display();
    
  }
    
  return 0;
  
}