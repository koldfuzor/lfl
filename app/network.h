/*
 * $Id: network.h 1335 2014-12-02 04:13:46Z justin $
 * Copyright (C) 2009 Lucid Fusion Labs

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LFL_CORE_APP_NETWORK_H__
#define LFL_CORE_APP_NETWORK_H__

namespace LFL {
DECLARE_bool(network_debug);
extern const Socket InvalidSocket;

struct SocketType { static const int Stream, Datagram, SeqPacket, Raw; };
struct Protocol { 
  enum { TCP=1, UDP=2, UNIX=3, GPLUS=4 };
  static const char *Name(int p);
};

struct Ethernet {
  UNALIGNED_struct Header {
    static const int Size = 14, AddrSize = 6;
    unsigned char dst[AddrSize], src[AddrSize];
    unsigned short type;
  }; UNALIGNED_END(Header, Header::Size);
};

struct IPV4 {
  typedef unsigned Addr;
  static const Addr ANY;

  UNALIGNED_struct Header {
    static const int MinSize = 20;
    unsigned char vhl, tos;
    unsigned short len, id, off;
    unsigned char ttl, prot;
    unsigned short checksum;
    unsigned int src, dst;
    int version() const { return vhl >> 4; }
    int hdrlen() const { return (vhl & 0x0f); }
  }; UNALIGNED_END(Header, Header::MinSize);

  static Addr Parse(const string &ip);
  static void ParseCSV(const string &text, vector<Addr> *out);
  static void ParseCSV(const string &text, set<Addr> *out);
  static string MakeCSV(const vector<Addr> &in);
  static string MakeCSV(const set<Addr> &in);
  static string Text(Addr addr)           { return StringPrintf("%u.%u.%u.%u",    addr&0xff, (addr>>8)&0xff, (addr>>16)&0xff, (addr>>24)&0xff); }
  static string Text(Addr addr, int port) { return StringPrintf("%u.%u.%u.%u:%u", addr&0xff, (addr>>8)&0xff, (addr>>16)&0xff, (addr>>24)&0xff, port); }
};

struct TCP {
  UNALIGNED_struct Header {
    static const int MinSize = 20;
    unsigned short src, dst;
    unsigned int seqn, ackn;
#ifdef LFL_BIG_ENDIAN
    unsigned char offx2, fin:1, syn:1, rst:1, push:1, ack:1, urg:1, exe:1, cwr:1;
#else
    unsigned char offx2, cwr:1, exe:1, urg:1, ack:1, push:1, rst:1, syn:1, fin:1;
#endif
    unsigned short win, checksum, urgp;
    int offset() const { return offx2 >> 4; }
  }; UNALIGNED_END(Header, Header::MinSize);
};

struct UDP {
  UNALIGNED_struct Header {
    static const int Size = 8;
    unsigned short src, dst, len, checksum;
  }; UNALIGNED_END(Header, Header::Size);
};

struct IPV4Endpoint {
  IPV4::Addr addr=0;
  int port=0;
  IPV4Endpoint() {}
  IPV4Endpoint(int A, int P) : addr(A), port(P) {};
  string name() const { return IPV4::Text(addr, port); }
  string ToString() const { string s; s.resize(sizeof(*this)); memcpy(&s[0], this, sizeof(*this)); return s; }
  static const IPV4Endpoint *FromString(const char *s) { return reinterpret_cast<const IPV4Endpoint*>(s); }
};

struct IPV4EndpointSource { 
  virtual bool Available() const { return true; }
  virtual void Get(IPV4::Addr *addr, int *port) = 0;
  virtual void Get(IPV4::Addr  addr, int *port) = 0;
  virtual void Close(IPV4::Addr addr, int port) = 0;
  virtual void BindFailed(IPV4::Addr addr, int port) {}
};

struct SingleIPV4Endpoint : public IPV4EndpointSource {
  IPV4Endpoint val;
  SingleIPV4Endpoint(IPV4::Addr A, int P) : val(A, P) {}
  void Get(IPV4::Addr *A, int *P) { *A=val.addr; *P=val.port; }
  void Get(IPV4::Addr  A, int *P) { if (A == val.addr) *P=val.port; }
  void Close(IPV4::Addr A, int P) { CHECK_EQ(A, val.addr); CHECK_EQ(P, val.port); }
};

struct IPV4EndpointPool : public IPV4EndpointSource {
  static const int ports_per_ip = 65536-1024, bytes_per_ip = ports_per_ip/8;
  vector<IPV4::Addr> source_addrs;
  vector<string> source_ports;
  IPV4EndpointPool(const string &ip_csv);
  bool Available() const;
  void Close(IPV4::Addr addr, int port);
  void Get(IPV4::Addr *addr, int *port); 
  void Get(IPV4::Addr addr, int *port);
  bool GetPort(int ind, int *port);
};

struct IPV4EndpointPoolFilter : public IPV4EndpointSource {
  set<IPV4::Addr> filter;
  IPV4EndpointPool *wrap;
  IPV4EndpointPoolFilter(IPV4EndpointPool *W) : wrap(W) {}
  bool Available() const { return wrap->Available(); }
  void Close(IPV4::Addr addr, int port) { return wrap->Close(addr, port); }
  void Get(IPV4::Addr addr, int *port) { return wrap->Get(addr, port); }
  bool GetPort(int ind, int *port) { return wrap->GetPort(ind, port); }
  void Get(IPV4::Addr *addr, int *port);
};

struct SystemNetwork {
  static void CloseSocket(Socket);
  static Socket OpenSocket(int protocol);
  static bool OpenSocketPair(Socket *fd_out, int sock_type=SocketType::Stream, bool close_on_exec=true);
  static int SetSocketBlocking(Socket fd, int blocking);
  static int SetSocketCloseOnExec(Socket fd, int close);
  static int SetSocketBroadcastEnabled(Socket fd, int enabled);
  static int SetSocketBufferSize(Socket fd, bool send_or_recv, int size);
  static int GetSocketBufferSize(Socket fd, bool send_or_recv);

  static int Bind(int fd, IPV4::Addr addr, int port);
  static Socket Accept(Socket listener, IPV4::Addr *addr, int *port);
  static Socket Listen(int protocol, IPV4::Addr addr, int port, int backlog=32, bool blocking=false);
  static int Connect(Socket fd, IPV4::Addr addr, int port, int *connected);
  static int SendTo(Socket fd, IPV4::Addr addr, int port, const char *buf, int len);
  static int GetSockName(Socket fd, IPV4::Addr *addr_out, int *port_out);
  static int GetPeerName(Socket fd, IPV4::Addr *addr_out, int *port_out);
  static string GetHostByAddr(IPV4::Addr addr);
  static IPV4::Addr GetHostByName(const string &host);
  static int IOVLen(const iovec*, int len);
  static bool EWouldBlock();
  static string LastError();
};

struct TransferredSocket { Socket socket; int offset; };

struct SocketSet {
  enum { READABLE=1, WRITABLE=2, EXCEPTION=4 };
  virtual ~SocketSet() {}
  virtual void Del(Socket fd) = 0;
  virtual void Add(Socket fd, int flag, void *val) = 0;
  virtual void Set(Socket fd, int flag, void *val) = 0;
  virtual int GetReadable(Socket fd) { return 0; };
  virtual int GetWritable(Socket fd) { return 0; };
  virtual int GetException(Socket fd) { return 0; };
  virtual int Select(int wait_time) { return 0; };
};

struct SelectSocketSet : public SocketSet {
  unordered_map<Socket, pair<int, void*> > socket;
  fd_set rfds, wfds, xfds;
  SocketSet *mirror=0;

  int Select(int wait_time);
  void Del(Socket fd)                    { socket.erase(fd);                if (mirror) mirror->Del(fd); }
  void Add(Socket fd, int flag, void *v) { socket[fd] = make_pair(flag, v); if (mirror) mirror->Add(fd, flag, v); }
  void Set(Socket fd, int flag, void *v) { socket[fd] = make_pair(flag, v); if (mirror) mirror->Set(fd, flag, v); }
  int Get(Socket fd, fd_set *set) { return FD_ISSET(fd, set); } 
  int GetReadable(Socket fd) { return Get(fd, &rfds); }
  int GetWritable(Socket fd) { return Get(fd, &wfds); }
  int GetException(Socket fd) { return Get(fd, &xfds); }
  string DebugString() const {
    string ret="SelectSocketSet={";
    for (auto &s : socket) StrAppend(&ret, s.first, ", ");
    return StrCat(ret.substr(0, ret.size()-2), "}");
  }
};

#if defined(LFL_WFMO) && defined(LFL_WINDOWS)
struct WFMOSocketSet : public SocketSet {
  static const int max_sockets = MAXIMUM_WAIT_OBJECTS;
  struct Event { struct Data { void *ptr=0; } data; } events[1];
  SortedVector<HANDLE> sockets;
  int num_events=0, cur_event=0;
  Socket cur_fd=-1;
  WFMOSocketSet() { sockets.reserve(max_sockets); }

  int Select(int wait_time) {
    int ret = WaitForMultipleObjects(sockets.size(), sockets.data(), FALSE, ToMilliseconds(wait_time));
    if (ret == WAIT_FAILED) return ERRORv(-1, "WFMO ", GetLastError());
    num_events = ret != WAIT_TIMEOUT;
    return 0;
  }
  void Del(Socket fd) {}
  void Add(Socket fd, int flag, void *val) {}
  void Set(Socket fd, int flag, void *val) {}
  int GetReadable(Socket fd) { return 0; }
  int GetWritable(Socket fd) { return 0; }
  int GetException(Socket fd) { return 0; }
};
typedef WFMOSocketSet LFLSocketSet;

#elif defined(LFL_EPOLL) && defined(LFL_LINUX_SERVER)
#include <sys/epoll.h>
template <int S> struct EPollSocketSet : public SocketSet {
  Socket epollfd, cur_fd;
  epoll_event events[S];
  int cur_event=-1, num_events=0;
  EPollSocketSet() : epollfd(epoll_create(S)) {}
  virtual ~EPollSocketSet() { close(epollfd); }

  int Select(int wait_time) {
    if ((num_events = epoll_wait(epollfd, events, S, ToMilliseconds(wait_time))) == -1) ERROR("epoll_wait() ", strerror(errno));
    return 0;
  }
  void Change(Socket fd, int op, int flag, void *val) {
    epoll_event ev;
    memzero(ev);
    ev.data.ptr = val;
    ev.events = ((flag & READABLE) ? EPOLLIN : 0) | ((flag & WRITABLE) ? EPOLLOUT : 0);
    if (epoll_ctl(epollfd, op, fd, &ev) == -1) ERROR("epoll_ctl(", epollfd, ", ", op, ", ", events, "): ", strerror(errno)); 
  }
  void Del(Socket fd)                      { Change(fd, EPOLL_CTL_DEL, READABLE|WRITABLE, 0);   }
  void Add(Socket fd, int flag, void *val) { Change(fd, EPOLL_CTL_ADD, flag,              val); }
  void Set(Socket fd, int flag, void *val) { Change(fd, EPOLL_CTL_MOD, flag,              val); }
  int GetReadable(Socket fd) { return Get(fd)->events & (EPOLLIN  | EPOLLERR | EPOLLHUP); }
  int GetWritable(Socket fd) { return Get(fd)->events & (EPOLLOUT | EPOLLERR | EPOLLHUP); }
  int GetException(Socket fd) { return 0; }
  epoll_event *Get(Socket fd) {
    CHECK_EQ(fd, cur_fd);
    CHECK(cur_event >= 0 && cur_event < S);
    return &events[cur_event];
  }
};
typedef EPollSocketSet<65536 * 6> LFLSocketSet;

#else
#define LFL_NETWORK_MONOLITHIC_FRAME
typedef SelectSocketSet LFLSocketSet;
#endif

/// SocketWakeupThread waits on SocketSet and calls app->scheduler.Wakeup() on event
struct SocketWakeupThread : public SocketSet {
  mutex *frame_mutex, *wait_mutex;
  SelectSocketSet sockets;
  mutex sockets_mutex;
  Thread thread;
  Socket pipe[2];
  bool wakeup_each=0;
  ~SocketWakeupThread();
  SocketWakeupThread(mutex *FM=0, mutex *WM=0) : frame_mutex(FM), wait_mutex(WM),
  thread(bind(&SocketWakeupThread::ThreadProc, this)) { pipe[0] = pipe[1] = -1; }

  void Add(Socket s, int f, void *v) { { ScopedMutex m(sockets_mutex); sockets.Add(s, f, v); } Wakeup(); }
  void Set(Socket s, int f, void *v) { { ScopedMutex m(sockets_mutex); sockets.Set(s, f, v); } Wakeup(); }
  void Del(Socket s)                 { { ScopedMutex m(sockets_mutex); sockets.Del(s);       } Wakeup(); }
  void Start();
  void Wait() { Wakeup(); thread.Wait(); }
  void Wakeup();
  void ThreadProc();
};

struct SSLSocket {
  SSL *ssl=0;
  BIO *bio=0;
  ~SSLSocket();
  const char *ErrorString() const;
  Socket GetSocket() const;
  ssize_t Read(char *buf, int readlen);
  ssize_t Write(const StringPiece &b);
  Socket Listen(int port, bool reuse);
  Socket Connect(SSL_CTX *sslctx, const string &hostport);
  Socket Connect(SSL_CTX *sslctx, IPV4::Addr addr, int port);
  Socket Accept(SSLSocket *out);
  static SSL_CTX *Init();
  static void Free();
};

struct Listener {
  bool ssl;
  Socket socket = InvalidSocket;
  Service *svc;
  SSLSocket bio;
  typed_ptr self_reference;
  Listener(Service *s, bool SSL=false) : ssl(SSL), svc(s), self_reference(MakeTyped(this)) {}
};

struct Connection {
  enum { Connected=1, Connecting=2, Reconnect=3, Error=5 };
  typedef function<void(Connection*)> CB;
  struct Handler {  
    virtual ~Handler() {}
    virtual int Heartbeat(Connection *c) { return 0; }
    virtual int Connected(Connection *c) { return 0; }
    virtual int Read(Connection *c) { return 0; }    
    virtual int Flushed(Connection *c) { return 0; }
    virtual void Close(Connection *c) {}
  };
  struct CallbackHandler : public Handler {
    CB heartbeat_cb, connected_cb, read_cb, flushed_cb, close_cb;
    CallbackHandler(const CB &R, const CB &C) : read_cb(R), close_cb(C) {}
    virtual int Heartbeat(Connection *c) { if (heartbeat_cb) heartbeat_cb(c); return 0; }
    virtual int Connected(Connection *c) { if (connected_cb) connected_cb(c); return 0; }
    virtual int Read(Connection *c) { if (read_cb) read_cb(c); return 0; }    
    virtual int Flushed(Connection *c) { if (flushed_cb) flushed_cb(c); return 0; }
    virtual void Close(Connection *c) { if (close_cb) close_cb(c); }
  };

  Service *svc;
  Socket socket;
  Time ct, rt, wt;
  string endpoint_name;
  IPV4::Addr addr, src_addr=0;
  bool readable=1, writable=0, control_messages=0;
  int state, port, src_port=0;
  StringBuffer rb, wb;
  typed_ptr self_reference;
  vector<IOVec> packets;
  deque<TransferredSocket> transferred_socket;
  unique_ptr<Handler> handler;
  Callback *detach;
  SSLSocket bio;

  virtual ~Connection();
  Connection(Service *s, Handler *h,                                     Callback *Detach=0) : svc(s), socket(-1),   ct(Now()), rt(Now()), wt(Now()), addr(0),    state(Error), port(0),    rb(65536), wb(65536), self_reference(MakeTyped(this)), handler(h), detach(Detach) {}
  Connection(Service *s, int State, int Sock,                            Callback *Detach=0) : svc(s), socket(Sock), ct(Now()), rt(Now()), wt(Now()), addr(0),    state(State), port(0),    rb(65536), wb(65536), self_reference(MakeTyped(this)),             detach(Detach) {}
  Connection(Service *s, int State, int Sock, IPV4::Addr Addr, int Port, Callback *Detach=0) : svc(s), socket(Sock), ct(Now()), rt(Now()), wt(Now()), addr(Addr), state(State), port(Port), rb(65536), wb(65536), self_reference(MakeTyped(this)),             detach(Detach) {}
  Connection(Service *s, int State,           IPV4::Addr Addr, int Port, Callback *Detach=0) : svc(s), socket(-1),   ct(Now()), rt(Now()), wt(Now()), addr(Addr), state(State), port(Port), rb(65536), wb(65536), self_reference(MakeTyped(this)),             detach(Detach) {}

  string Name() const { return !endpoint_name.empty() ? endpoint_name : IPV4::Text(addr, port); }
  void SetError() { state = Error; ct = Now(); }
  void SetConnected() { state = Connected; ct = Now(); }
  void SetReconnect() { state = Reconnect; ct = Now(); }
  void SetConnecting() { state = Connecting; ct = Now(); }
  int SetSourceAddress() { return SystemNetwork::GetSockName(socket, &src_addr, &src_port); }
  int Write(const string &buf) { return Write(buf.c_str(), buf.size()); }
  int Write(const char *buf, int len);
  int WriteFlush();
  int WriteFlush(const string &buf) { return WriteFlush(buf.c_str(), buf.size()); }
  int WriteFlush(const char *buf, int len);
  int WriteFlush(const char *buf, int len, int transfer_socket);
  int WriteVFlush(const iovec *iov, int len);
  int WriteVFlush(const iovec *iov, int len, int transfer_socket);
  int SendTo(const char *buf, int len);
  int Read();
  int Reads();
  int ReadPacket();
  int ReadPackets();
  int Add(const char *buf, int len);
  int AddPacket(const char *buf, int len);
  int ReadFlush(int len);
};

struct Service {
  string name;
  int protocol, reconnect=0;
  bool initialized=0, heartbeats=0, endpoint_read_autoconnect=0;
  void *game_network=0;
  map<string, unique_ptr<Listener>> listen;
  map<Socket, unique_ptr<Connection>> conn;
  map<string, unique_ptr<Connection>> endpoint;
  IPV4EndpointSource *connect_src_pool=0;
  Connection fake;
  Service(int prot=Protocol::TCP) : protocol(prot), fake(this, Connection::Connected, 0) {}

  void QueueListen(IPV4::Addr addr, int port, bool SSL=false) { QueueListen(IPV4Endpoint(addr,port).ToString(), SSL); }
  void QueueListen(const string &n, bool SSL=false) { listen[n] = make_unique<Listener>(this, SSL); }
  Listener *GetListener() { return listen.size() ? listen.begin()->second.get() : 0; }

  int OpenSocket(Connection *c, int protocol, int blocking, IPV4EndpointSource*);
  Socket Listen(IPV4::Addr addr, int port, Listener*);
  Connection *Accept(int state, Socket socket, IPV4::Addr addr, int port);
  Connection *Connect(IPV4::Addr addr, int port, IPV4EndpointSource *src_addr=0, Callback *detach=0);
  Connection *Connect(IPV4::Addr addr, int port, IPV4::Addr src_addr, int src_port, Callback *detach=0);
  Connection *Connect(const string &hostport, int default_port=0, Callback *detach=0);
  Connection *SSLConnect(SSL_CTX *sslctx, IPV4::Addr addr, int port, Callback *detach=0);
  Connection *SSLConnect(SSL_CTX *sslctx, const string &hostport, int default_port=0, Callback *detach=0);
  Connection *AddConnectedSocket(Socket socket, Connection::Handler*);
  Connection *EndpointConnect(const string &endpoint_name);
  void EndpointReadCB(string *endpoint_name, string *packet);
  void EndpointRead(const string &endpoint_name, const char *buf, int len);
  void EndpointClose(const string &endpoint_name);
  void Detach(Connection *c);

  virtual void Close(Connection *c);
  virtual int UDPFilter(Connection *e, const char *buf, int len) { return 0; }
  virtual int Connected(Connection *c) { return 0; }
  virtual int Frame() { return 0; }
};

struct ServiceEndpointEraseList {
  vector<pair<Service*, Socket> > sockets;
  vector<pair<Service*, string> > endpoints;
  void AddSocket  (Service *s, Socket fd)        { sockets  .emplace_back(s, fd); }
  void AddEndpoint(Service *s, const string &en) { endpoints.emplace_back(s, en); }
  void Erase() {
    for (auto &r : endpoints) r.first->endpoint.erase(r.second);
    for (auto &r : sockets)   r.first->conn    .erase(r.second);
    sockets  .clear();
    endpoints.clear();
  }
};

struct UnixClient : public Service {
  UnixClient() : Service(Protocol::UNIX) {}
};

struct UnixServer : public Service {
  UnixServer(const string &n) : Service(Protocol::UNIX) { QueueListen(n); }
};

struct UDPClient : public Service {
  static const int MTU = 1500;
  enum { Write=1, Sendto=2 };
  typedef function<void(Connection*, const char*, int)> ResponseCB;
  typedef function<void(Connection*)> HeartbeatCB; 
  struct PersistentConnectionHandler : public Connection::Handler {
    UDPClient::ResponseCB responseCB;
    UDPClient::HeartbeatCB heartbeatCB;
    PersistentConnectionHandler(const UDPClient::ResponseCB &RCB, const UDPClient::HeartbeatCB &HCB) : responseCB(RCB), heartbeatCB(HCB) {}
    int Heartbeat(Connection *c) { if (heartbeatCB) heartbeatCB(c); return 0; }
    void Close(Connection *c) { if (responseCB) responseCB(c, 0, 0); }
    int Read(Connection *c);
  };
  UDPClient() : Service(Protocol::UDP) { heartbeats=true; }
  Connection *PersistentConnection(const string &url, const ResponseCB& cb, int default_port) { return PersistentConnection(url, cb, HeartbeatCB(), default_port); }
  Connection *PersistentConnection(const string &url, const ResponseCB&, const HeartbeatCB&, int default_port);
};

struct UDPServer : public Service {
  virtual ~UDPServer() {}
  Connection::Handler *handler=0;
  UDPServer(int port) { protocol=Protocol::UDP; QueueListen(0, port); }
  virtual int Connected(Connection *c) { c->handler = unique_ptr<Connection::Handler>(handler); return 0; }
};

struct TCPClient : public Service {};
struct TCPServer : public Service {
  Connection::Handler *handler=0;
  TCPServer(int port) { QueueListen(0, port); }
  virtual int Connected(Connection *c) { c->handler = unique_ptr<Connection::Handler>(handler); return 0; }
  virtual void Close(Connection *c) { c->handler.release(); Service::Close(c); }
};

struct GPlusClient : public Service {
  static const int MTU = 1500;
  enum { Write=1, Sendto=2 };
  GPlusClient() : Service(Protocol::GPLUS) { heartbeats=true; }
  Connection *PersistentConnection(const string &name, UDPClient::ResponseCB cb, UDPClient::HeartbeatCB HCB);
};

struct GPlusServer : public Service {
  virtual ~GPlusServer() {}
  Connection::Handler *handler=0;
  GPlusServer() : Service(Protocol::GPLUS) { endpoint_read_autoconnect=1; }
  virtual int Connected(Connection *c) { c->handler = unique_ptr<Connection::Handler>(handler); return 0; }
};

struct Network : public Module {
  int select_time=0;
  LFLSocketSet active;
  vector<Service*> service_table;
  unique_ptr<UnixClient> unix_client;
  unique_ptr<UnixServer> unix_server;
  unique_ptr<UDPClient> udp_client;
  unique_ptr<UDPServer> udp_server;
  unique_ptr<TCPClient> tcp_client;
  unique_ptr<GPlusClient> gplus_client;
  unique_ptr<GPlusServer> gplus_server;
  unique_ptr<SystemResolver> system_resolver;
  unique_ptr<RecursiveResolver> recursive_resolver;
  SSL_CTX *ssl=0;
  Network();
  virtual ~Network();

  int Init();
  int Enable(Service *svc);
  int Disable(Service *svc);
  int Shutdown(Service *svc);
  int Enable(const vector<Service*> &svc);
  int Disable(const vector<Service*> &svc);
  int Shutdown(const vector<Service*> &svc);
  int Frame(unsigned);
  void AcceptFrame(Service *svc, Listener *listener);
  void TCPConnectionFrame(Service *svc, Connection *c, ServiceEndpointEraseList *removelist);
  void UDPConnectionFrame(Service *svc, Connection *c, ServiceEndpointEraseList *removelist, const string &epk);

  void ConnClose(Service *svc, Connection *c, ServiceEndpointEraseList *removelist);
  void ConnCloseDetached(Service *svc, Connection *c);
  void ConnCloseAll(Service *svc);

  void EndpointRead(Service *svc, const char *name, const char *buf, int len);
  void EndpointClose(Service *svc, Connection *c, ServiceEndpointEraseList *removelist, const string &epk);
  void EndpointCloseAll(Service *svc);

  void UpdateActive(Connection *c);
};

/// NetworkThread runs the Network Module in a new thread with a multiplexed Callback queue
struct NetworkThread {
  struct ConnectionHandler : public Connection::Handler {
    void HandleMessage(Callback *cb) { (*cb)(); delete cb; }
    int Read(Connection *c);
  };

  bool init=0;
  Network *net=0;
  Connection *rd=0;
  unique_ptr<Connection> wr;
  unique_ptr<Thread> thread;
  NetworkThread(Network *N, bool Init);

  void Write(Callback *x) { CHECK_EQ(sizeof(x), wr->WriteFlush(reinterpret_cast<const char*>(&x), sizeof(x))); }
  void HandleMessagesLoop() { if (init) net->Init(); while (GetLFApp()->run) { net->Frame(0); } }
};

struct Sniffer {
  typedef function<void(const char*, int, int)> CB;
  Thread thread;
  CB cb;
  int ip, mask;
  void *handle;
  Sniffer(void *H, int I, int M, CB C) : cb(C), handle(H), ip(I), mask(M) {}
  ~Sniffer() { thread.Wait(); }
  void Threadproc();
  static unique_ptr<Sniffer> Open(const string &dev, const string &filter, int snaplen, CB cb);
  static void PrintDevices(vector<string> *out);
  static void GetDeviceAddressSet(set<IPV4::Addr> *out);
  static void GetIPAddress(IPV4::Addr *out);
  static void GetBroadcastAddress(IPV4::Addr *out);
};

struct GeoResolution {
  void *impl;
  GeoResolution(void *I) : impl(I) {}
  bool Resolve(const string &addr, string *country, string *region, string *city, float *lat, float *lng);
  static unique_ptr<GeoResolution> Open(const string &db);
};

int NBRead(Socket fd, char *buf, int size, int timeout=0);
int NBRead(Socket fd, string *buf, int timeout=0);
string NBRead(Socket fd, int size, int timeout=0);
bool NBReadable(Socket fd, int timeout=0);
bool NBFGets(FILE*, char *buf, int size, int timeout=0);
bool FGets(char *buf, int size);
string PromptFGets(const string &p, int s=32);

}; // namespace LFL

#include "net/http.h"

#endif // LFL_CORE_APP_NETWORK_H__
