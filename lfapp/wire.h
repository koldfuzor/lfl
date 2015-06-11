/*
 * $Id$
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

#ifndef __LFL_LFAPP_WIRE_H__
#define __LFL_LFAPP_WIRE_H__
namespace LFL {

struct Protocol { 
  enum { TCP=1, UDP=2, UNIX=3, GPLUS=4 }; int p;
  static const char *Name(int p);
};

struct Serializable {
  struct Stream {
    char *buf;
    int size;
    mutable int offset=0;
    mutable bool error=0;
    Stream(char *B, int S) : buf(B), size(S) {}

    virtual unsigned char  *N8()            = 0;
    virtual unsigned short *N16()           = 0;
    virtual unsigned       *N32()           = 0;
    virtual char           *Get(int size=0) = 0;
    virtual char           *End()           = 0;

    int Len() const { return size; }
    int Pos() const { return offset; };
    int Remaining() const { return size - offset; }
    int Result() const { return error ? -1 : 0; }
    const char *Start() const { return buf; }
    const char *Advance(int n=0) const { return Get(n); }
    const char           *End() const { return buf + size; }
    const unsigned char  *N8()  const { unsigned char  *ret = (unsigned char *)(buf+offset); offset += 1;   if (offset > size) { error=1; return 0; } return ret; }
    const unsigned short *N16() const { unsigned short *ret = (unsigned short*)(buf+offset); offset += 2;   if (offset > size) { error=1; return 0; } return ret; }
    const unsigned       *N32() const { unsigned       *ret = (unsigned      *)(buf+offset); offset += 4;   if (offset > size) { error=1; return 0; } return ret; }
    const char           *Get(int len=0) const { char  *ret = (char          *)(buf+offset); offset += len; if (offset > size) { error=1; return 0; } return ret; }

    void String  (const StringPiece &in) { char *v = (char*)Get(in.size());   if (v) { memcpy(v,   in.data(), in.size()); } }
    void BString (const StringPiece &in) { char *v = (char*)Get(in.size()+4); if (v) { memcpy(v+4, in.data(), in.size()); *((int*)v) = htonl(in.size()); } }
    void NTString(const StringPiece &in) { char *v = (char*)Get(in.size()+1); if (v) { memcpy(v,   in.data(), in.size()); v[in.size()]=0; } }

    void Write8 (const unsigned char  &in) { unsigned char  *v =                 N8();  if (v) *v = in; }
    void Write8 (const          char  &in) {          char  *v = (char*)         N8();  if (v) *v = in; }
    void Write16(const unsigned short &in) { unsigned short *v =                 N16(); if (v) *v = in; }
    void Write16(const          short &in) {          short *v = (short*)        N16(); if (v) *v = in; }
    void Write32(const unsigned int   &in) { unsigned int   *v =                 N32(); if (v) *v = in; }
    void Write32(const          int   &in) {          int   *v = (int*)          N32(); if (v) *v = in; }
    void Write32(const unsigned long  &in) { unsigned long  *v = (unsigned long*)N32(); if (v) *v = in; }
    void Write32(const          long  &in) {          long  *v = (long*)         N32(); if (v) *v = in; }

    void Ntohs(const unsigned short &in) { unsigned short *v =         N16(); if (v) *v = ntohs(in); }
    void Htons(const unsigned short &in) { unsigned short *v =         N16(); if (v) *v = htons(in); }
    void Ntohs(const          short &in) {          short *v = (short*)N16(); if (v) *v = ntohs(in); }
    void Htons(const          short &in) {          short *v = (short*)N16(); if (v) *v = htons(in); }
    void Ntohl(const unsigned int   &in) { unsigned int   *v =         N32(); if (v) *v = ntohl(in); }
    void Htonl(const unsigned int   &in) { unsigned int   *v =         N32(); if (v) *v = htonl(in); }
    void Ntohl(const          int   &in) {          int   *v = (int*)  N32(); if (v) *v = ntohl(in); }
    void Htonl(const          int   &in) {          int   *v = (int*)  N32(); if (v) *v = htonl(in); }

    void Htons(unsigned short *out) const { const unsigned short *v =         N16(); *out = v ? htons(*v) : 0; }
    void Ntohs(unsigned short *out) const { const unsigned short *v =         N16(); *out = v ? ntohs(*v) : 0; }
    void Htons(         short *out) const { const          short *v = (short*)N16(); *out = v ? htons(*v) : 0; }
    void Ntohs(         short *out) const { const          short *v = (short*)N16(); *out = v ? ntohs(*v) : 0; }
    void Htonl(unsigned int   *out) const { const unsigned int   *v =         N32(); *out = v ? htonl(*v) : 0; }
    void Ntohl(unsigned int   *out) const { const unsigned int   *v =         N32(); *out = v ? ntohl(*v) : 0; }
    void Htonl(         int   *out) const { const          int   *v = (int*)  N32(); *out = v ? htonl(*v) : 0; }
    void Ntohl(         int   *out) const { const          int   *v = (int*)  N32(); *out = v ? ntohl(*v) : 0; }

    void Read8 (unsigned char  *out) const { const unsigned char  *v =                 N8();  *out = v ? *v : 0; }
    void Read8 (         char  *out) const { const          char  *v = (char*)         N8();  *out = v ? *v : 0; }
    void Read16(unsigned short *out) const { const unsigned short *v =                 N16(); *out = v ? *v : 0; }
    void Read16(         short *out) const { const          short *v = (short*)        N16(); *out = v ? *v : 0; }
    void Read32(unsigned int   *out) const { const unsigned int   *v =                 N32(); *out = v ? *v : 0; }
    void Read32(         int   *out) const { const          int   *v = (int*)          N32(); *out = v ? *v : 0; }
    void Read32(unsigned long  *out) const { const unsigned long  *v = (unsigned long*)N32(); *out = v ? *v : 0; }
    void Read32(         long  *out) const { const          long  *v = (long*)         N32(); *out = v ? *v : 0; }
    void ReadString(StringPiece *out) const { Ntohl(&out->len); out->buf = Get(out->len); }
  };

  struct ConstStream : public Stream {
    ConstStream(const char *B, int S) : Stream((char*)B, S) {}
    char           *End()          { FATAL((void*)this, ": ConstStream write"); return 0; }
    unsigned char  *N8()           { FATAL((void*)this, ": ConstStream write"); return 0; }
    unsigned short *N16()          { FATAL((void*)this, ": ConstStream write"); return 0; }
    unsigned       *N32()          { FATAL((void*)this, ": ConstStream write"); return 0; }
    char           *Get(int len=0) { FATAL((void*)this, ": ConstStream write"); return 0; }
  };

  struct MutableStream : public Stream {
    MutableStream(char *B, int S) : Stream(B, S) {}
    char           *End() { return buf + size; }
    unsigned char  *N8()  { unsigned char  *ret = (unsigned char *)(buf+offset); offset += 1;   if (offset > size) { error=1; return 0; } return ret; }
    unsigned short *N16() { unsigned short *ret = (unsigned short*)(buf+offset); offset += 2;   if (offset > size) { error=1; return 0; } return ret; }
    unsigned       *N32() { unsigned       *ret = (unsigned      *)(buf+offset); offset += 4;   if (offset > size) { error=1; return 0; } return ret; }
    char           *Get(int len=0) { char  *ret = (char          *)(buf+offset); offset += len; if (offset > size) { error=1; return 0; } return ret; }
  };

  struct Header {
    static const int size = 4;
    unsigned short id, seq;

    void Out(Stream *o) const;
    void In(const Stream *i);
  };

  virtual int Type() const = 0;
  virtual int Size() const = 0;
  virtual int HeaderSize() const = 0;
  virtual int In(const Stream *i) = 0;
  virtual void Out(Stream *o) const = 0;

  virtual string ToString() const;
  virtual string ToString(unsigned short seq) const;
  virtual void ToString(string *out) const;
  virtual void ToString(string *out, unsigned short seq) const;
  virtual void ToString(char *buf, int len) const;
  virtual void ToString(char *buf, int len, unsigned short seq) const;

  bool HdrCheck(int content_len) { return content_len >= Header::size + HeaderSize(); }
  bool    Check(int content_len) { return content_len >= Header::size +       Size(); }
  bool HdrCheck(const Stream *is) { return HdrCheck(is->Len()); }
  bool    Check(const Stream *is) { return    Check(is->Len()); }
  int      Read(const Stream *is) { if (!HdrCheck(is)) return -1; return In(is); }

  template <class X> static int GetType() { return ((X*)NULL)->X::Type(); }
};

struct Ethernet {
  struct Header {
    static const int Size = 14, AddrSize = 6;
    unsigned char dst[AddrSize], src[AddrSize];
    unsigned short type;
  };
};

struct IPV4 {
  typedef unsigned Addr;
  static const Addr ANY;
  struct Header {
    static const int MinSize = 20;
    unsigned char vhl, tos;
    unsigned short len, id, off;
    unsigned char ttl, prot;
    unsigned short checksum;
    unsigned int src, dst;
    int version() const { return vhl >> 4; }
    int hdrlen() const { return (vhl & 0x0f); }
  };
  static Addr Parse(const string &ip);
  static void ParseCSV(const string &text, vector<Addr> *out);
  static void ParseCSV(const string &text, set<Addr> *out);
  static string MakeCSV(const vector<Addr> &in);
  static string MakeCSV(const set<Addr> &in);
  static string Text(Addr addr)           { return StringPrintf("%u.%u.%u.%u",    addr&0xff, (addr>>8)&0xff, (addr>>16)&0xff, (addr>>24)&0xff); }
  static string Text(Addr addr, int port) { return StringPrintf("%u.%u.%u.%u:%u", addr&0xff, (addr>>8)&0xff, (addr>>16)&0xff, (addr>>24)&0xff, port); }
};

struct TCP {
  struct Header {
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
  };
};

struct UDP {
  struct Header {
    static const int Size = 8;
    unsigned short src, dst, len, checksum;
  };
};

#undef IN
struct DNS {
  struct Header {
    unsigned short id;
#ifdef LFL_BIG_ENDIAN
    unsigned short qr:1, opcode:4, aa:1, tc:1, rd:1, ra:1, unused:1, ad:1, cd:1, rcode:4;
#else
    unsigned short rd:1, tc:1, aa:1, opcode:4, qr:1, rcode:4, cd:1, ad:1, unused:1, ra:1;
#endif
    unsigned short qdcount, ancount, nscount, arcount;
    static const int size = 12;
  };

  struct Type { enum { A=1, NS=2, MD=3, MF=4, CNAME=5, SOA=6, MB=7, MG=8, MR=9, _NULL=10, WKS=11, PTR=12, HINFO=13, MINFO=14, MX=15, TXT=16 }; };
  struct Class { enum { IN=1, CS=2, CH=3, HS=4 }; };

  struct Record {
    string question, answer; unsigned short type=0, _class=0, ttl1=0, ttl2=0, pref=0; IPV4::Addr addr=0;
    string DebugString() const { return StrCat("Q=", question, ", A=", answer.empty() ? IPV4::Text(addr) : answer); }
  };
  struct Response {
    vector<DNS::Record> Q, A, NS, E;
    string DebugString() const;
  };

  static int WriteRequest(unsigned short id, const string &querytext, unsigned short type, char *out, int len);
  static int ReadResponse(const char *buf, int len, Response *response);
  static int ReadResourceRecord(const Serializable::Stream *in, int num, vector<Record> *out);
  static int ReadString(const char *start, const char *cur, const char *end, string *out);

  typedef map<string, vector<IPV4::Addr> > AnswerMap;
  static void MakeAnswerMap(const vector<Record> &in, AnswerMap *out);
  static void MakeAnswerMap(const vector<Record> &in, const AnswerMap &qmap, int type, AnswerMap *out);
};

struct HTTP {
  static bool ParseHost(const char *host, const char *host_end, string *hostO, string *portO);
  static bool ResolveHost(const char *host, const char *host_end, IPV4::Addr *ipv4_addr, int *tcp_port, bool ssl, int defport=0);
  static bool ResolveEndpoint(const string &host, const string &port, IPV4::Addr *ipv4_addr, int *tcp_port, bool ssl, int defport=0);
  static bool ParseURL(const char *url, string *protO, string *hostO, string *portO, string *pathO);
  static bool ResolveURL(const char *url, bool *ssl, IPV4::Addr *ipv4_addr, int *tcp_port, string *host, string *path, int defport=0, string *prot=0);
  static string HostURL(const char *url);

  static int ParseRequest(char *buf, char **methodO, char **urlO, char **argsO, char **verO);
  static       char *FindHeadersStart(      char *buf);
  static       char *FindHeadersEnd  (      char *buf);
  static const char *FindHeadersEnd  (const char *buf);
  static int GetHeaderLen(const char *beg, const char *end);
  static int GetHeaderNameLen(const char *beg);
  static int GetURLArgNameLen(const char *beg);
  static string GrepHeaders(const char *headers, const char *headers_end, const string &name);
  static int    GrepHeaders(const char *headers, const char *headers_end, int num, ...);
  static int    GrepURLArgs(const char *urlargs, const char *urlargs_end, int num, ...);
  static string EncodeURL(const char *url);
};

struct SSH {
  static const int BinaryPacketHeaderSize = 6;
  static int BinaryPacketLength(const char *b, unsigned char *padding, unsigned char *id);
  static int BigNumSize(const BigNum n);
  static BigNum ReadBigNum(BigNum n, const Serializable::Stream *i);
  static void WriteBigNum(const BigNum n, Serializable::Stream *o);

  struct Key {
    enum { RSA=1, DSS=2, End=2 };
    static int Id(const string &n);
    static const char *Name(int id);
    static string PreferenceCSV();
    static bool PreferenceIntersect(const StringPiece &pref_csv, int *out);
  };
  struct KEX {
    enum { DH14_SHA1=1, DH1_SHA1=2, End=2 /* DHG_SHA256, DHG_SHA1 */ };
    static int Id(const string &n);
    static const char *Name(int id);
    static string PreferenceCSV();
    static bool PreferenceIntersect(const StringPiece &pref_csv, int *out);
  };
  struct Cipher {
    enum { AES128_CBC=1, TripDES_CBC=2, End=2 };
    static int Id(const string &n);
    static const char *Name(int id);
    static Crypto::CipherAlgo Algo(int id);
    static string PreferenceCSV();
    static bool PreferenceIntersect(const StringPiece &pref_csv, Crypto::CipherAlgo *out);
  };
  struct MAC {
    enum { MD5=1, SHA1=2, End=1 };
    static int Id(const string &n);
    static const char *Name(int id);
    static Crypto::MACAlgo Algo(int id);
    static string PreferenceCSV();
    static bool PreferenceIntersect(const StringPiece &pref_csv, Crypto::MACAlgo *out);
  };

  struct Serializable : public LFL::Serializable {
    string ToString(std::mt19937&, int block_size, unsigned *sequence_number) const;
    void ToString(string *out, std::mt19937&, int block_size) const;
    void ToString(char *buf, int len, std::mt19937&) const;
  };
  struct MSG_DISCONNECT : public Serializable {
    int reason_code=0;
    StringPiece description, language;

    int HeaderSize() const { return 4 + 2*4; }
    int Type() const { return 1; }
    int Size() const { return HeaderSize() + description.size() + language.size(); }

    int In(const Serializable::Stream *i) { i->Ntohl(&reason_code); i->ReadString(&description); i->ReadString(&language); return i->Result(); }
    void Out(Serializable::Stream *o) const {}
  };
  struct MSG_DEBUG : public Serializable {
    unsigned char always_display=0;
    StringPiece message, language;

    int HeaderSize() const { return 1 + 2*4; }
    int Type() const { return 4; }
    int Size() const { return HeaderSize() + message.size() + language.size(); }

    int In(const Serializable::Stream *i) { i->Read8(&always_display); i->ReadString(&message); i->ReadString(&language); return i->Result(); }
    void Out(Serializable::Stream *o) const {}
  };
  struct MSG_SERVICE_REQUEST : public Serializable {
    StringPiece service_name;
    MSG_SERVICE_REQUEST(const string &SN) : service_name(SN) {}

    int HeaderSize() const { return 4; }
    int Type() const { return 5; }
    int Size() const { return HeaderSize() + service_name.size(); }

    int In(const Serializable::Stream *i) { i->ReadString(&service_name); return i->Result(); }
    void Out(Serializable::Stream *o) const { o->BString(service_name); }
  };
  struct MSG_SERVICE_ACCEPT : public Serializable {
    StringPiece service_name;
    MSG_SERVICE_ACCEPT(const string &SN) : service_name(SN) {}

    int HeaderSize() const { return 4; }
    int Type() const { return 6; }
    int Size() const { return HeaderSize() + service_name.size(); }

    int In(const Serializable::Stream *i) { i->ReadString(&service_name); return i->Result(); }
    void Out(Serializable::Stream *o) const { o->BString(service_name); }
  };
  struct MSG_KEXINIT : public Serializable {
    StringPiece cookie, kex_algorithms, server_host_key_algorithms, encryption_algorithms_client_to_server,
                encryption_algorithms_server_to_client, mac_algorithms_client_to_server, mac_algorithms_server_to_client,
                compression_algorithms_client_to_server, compression_algorithms_server_to_client, languages_client_to_server,
                languages_server_to_client;
    unsigned char first_kex_packet_follows=0;
    MSG_KEXINIT() {}
    MSG_KEXINIT(const string &C, const string &KEXA, const string &KA, const string &EC, const string &ES, const string &MC,
                const string &MS, const string &CC, const string &CS, const string &LC, const string &LS) :
      cookie(C), kex_algorithms(KEXA), server_host_key_algorithms(KA), encryption_algorithms_client_to_server(EC),
      encryption_algorithms_server_to_client(ES), mac_algorithms_client_to_server(MC), mac_algorithms_server_to_client(MS),
      compression_algorithms_client_to_server(CC), compression_algorithms_server_to_client(CS), languages_client_to_server(LC),
      languages_server_to_client(LS) {}

    int HeaderSize() const { return 21 + 10*4; }
    int Type() const { return 20; }
    int Size() const;
    string DebugString() const;

    void Out(Serializable::Stream *o) const;
    int In(const Serializable::Stream *i);
  };
  struct MSG_NEWKEYS : public Serializable {
    int HeaderSize() const { return 0; }
    int Type() const { return 21; }
    int Size() const { return HeaderSize(); }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { return 0; }
  };
  struct MSG_KEXDH_INIT : public Serializable {
    BigNum e;
    MSG_KEXDH_INIT(BigNum E=0) : e(E) {}

    int HeaderSize() const { return 4; }
    int Type() const { return 30; }
    int Size() const { return HeaderSize() + BigNumSize(e); }

    void Out(Serializable::Stream *o) const { WriteBigNum(e, o); }
    int In(const Serializable::Stream *i) { return 0; }
  };
  struct MSG_KEXDH_REPLY : public Serializable {
    StringPiece k_s, h_sig;
    BigNum f;
    MSG_KEXDH_REPLY(BigNum F=0) : f(F) {}

    int HeaderSize() const { return 4*3; }
    int Type() const { return 31; }
    int Size() const { return HeaderSize() + BigNumSize(f) + k_s.size() + h_sig.size(); }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { i->ReadString(&k_s); f=ReadBigNum(f,i); i->ReadString(&h_sig); return i->Result(); }
  };
  struct MSG_USERAUTH_REQUEST : public Serializable {
    StringPiece user_name, service_name, method_name, algo_name, secret, sig;
    MSG_USERAUTH_REQUEST(const string &UN, const string &SN, const string &MN, const string &AN, const string &P, const string &S)
      : user_name(UN), service_name(SN), method_name(MN), algo_name(AN), secret(P), sig(S) {}

    int HeaderSize() const { return 4*3; }
    int Type() const { return 50; }
    int Size() const;

    void Out(Serializable::Stream *o) const;
    int In(const Serializable::Stream *i) { return 0; }
  };
  struct MSG_USERAUTH_FAILURE : public Serializable {
    StringPiece auth_left;
    unsigned char partial_success=0;

    int HeaderSize() const { return 4; }
    int Type() const { return 51; }
    int Size() const { return HeaderSize() + auth_left.size(); }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { i->ReadString(&auth_left); i->Read8(&partial_success); return i->Result(); }
  };
  struct MSG_USERAUTH_SUCCESS : public Serializable {
    int HeaderSize() const { return 0; }
    int Type() const { return 52; }
    int Size() const { return HeaderSize(); }
    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { return i->Result(); }
  };
  struct MSG_USERAUTH_INFO_REQUEST : public Serializable {
    StringPiece name, instruction, language;
    struct Prompt { StringPiece text; unsigned char echo=0; };
    vector<Prompt> prompt;

    int HeaderSize() const { return 4*3; }
    int Type() const { return 60; }
    int Size() const;

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i);
  };
  struct MSG_USERAUTH_INFO_RESPONSE : public Serializable {
    vector<StringPiece> response;
    MSG_USERAUTH_INFO_RESPONSE() {}
    MSG_USERAUTH_INFO_RESPONSE(const vector<StringPiece> &s) : response(s) {}

    int HeaderSize() const { return 4; }
    int Type() const { return 61; }
    int Size() const;

    void Out(Serializable::Stream *o) const;
    int In(const Serializable::Stream *i) { return i->Result(); }
  };
  struct MSG_CHANNEL_OPEN : public Serializable {
    StringPiece channel_type;
    int sender_channel, initial_win_size, maximum_packet_size;
    MSG_CHANNEL_OPEN(const string &CT, int SC, int IWS, int MPS) : channel_type(CT), sender_channel(SC), initial_win_size(IWS), maximum_packet_size(MPS) {}

    int HeaderSize() const { return 4*4; }
    int Type() const { return 90; }
    int Size() const { return HeaderSize() + channel_type.size(); }

    void Out(Serializable::Stream *o) const { o->BString(channel_type); o->Htonl(sender_channel); o->Htonl(initial_win_size); o->Htonl(maximum_packet_size); }
    int In(const Serializable::Stream *i) { return i->Result(); }
  };
  struct MSG_CHANNEL_OPEN_CONFIRMATION : public Serializable {
    int recipient_channel, sender_channel, initial_win_size, maximum_packet_size;

    int HeaderSize() const { return 4*4; }
    int Type() const { return 91; }
    int Size() const { return HeaderSize(); }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { i->Ntohl(&recipient_channel); i->Ntohl(&sender_channel); i->Ntohl(&initial_win_size); i->Ntohl(&maximum_packet_size); return i->Result(); }
  };
  struct MSG_CHANNEL_WINDOW_ADJUST : public Serializable {
    int recipient_channel, bytes_to_add;

    int HeaderSize() const { return 4*2; }
    int Type() const { return 93; }
    int Size() const { return HeaderSize(); }

    void Out(Serializable::Stream *o) const { o->Htonl(recipient_channel); o->Htonl(bytes_to_add); }
    int In(const Serializable::Stream *i) { i->Ntohl(&recipient_channel); i->Ntohl(&bytes_to_add); return i->Result(); }
  };
  struct MSG_CHANNEL_DATA : public Serializable {
    int recipient_channel;
    StringPiece data;
    MSG_CHANNEL_DATA() {}
    MSG_CHANNEL_DATA(int RC, const StringPiece &D) : recipient_channel(RC), data(D) {}

    int HeaderSize() const { return 4*2; }
    int Type() const { return 94; }
    int Size() const { return HeaderSize() + data.size(); }

    void Out(Serializable::Stream *o) const { o->Htonl(recipient_channel); o->BString(data); }
    int In(const Serializable::Stream *i) { i->Ntohl(&recipient_channel); i->ReadString(&data); return i->Result(); }
  };
  struct MSG_CHANNEL_REQUEST : public Serializable {
    int recipient_channel=0, width=0, height=0, pixel_width=0, pixel_height=0;
    StringPiece request_type, term, term_mode;
    unsigned char want_reply=0;
    MSG_CHANNEL_REQUEST(int RC, const string &RT, const string &V, bool WR) : recipient_channel(RC), request_type(RT), term(V), want_reply(WR) {}
    MSG_CHANNEL_REQUEST(int RC, const string &RT, const point &D, const point &PD, const string &T, const string &TM, bool WR) :
      recipient_channel(RC), width(D.x), height(D.y), pixel_width(PD.x), pixel_height(PD.y), request_type(RT), term(T), term_mode(TM), want_reply(WR) {}

    int HeaderSize() const { return 4*2 + 1; }
    int Type() const { return 98; }
    int Size() const;

    void Out(Serializable::Stream *o) const;
    int In(const Serializable::Stream *i) { return i->Result(); }
  };
  struct MSG_CHANNEL_SUCCESS : public Serializable {
    int HeaderSize() const { return 0; }
    int Type() const { return 99; }
    int Size() const { return HeaderSize(); }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { return 0; }
  };
  struct MSG_CHANNEL_FAILURE : public Serializable {
    int HeaderSize() const { return 0; }
    int Type() const { return 100; }
    int Size() const { return HeaderSize(); }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i) { return 0; }
  };
  struct DSSKey {
    StringPiece format_id;
    BigNum p, q, g, y;
    DSSKey(BigNum P, BigNum Q, BigNum G, BigNum Y) : p(P), q(Q), g(G), y(Y) {}

    int HeaderSize() const { return 4*4; }
    int Size() const { return HeaderSize() + format_id.size() + BigNumSize(p) + BigNumSize(q) + BigNumSize(g) + BigNumSize(y); }
    int Type() const { return 0; }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i);
  };
  struct DSSSignature {
    StringPiece format_id;
    BigNum r, s;
    DSSSignature(BigNum R, BigNum S) : r(R), s(S) {}

    int HeaderSize() const { return 4*2 + 7 + 20*2; }
    int Size() const { return HeaderSize(); }
    int Type() const { return 0; }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i);
  };
  struct RSAKey {
    StringPiece format_id;
    BigNum e, n;
    RSAKey(BigNum E, BigNum N) : e(E), n(N) {}

    int HeaderSize() const { return 2*4; }
    int Size() const { return HeaderSize() + format_id.size() + BigNumSize(e) + BigNumSize(n); }
    int Type() const { return 0; }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i);
  };
  struct RSASignature {
    StringPiece format_id, sig;

    int HeaderSize() const { return 4*2 + 7; }
    int Size() const { return HeaderSize() + sig.size(); }
    int Type() const { return 0; }

    void Out(Serializable::Stream *o) const {}
    int In(const Serializable::Stream *i);
  };
};

