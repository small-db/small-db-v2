// prevent multiple inclusions of the header file
#pragma once

#include <vector>

class Message {
public:
  virtual ~Message() = default;
  // virtual int get_length() = 0;
  virtual void encode(std::vector<char> &buffer) = 0;
};

class NetworkPackage {
private:
  std::vector<Message *> messages;

public:
  void add_message(Message *message);
};