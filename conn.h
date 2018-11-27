#ifndef CONN_H
#define CONN_H
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <mqueue.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ctime>

class ConnPrivate {
public:
  virtual ~ConnPrivate() {}
};

class Conn {
public:
  Conn(int id, bool isInHost);
  ~Conn();
  static void s_initConn();
  bool Read(void* buf, size_t count);
  bool Write(const void* buf, size_t count);
private:
  int m_id;
  bool m_isInHost;
  ConnPrivate* m_connPrivate;
};
#define MAX_MSG_SIZE 100
#define CLIENTS_COUNT 5
#endif //CONN_H