struct SMTP {
  struct Message {
    string mail_from, content;
    vector<string> rcpt_to;
    void clear() { mail_from.clear(); content.clear(); rcpt_to.clear(); }
  };
  static void HTMLMessage(const string& from, const string& to, const string& subject, const string& content, string *out);
  static void NativeSendmail(const string &message);
  static string EmailFrom(const string &message);
  static int SuccessCode  (int code) { return code == 250; }
  static int RetryableCode(int code) {
    return !code || code == 221 || code == 421 || code == 450 || code == 451 || code == 500 || code == 501 || code == 502 || code == 503;
  }
};

struct InterProcessProtocol {
  struct Header : public Serializable::Header {};
  struct LoadResourceRequest : public Serializable {
    unsigned short ipr_type;
    int ipr_len;
    string ipr_url;
    LoadResourceRequest(int t=0, const string &u=string(), int l=0) : ipr_type(t), ipr_len(l), ipr_url(u) {}

    int Type() const { return 1; }
    int Size() const { return HeaderSize() + ipr_url.size(); }
    int HeaderSize() const { return 8; }

    void Out(Serializable::Stream *o) const { o->Htons(ipr_type); o->Htons((unsigned short)ipr_url.size()); o->Htonl(ipr_len); o->String(ipr_url); }
    int   In(const Serializable::Stream *i) { unsigned short l; i->Ntohs(&ipr_type); i->Ntohs(&l); i->Ntohl(&ipr_len); ipr_url.assign(i->Get(l), l); return 0; }
  };
  struct LoadResourceResponse : public Serializable {
    unsigned short ipr_type;
    int ipr_len;
    string ipr_url;
    LoadResourceResponse(int t=0, const string &u=string(), int l=0) : ipr_type(t), ipr_len(l), ipr_url(u) {}

