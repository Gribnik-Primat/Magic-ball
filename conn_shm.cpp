#include "connsyncp.h"

class ConnShmPrivate : public ConnSyncPrivate {
public:
  ConnShmPrivate(bool isInHost, int id) : 
    ConnSyncPrivate(isInHost, id) { }
  char* m_msgRead;
  char* m_msgWrite;
  std::string m_name;
};

char* openShFile(std::string& name) {
  int shmfd;
  int shSegSize = MAX_MSG_SIZE + 1;
  shmfd = shm_open(name.c_str(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
  if (shmfd < 0) {
      syslog(LOG_ERR, "can't open shared file for IPC");
      exit(EXIT_FAILURE);
  }
  ftruncate(shmfd, shSegSize);
  char* msg = (char*)mmap(NULL, shSegSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (msg == NULL) {
    syslog(LOG_ERR, "error in mmap");
    exit(EXIT_FAILURE);
  }
  return msg;
}

Conn::Conn(int id, bool isInHost) : m_id(id), m_isInHost(isInHost) {
  m_connPrivate = new ConnShmPrivate(isInHost, id);
  ConnShmPrivate* connP = (ConnShmPrivate*)m_connPrivate;
  std::ostringstream s;
  s << m_id;
  std::string nameH2C = "/conn_shm" + s.str() + "0";
  std::string nameC2H = "/conn_shm" + s.str() + "1";

  char* msgH2C = openShFile(nameH2C);
  char* msgC2H = openShFile(nameC2H);

  if (m_isInHost) {
    connP->m_msgRead = msgC2H;
    connP->m_msgWrite = msgH2C;
    connP->m_name = nameH2C;
  } else {
    connP->m_msgRead = msgH2C;
    connP->m_msgWrite = msgC2H;
    connP->m_name = nameC2H;
  }
}

Conn::~Conn() {
  ConnShmPrivate* connP = (ConnShmPrivate*)m_connPrivate;
  if (m_isInHost) {
    connP->closeSem();
  }
  shm_unlink((const char*)connP->m_name.c_str());
  delete m_connPrivate;
}

void Conn::s_initConn() {

}

bool Conn::Read(void* buf, size_t count) {
  ConnShmPrivate* connP = (ConnShmPrivate*)m_connPrivate;
  int n = 0;
  sem_wait(connP->m_semRead);
  n = snprintf((char*)buf, count, "%s", connP->m_msgRead);
  sem_post(connP->m_semRead);
  return n > 0;
}

bool Conn::Write(const void* buf, size_t count) {
  ConnShmPrivate* connP = (ConnShmPrivate*)m_connPrivate;
  int n = 0;
  sem_wait(connP->m_semWrite);
  n = snprintf(connP->m_msgWrite, count, "%s", (char*)buf);
  sem_post(connP->m_semWrite);
  return n > 0;
}
