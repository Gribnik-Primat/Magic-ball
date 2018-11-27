#ifndef CONNSYNCP_H
#define CONNSYNCP_H
#include "conn.h"

class ConnSyncPrivate : public ConnPrivate {
public:
  ConnSyncPrivate(bool isInHost, int id);

  void closeSem();

  sem_t* m_semRead;
  sem_t* m_semWrite;
};
#endif //CONNSYNCP_H