    int Type() const { return 2; }
    int Size() const { return HeaderSize() + ipr_url.size(); }
    int HeaderSize() const { return 8; }

    void Out(Serializable::Stream *o) const { o->Htons(ipr_type); o->Htons((unsigned short)ipr_url.size()); o->Htonl(ipr_len); o->String(ipr_url); }
    int   In(const Serializable::Stream *i) { unsigned short l; i->Ntohs(&ipr_type); i->Ntohs(&l); i->Ntohl(&ipr_len); ipr_url.assign(i->Get(l), l); return 0; }
  };
  struct ContentResource : public Serializable {
    StringPiece buf, name, type;
    ContentResource() {}
    ContentResource(const string &b, const string &n, const string &t) : buf(b), name(n), type(t) {}

    int Type() const { return 1<<11 | 1; }
    int Size() const { return HeaderSize() + 3 + buf.size() + name.size() + type.size(); }
    int HeaderSize() const { return sizeof(int) * 3; }

    void Out(Serializable::Stream *o) const {
      o->Htonl   (buf.size()); o->Htonl   (name.size()); o->Htonl   (type.size());
      o->NTString(buf);        o->NTString(name);        o->NTString(type);
    }
    int In(const Serializable::Stream *i) {
      /**/      i->Ntohl(&buf.len); /**/         i->Ntohl(&name.len); /**/         i->Ntohl(&type.len);
      buf.buf = i->Get  ( buf.len+1); name.buf = i->Get  ( name.len+1); type.buf = i->Get  ( type.len+1);
      return 0;
    }
  };
  struct TextureResource : public Serializable {
    int width, height, pf, linesize;
    StringPiece buf;
    TextureResource() : width(0), height(0), pf(0), linesize(0) {}
    TextureResource(const Texture &t) : width(t.width), height(t.height), pf(t.pf), linesize(t.LineSize()),
    buf(reinterpret_cast<const char *>(t.buf), t.BufferSize()) {}

