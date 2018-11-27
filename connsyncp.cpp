#include "connsyncp.h"

ConnSyncPrivate::ConnSyncPrivate(bool isInHost, int id) {
  std::ostringstream s;
  s << id;
  std::string semName1 = "/conn_sem" + s.str() + "0";
  std::string semName2 = "/conn_sem" + s.str() + "1";
  sem_t* sem1 = sem_open(semName1.c_str(), O_CREAT, S_IRWXU | S_IRWXG, 1);
  if (sem1 == SEM_FAILED) {
    syslog(LOG_ERR, "can't open semaphore for IPC");
    exit(EXIT_FAILURE);
  } 
  sem_t* sem2 = sem_open(semName2.c_str(), O_CREAT, S_IRWXU | S_IRWXG, 1);
  if (sem2 == SEM_FAILED) {
    syslog(LOG_ERR, "can't open semaphore for IPC");
    exit(EXIT_FAILURE);
  } 
  if (isInHost) {
    m_semRead = sem1;
    m_semWrite = sem2;
  } else {
    m_semRead = sem2;
    m_semWrite = sem1;
  }
}

void ConnSyncPrivate::closeSem() {
  sem_close(m_semRead);
  sem_close(m_semWrite);
}
