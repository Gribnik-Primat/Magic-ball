#include "conn.h"

class ConnPipePrivate : public ConnPrivate {
public:
  static int m_fdH2C[CLIENTS_COUNT * 2];
  static int m_fdC2H[CLIENTS_COUNT * 2];
};

int m_fdH2C[CLIENTS_COUNT * 2] = { 0 };
int m_fdC2H[CLIENTS_COUNT * 2] = { 0 };

Conn::Conn(int id, bool isInHost) : m_id(id), m_isInHost(isInHost) {
}

Conn::~Conn() {
  if (m_isInHost) {
    close(m_fdC2H[2 * m_id]);
  } else {
    close(m_fdH2C[2 * m_id + 1]);
  }
}

void Conn::s_initConn() {
  for (int i = 0; i < CLIENTS_COUNT; i++) {
    if (pipe(&m_fdH2C[2 * i]) == -1) {
      syslog(LOG_ERR, "can't create IPC");
      exit(EXIT_FAILURE);
    }
    if (pipe(&m_fdC2H[2 * i]) == -1) {
      syslog(LOG_ERR, "can't create IPC");
      exit(EXIT_FAILURE);
    }
  }
}

bool Conn::Read(void* buf, size_t count) {
  if (m_isInHost) {
    close(m_fdC2H[2 * m_id + 1]);
    return read(m_fdC2H[2 * m_id], buf, count) != -1;
  } else {
    close(m_fdH2C[2 * m_id + 1]);
    return read(m_fdH2C[2 * m_id], buf, count) != -1;
  }
}

bool Conn::Write(const void* buf, size_t count) {
  if (m_isInHost) {
    close(m_fdH2C[2 * m_id]);
    return write(m_fdH2C[2 * m_id + 1], buf, count) != -1;
  } else {
    close(m_fdC2H[2 * m_id]);
    return write(m_fdC2H[2 * m_id + 1], buf, count) != -1;
  }
}
