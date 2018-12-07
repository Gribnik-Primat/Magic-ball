#include "host.h"

Host::Host(std::vector<Conn*>& conn) : m_conn(conn) {

}

void Host::exec() {
  std::string inputStr;
  std::getline(std::cin, inputStr);
  while(inputStr.compare("exit")!=0){
    for(int i = 0; i < CLIENTS_COUNT; i++) {
      syslog(LOG_INFO, "writing to client");
      m_conn[i]->Write(inputStr.c_str(), inputStr.length() + 1);
      char* buf = new char[MAX_MSG_SIZE + 1];
      while(!m_conn[i]->Read(buf, MAX_MSG_SIZE + 1)) {}
      syslog(LOG_INFO, "writing result");
      std::string str = buf;
      std::cout << "answer " << i << ": " << str << std::endl;
    }
  }
}

int main(void) {
  openlog("magic ball", LOG_PID | LOG_CONS, LOG_USER);
  Conn::s_initConn();

  for(int i = 0; i < CLIENTS_COUNT; i++) {
    int pid = fork();
    if (pid == 0) {
      syslog(LOG_INFO, "forked succesfully");
      Conn conn(i, false);
      Client client(conn);
      client.exec();
      syslog(LOG_INFO, "exiting child");
      closelog();
      exit(EXIT_SUCCESS);
    } else if (pid < 0) {
      syslog(LOG_INFO, "fork error");
      exit(EXIT_FAILURE);
    }
  }
  syslog(LOG_INFO, "in host process");
  std::vector<Conn*> connHost(CLIENTS_COUNT);
  for (int i = 0; i < CLIENTS_COUNT; i++) {
    connHost[i] = new Conn(i, true);
  }
  syslog(LOG_INFO, "host conn created");
  Host host(connHost);
  host.exec();
  for (int i = 0; i < CLIENTS_COUNT; i++) {
    delete connHost[i];
  }
  closelog();
  return EXIT_SUCCESS;
}
