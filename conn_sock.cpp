#include "conn.h"

#define PORT 20000

class ConnSockPrivate : public ConnPrivate {
public:
  int m_sockfd1, m_newsockfd, m_sockfd2, m_sock;
};

Conn::Conn(int id, bool isInHost) : m_id(id), m_isInHost(isInHost) {
  m_connPrivate = new ConnSockPrivate();
  ConnSockPrivate* connP = (ConnSockPrivate*)m_connPrivate;
  int portNum = PORT + m_id;
  socklen_t clilen = sizeof(sockaddr_in);
  sockaddr_in hostAddr, clientAddr;
  if (m_isInHost) {
    connP->m_sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (connP->m_sockfd1 < 0) {
      syslog(LOG_ERR, "can't open socket for IPC");
      exit(EXIT_FAILURE);
    }
    bzero((char*)&hostAddr, sizeof(hostAddr));
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_addr.s_addr = INADDR_ANY;
    hostAddr.sin_port = htons(portNum);

    if (bind(connP->m_sockfd1, (sockaddr*)&hostAddr, sizeof(hostAddr)) < 0) {
      syslog(LOG_ERR, "can't bind socket for IPC");
      exit(EXIT_FAILURE);
    }
    listen(connP->m_sockfd1, 1);
    syslog(LOG_ERR, "on accept");
    connP->m_newsockfd = accept(connP->m_sockfd1, (sockaddr*)&clientAddr, &clilen);
    if (connP->m_newsockfd < 0) {
      syslog(LOG_ERR, "can't accept socket for IPC");
      exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "connection established");

    connP->m_sock = connP->m_newsockfd;
  } else {
    hostent* host;
    connP->m_sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (connP->m_sockfd2 < 0) {
      syslog(LOG_ERR, "can't open socket for IPC");
      exit(EXIT_FAILURE);
    }
    host = gethostbyname("127.0.0.1");
    if (host == NULL) {
      syslog(LOG_ERR, "can't get host for IPC");
      exit(EXIT_FAILURE);
    }
    bzero((char*)&hostAddr, sizeof(hostAddr));
    hostAddr.sin_family = AF_INET;
    bcopy((char*)host->h_addr, (char*)&hostAddr.sin_addr.s_addr, host->h_length);
    hostAddr.sin_port = htons(portNum);
    clock_t startTime = clock();
    int delaySec = 5;
    clock_t endTime = delaySec * 1000 + startTime;
    while (connect(connP->m_sockfd2, (sockaddr*)&hostAddr, sizeof(hostAddr)) < 0) { 
      if (clock() >= endTime) {
        syslog(LOG_ERR, "can't connect to host for IPC");
        exit(EXIT_FAILURE);
      }
    }

    connP->m_sock = connP->m_sockfd2;
  }
}

Conn::~Conn() {
  ConnSockPrivate* connP = (ConnSockPrivate*)m_connPrivate;
  close(connP->m_sockfd1);
  close(connP->m_newsockfd);
  close(connP->m_sockfd2);
  delete m_connPrivate;
}

void Conn::s_initConn() {

}

bool Conn::Read(void* buf, size_t count) {
  ConnSockPrivate* connP = (ConnSockPrivate*)m_connPrivate;
  return read(connP->m_sock, buf, count) != -1;
}

bool Conn::Write(const void* buf, size_t count) {
  ConnSockPrivate* connP = (ConnSockPrivate*)m_connPrivate;
  return write(connP->m_sock, buf, count) != -1;
}
