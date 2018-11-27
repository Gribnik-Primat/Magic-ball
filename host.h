#ifndef HOST_H
#define HOST_H
#include <syslog.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "conn.h"
#include "client.h"

class Host {
public:
  Host(std::vector<Conn*>& conn);
  void exec();
private:
  std::vector<Conn*> m_conn;
};
#endif //HOST_H
