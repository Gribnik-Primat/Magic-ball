#include "client.h"

Client::Client(Conn& conn) : m_conn(conn) {
  m_answers.push_back("Maybe");
  m_answers.push_back("Yes");
  m_answers.push_back("No");
  m_answers.push_back("Ask again later");
  srand(getpid());
}

std::string Client::_getAnswer(std::string& question) {
  int idx = rand() % m_answers.size();
  return m_answers[idx];
}

void Client::exec() {
  char* buf = new char[MAX_MSG_SIZE + 1];
  while(true) {
    if (m_conn.Read(buf, MAX_MSG_SIZE)) {
      std::string question = buf;
      std::string answer = _getAnswer(question);
      m_conn.Write(answer.c_str(), answer.length() + 1);
      syslog(LOG_INFO, "read from client finished");
      delete[] buf;
      break;
    }
  }
}