    int Type() const { return 1<<11 | 2; }
    int Size() const { return HeaderSize() + buf.size(); }
    int HeaderSize() const { return sizeof(int) * 4; }

    void Out(Serializable::Stream *o) const {
      CHECK_EQ(linesize * height, buf.len);
      o->Htonl(width); o->Htonl(height); o->Htonl(pf); o->Htonl(linesize);
      o->String(buf);
    }
    int In(const Serializable::Stream *i) {
      i->Ntohl(&width); i->Ntohl(&height); i->Ntohl(&pf); i->Ntohl(&linesize);
      buf.buf = i->Get((buf.len = linesize * height));
      return 0;
    }
  };
};

struct GameProtocol {
  struct Header : public Serializable::Header {};
  struct Position {
    static const int size = 12, scale = 1000;
    int x, y, z;

    void From(const v3 &v) { x=(int)(v.x*scale); y=(int)(v.y*scale); z=(int)(v.z*scale); }
    void To(v3 *v) { v->x=(float)x/scale; v->y=(float)y/scale; v->z=(float)z/scale; }
    void Out(Serializable::Stream *o) const { o->Htonl( x); o->Htonl( y); o->Htonl( z); }
    void In(const Serializable::Stream *i)  { i->Ntohl(&x); i->Ntohl(&y); i->Ntohl(&z); }
  };
  struct Orientation {
    static const int size = 12, scale=16384;
    short ort_x, ort_y, ort_z, up_x, up_y, up_z;

