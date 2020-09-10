#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <chrono>
#include <cstring>
#include <string>

// RECEIVE //

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

int main() {
  
  // SETTINGS //
  
  bool blue = false;
  
  unsigned short data_port = 54000;
  
  //////////////
  
  sf::RenderWindow window(sf::VideoMode(
                      static_cast<unsigned int>(300),
                      static_cast<unsigned int>(300)),
                      "Receive - Networking 2", static_cast<unsigned int>(get_video_mode()));
  
  sf::IpAddress host_ip;
  unsigned short host_port;
  
  {
    sf::UdpSocket socket;
    socket.setBlocking(false);

    if (socket.bind(data_port) != sf::Socket::Done)
    {
      std::cout << "Could not bind socket!" << std::endl;
      return 1;
    }
    
    {
      std::cout << "Finding host..." << std::endl;
      char data[100];
      std::size_t received;
      unsigned short tmp_port;
      while (std::strncmp(data, "Hosting network app test on ", 28)) {
        /*
        if (socket.receive(data, 100, received, host_ip, host_port) != sf::Socket::Done) {
          std::cout << "Error receiving data!" << std::endl;
          return 1;
        }
        */
        socket.receive(data, 100, received, host_ip, tmp_port);
        
        sf::Event event;
        while (window.pollEvent(event)) {
          if (event.type == sf::Event::Closed) window.close();
        }
        
        window.clear(sf::Color(100, 100, 155));
        window.display();
      }
      std::string str_port = "";
      for (unsigned int i = 28; i <= received; i++) {
        str_port += data[i];
      }
      host_port = std::stoi(str_port);
      std::clog << data << std::endl;
    }
    
    socket.unbind();
    std::cout << "Host found: " << host_ip << " at port " << host_port << std::endl;
  }
  
  sf::UdpSocket send_socket;
  send_socket.setBlocking(false);
  
  if (send_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
    std::cout << "Could not bind socket!" << std::endl;
    return 1;
  }
  
  sf::UdpSocket rec_socket;
  rec_socket.setBlocking(false);
  
  if (rec_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
    std::cout << "Could not bind socket!" << std::endl;
    return 1;
  }
  
  unsigned short client_port = rec_socket.getLocalPort();
  
  {
    std::string msg = "Connect network app test on " + std::to_string(client_port);
    char data[100];
    std::strcpy(data, msg.c_str());
    int size = 0;
    for (char c : data) {
      size++;
    }
    if (send_socket.send(data, size, host_ip, host_port) != sf::Socket::Done) {
      std::cout << "Could not send data!" << std::endl;
      return 1;
    }
  }
  
  int color = 25;
  
  while (window.isOpen()) {
    char data[100];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;
    
    if (rec_socket.receive(data, 100, received, sender, port) == sf::Socket::Error) {
      std::cout << "Error receiving data!" << std::endl;
      return 1;
    }
    std::string time = get_time();
    
    if (received != 0) {
      std::cout << "Received " << received
                << " bytes from " << sender
                << " on port " << port
                << " at " << time
                << ":\n" << data
                << "\n" << std::endl;
      color = 230;
    }
    
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }
    
    if (blue) {
      window.clear(sf::Color(0, 0, color));
    } else {
      window.clear(sf::Color(color, 255 - color, 0));
    }
    if (color > 25) color--;
    window.display();
    
  }
    
  return 0;
  
}