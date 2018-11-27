#include "connsyncp.h"

class ConnMmapPrivate : public ConnSyncPrivate {
public:
  ConnMmapPrivate(bool isInHost, int id) : 
    ConnSyncPrivate(isInHost, id) { }
  static char* m_msgH2C[CLIENTS_COUNT];
  static char* m_msgC2H[CLIENTS_COUNT];
  char* m_msgRead;
  char* m_msgWrite;
};
char* m_msgH2C[CLIENTS_COUNT] = { NULL };
char* m_msgC2H[CLIENTS_COUNT] = { NULL };

Conn::Conn(int id, bool isInHost) : m_id(id), m_isInHost(isInHost) {
  m_connPrivate = new ConnMmapPrivate(isInHost, id);
  ConnMmapPrivate* connP = (ConnMmapPrivate*)m_connPrivate;
  if (m_isInHost) {
    connP->m_msgRead = m_msgC2H[m_id];
    connP->m_msgWrite = m_msgH2C[m_id];
  } else {
    connP->m_msgRead = m_msgH2C[m_id];
    connP->m_msgWrite = m_msgC2H[m_id];
  }
}

Conn::~Conn() {
  ConnMmapPrivate* connP = (ConnMmapPrivate*)m_connPrivate;
  if (m_isInHost) {
    connP->closeSem();
  }
  delete m_connPrivate;
}

char* anonMap() {
  int shSegSize = MAX_MSG_SIZE + 1;
  char* msg = (char*)mmap(NULL, shSegSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
  if (msg == NULL) {
    syslog(LOG_ERR, "error in mmap");
    exit(EXIT_FAILURE);
  }
  return msg;
}

void Conn::s_initConn() {
  for (int i = 0; i < CLIENTS_COUNT; i++) {
    m_msgH2C[i] = anonMap();
    m_msgC2H[i] = anonMap();
  }
}

bool Conn::Read(void* buf, size_t count) {
  ConnMmapPrivate* connP = (ConnMmapPrivate*)m_connPrivate;
  int n = 0;
  sem_wait(connP->m_semRead);
  n = snprintf((char*)buf, count, "%s", connP->m_msgRead);
  sem_post(connP->m_semRead);
  return n > 0;
}

bool Conn::Write(const void* buf, size_t count) {
  ConnMmapPrivate* connP = (ConnMmapPrivate*)m_connPrivate;
  int n = 0;
  sem_wait(connP->m_semWrite);
  n = snprintf(connP->m_msgWrite, count, "%s", (char*)buf);
  sem_post(connP->m_semWrite);
  return n > 0;
}