    void From(const v3 &ort, const v3 &up) {
      ort_x = (short)(ort.x*scale); ort_y = (short)(ort.y*scale); ort_z = (short)(ort.z*scale);
      up_x = (short)(up.x*scale);  up_y  = (short)(up.y*scale);  up_z =  (short)(up.z*scale);
    }
    void To(v3 *ort, v3 *up) {
      ort->x = (float)ort_x/scale; ort->y = (float)ort_y/scale; ort->z = (float)ort_z/scale;
      up->x = (float) up_x/scale;  up->y = (float) up_y/scale;  up->z = (float) up_z/scale;
    }
    void Out(Serializable::Stream *o) const { o->Htons( ort_x); o->Htons( ort_y); o->Htons( ort_z); o->Htons( up_x); o->Htons( up_y); o->Htons( up_z); }
    void In(const Serializable::Stream *i)  { i->Ntohs(&ort_x); i->Ntohs(&ort_y); i->Ntohs(&ort_z); i->Ntohs(&up_x); i->Ntohs(&up_y); i->Ntohs(&up_z); }
  };
  struct Velocity {
    static const int size = 6, scale=1000;
    unsigned short x, y, z;

    void From(const v3 &v) { x=(unsigned short)(v.x*scale); y=(unsigned short)(v.y*scale); z=(unsigned short)(v.z*scale); }
    void To(v3 *v) { v->x=(float)x/scale; v->y=(float)y/scale; v->z=(float)z/scale; }
    void Out(Serializable::Stream *o) const { o->Htons( x); o->Htons( y); o->Htons( z); }
    void In(const Serializable::Stream *i)  { i->Ntohs(&x); i->Ntohs(&y); i->Ntohs(&z); }
  };
  struct Entity {
    static const int size = 8 + Position::size + Orientation::size + Velocity::size;
    unsigned short id, type, anim_id, anim_len;
    Position pos;
    Orientation ort;
    Velocity vel;

