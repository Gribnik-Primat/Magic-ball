#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include <vector>
#include <ctime>
#include "conn.h"
#include "host.h"

class Client {
public:
  Client(Conn& conn);
  void exec();
private:
  std::string _getAnswer(std::string& question);

  Conn m_conn;
  std::vector<std::string> m_answers;
};
#endif //CLIENT_H
