#include "conn.h"

class ConnMqPrivate : public ConnPrivate {
public:
  mqd_t m_readId;
  mqd_t m_writeId;
};

mqd_t openMq(std::string& mqName) {
  mqd_t mqId;
  mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
  mqId = mq_open(mqName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
  if (mqId == -1) {
    syslog(LOG_ERR, "can't create IPC");
    exit(EXIT_FAILURE);
  }
  return mqId;
}

Conn::Conn(int id, bool isInHost) : m_id(id), m_isInHost(isInHost) {
  m_connPrivate = new ConnMqPrivate();
  ConnMqPrivate* connP = (ConnMqPrivate*)m_connPrivate;
  std::ostringstream s;
  s << m_id;
  std::string mqNameHost = "/conn_mq" + s.str() + "0";
  std::string mqNameClient = "/conn_mq" + s.str() + "1";
  mqd_t hostId = openMq(mqNameHost);
  mqd_t clientId = openMq(mqNameClient);
  if (m_isInHost) {
    connP->m_readId = hostId;
    connP->m_writeId = clientId;
  } else {
    connP->m_readId = clientId;
    connP->m_writeId = hostId;
  }
}

Conn::~Conn() {
  ConnMqPrivate* connP = (ConnMqPrivate*)m_connPrivate;
  mq_close(connP->m_readId);
  delete m_connPrivate;
}

void Conn::s_initConn() {
 
}

bool Conn::Read(void* buf, size_t count) {
  ConnMqPrivate* connP = (ConnMqPrivate*)m_connPrivate;
  return mq_receive(connP->m_readId, (char*)buf, count, NULL) != -1;
}

bool Conn::Write(const void* buf, size_t count) {
  ConnMqPrivate* connP = (ConnMqPrivate*)m_connPrivate;
  unsigned int prior = 1;
  return mq_send(connP->m_writeId, (char*)buf, count, prior) != -1;
}