    void From(const LFL::Entity *e) { id=atoi(e->name.c_str()); type=e->asset?e->asset->typeID:0; anim_id=e->animation.id; anim_len=e->animation.len; pos.From(e->pos); ort.From(e->ort, e->up); vel.From(e->vel); }
    void Out(Serializable::Stream *o) const { o->Htons( id); o->Htons( type); o->Htons( anim_id); o->Htons( anim_len); pos.Out(o); ort.Out(o); vel.Out(o); }
    void In(const Serializable::Stream *i)  { i->Ntohs(&id); i->Ntohs(&type); i->Ntohs(&anim_id); i->Ntohs(&anim_len); pos.In(i);  ort.In(i);  vel.In(i);  }
  };
  struct Collision {
    static const int size = 8;
    unsigned short fmt, id1, id2, time;

    void Out(Serializable::Stream *o) const { o->Htons( fmt); o->Htons( id1); o->Htons( id2); o->Htons( time); }
    void In(const Serializable::Stream *i)  { i->Ntohs(&fmt); i->Ntohs(&id1); i->Ntohs(&id2); i->Ntohs(&time); }
  };
  struct ChallengeRequest : public Serializable {
    int Type() const { return 1; }
    int Size() const { return HeaderSize(); }
    int HeaderSize() const { return 0; }

    void Out(Serializable::Stream *o) const {}
    int   In(const Serializable::Stream *i) { return 0; }
  };
  struct ChallengeResponse : public Serializable {
    int token;

    int Type() const { return 2; }
    int Size() const { return HeaderSize(); }
    int HeaderSize() const { return 4; }

    void Out(Serializable::Stream *o) const { o->Htonl( token); }
    int   In(const Serializable::Stream *i) { i->Ntohl(&token); return 0; }
  };
  struct JoinRequest : public Serializable {
    int token;
    string PlayerName;

    int Type() const { return 3; }
    int Size() const { return HeaderSize() + PlayerName.size(); }
    int HeaderSize() const { return 4; }

    void Out(Serializable::Stream *o) const { o->Htonl( token); o->String(PlayerName); }
    int   In(const Serializable::Stream *i) { i->Ntohl(&token); PlayerName = i->Get(); return 0; }
  };
  struct JoinResponse : public Serializable {
    string rcon;

    int Type() const { return 4; }
    int Size() const { return rcon.size(); }
    int HeaderSize() const { return 0; }

    void Out(Serializable::Stream *o) const { o->String(rcon); }
    int   In(const Serializable::Stream *i) { rcon = i->Get(); return 0; }
  };
  struct WorldUpdate : public Serializable {
    unsigned short id;
    vector<Entity> entity;
    vector<Collision> collision;

    int Type() const { return 5; }
    int Size() const { return HeaderSize() + entity.size() * Entity::size + collision.size() * Collision::size; }
    int HeaderSize() const { return 6; }

    void Out(Serializable::Stream *o) const {
      unsigned short entities=entity.size(), collisions=collision.size();
      o->Htons(id); o->Htons(entities); o->Htons(collisions);
      for (int i=0; i<entities;   i++) entity   [i].Out(o);
      for (int i=0; i<collisions; i++) collision[i].Out(o);
    }
    int In(const Serializable::Stream *in) {
      unsigned short entities, collisions;
      in->Ntohs(&id); in->Ntohs(&entities); in->Ntohs(&collisions);
      if (!Check(in)) return -1;

      entity.resize(entities); collision.resize(collisions);
      for (int i=0; i<entities;   i++) entity[i]   .In(in);
      for (int i=0; i<collisions; i++) collision[i].In(in);
      return 0;
    }
  };
  struct PlayerUpdate : public Serializable {
    unsigned short id_WorldUpdate, time_since_WorldUpdate;
    unsigned buttons;
    Orientation ort;

    int Type() const { return 6; }
    int Size() const { return HeaderSize(); }
    int HeaderSize() const { return 8 + Orientation::size; }

    void Out(Serializable::Stream *o) const { o->Htons( id_WorldUpdate); o->Htons( time_since_WorldUpdate); o->Htonl( buttons); ort.Out(o); }
    int   In(const Serializable::Stream *i) { i->Ntohs(&id_WorldUpdate); i->Ntohs(&time_since_WorldUpdate); i->Ntohl(&buttons); ort.In(i); return 0; }
  };
  struct RconRequest : public Serializable {
    string Text;
    RconRequest(const string &t=string()) : Text(t) {}

    int Type() const { return 7; }
    int Size() const { return HeaderSize() + Text.size(); }
    int HeaderSize() const { return 0; }

    void Out(Serializable::Stream *o) const { o->String(Text); }
    int   In(const Serializable::Stream *i) { Text = i->Get(); return 0; }
  };
  struct RconResponse : public Serializable {
    int Type() const { return 8; }
    int Size() const { return HeaderSize(); }
    int HeaderSize() const { return 0; }

    void Out(Serializable::Stream *o) const {}
    int   In(const Serializable::Stream *i) { return 0; }
  };
  struct PlayerList : public RconRequest {
    PlayerList() {}
    int Type() const { return 9; }
  };
};

}; // namespace LFL
#endif // __LFL_LFAPP_WIRE_H__
