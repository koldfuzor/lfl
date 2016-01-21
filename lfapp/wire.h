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

#ifndef LFL_LFAPP_WIRE_H__
#define LFL_LFAPP_WIRE_H__

namespace LFL {
struct Protocol { 
  enum { TCP=1, UDP=2, UNIX=3, GPLUS=4 };
  static const char *Name(int p);
};

struct Serializable {
  template <class X> static void ReadType (X    *addr, const void *v) { if (addr) memcpy(addr,  v, sizeof(X)); }
  template <class X> static void WriteType(void *addr, const X    &v) { if (addr) memcpy(addr, &v, sizeof(X)); }

  struct Stream {
    char *buf;
    int size;
    mutable int offset=0;
    mutable bool error=0;
    Stream(char *B, int S) : buf(B), size(S) {}

    int Len() const { return size; }
    int Pos() const { return offset; };
    int Remaining() const { return size - offset; }
    int Result() const { return error ? -1 : 0; }
    const char *Start() const { return buf; }
    const char *End() const { return buf + size; }
    const char *Get(int len=0) const {
      const char *ret = buf + offset;
      if ((offset += len) > size) { error=1; return 0; }
      return ret;
    }
    virtual char *End()          { FATAL(Void(this), ": ConstStream write"); return 0; }
    virtual char *Get(int len=0) { FATAL(Void(this), ": ConstStream write"); return 0; }

    template <class X>
    void AString (const ArrayPiece<X> &in) { auto v = Get(in.Bytes()+sizeof(int)); if (v) { memcpy(v+4, in.ByteData(), in.Bytes()); WriteType<int>(v, htonl(in.Bytes())); } }
    void BString (const StringPiece   &in) { auto v = Get(in.size ()+sizeof(int)); if (v) { memcpy(v+4, in.data(),     in.size());  WriteType<int>(v, htonl(in.size ())); } }
    void NTString(const StringPiece   &in) { auto v = Get(in.size ()+1);           if (v) { memcpy(v,   in.data(),     in.size());  v[in.size()]=0; } }
    void String  (const StringPiece   &in) { auto v = Get(in.size ());             if (v) { memcpy(v,   in.data(),     in.size());                  } }

    void Write8 (const unsigned char  &v) { WriteType(Get(sizeof(v)), v); }
    void Write8 (const          char  &v) { WriteType(Get(sizeof(v)), v); }
    void Write16(const unsigned short &v) { WriteType(Get(sizeof(v)), v); }
    void Write16(const          short &v) { WriteType(Get(sizeof(v)), v); }
    void Write32(const unsigned int   &v) { WriteType(Get(sizeof(v)), v); }
    void Write32(const          int   &v) { WriteType(Get(sizeof(v)), v); }
    void Write32(const unsigned long  &v) { WriteType(Get(sizeof(v)), v); }
    void Write32(const          long  &v) { WriteType(Get(sizeof(v)), v); }

    void Htons  (const unsigned short &v) { WriteType(Get(sizeof(v)), htons(v)); }
    void Ntohs  (const unsigned short &v) { WriteType(Get(sizeof(v)), ntohs(v)); }
    void Htons  (const          short &v) { WriteType(Get(sizeof(v)), htons(v)); }
    void Ntohs  (const          short &v) { WriteType(Get(sizeof(v)), ntohs(v)); }
    void Htonl  (const unsigned int   &v) { WriteType(Get(sizeof(v)), htonl(v)); }
    void Ntohl  (const unsigned int   &v) { WriteType(Get(sizeof(v)), ntohl(v)); }
    void Htonl  (const          int   &v) { WriteType(Get(sizeof(v)), htonl(v)); }
    void Ntohl  (const          int   &v) { WriteType(Get(sizeof(v)), ntohl(v)); }

    void Htons(unsigned short *out) const { Read16(out); *out = htons(*out); }
    void Ntohs(unsigned short *out) const { Read16(out); *out = ntohs(*out); }
    void Htons(         short *out) const { Read16(out); *out = htons(*out); }
    void Ntohs(         short *out) const { Read16(out); *out = ntohs(*out); }
    void Htonl(unsigned int   *out) const { Read32(out); *out = htonl(*out); }
    void Ntohl(unsigned int   *out) const { Read32(out); *out = ntohl(*out); }
    void Htonl(         int   *out) const { Read32(out); *out = htonl(*out); }
    void Ntohl(         int   *out) const { Read32(out); *out = ntohl(*out); }
    
    void Read8 (unsigned char   *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read8 (         char   *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read16(unsigned short  *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read16(         short  *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read32(unsigned int    *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read32(         int    *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read32(unsigned long   *out) const { ReadType(out, Get(sizeof(*out))); }
    void Read32(         long   *out) const { ReadType(out, Get(sizeof(*out))); }
    void ReadString(StringPiece *out) const { Ntohl(&out->len); out->buf = Get(out->len); }

    template <class X> void ReadUnalignedArray(ArrayPiece<X> *out) const
    { int l=0; Ntohl(&l); out->assign(reinterpret_cast<const X*>(Get(l)), l/sizeof(X)); }
  };

  struct ConstStream : public Stream {
    ConstStream(const char *B, int S) : Stream(const_cast<char*>(B), S) {}
  };

  struct MutableStream : public Stream {
    MutableStream(char *B, int S) : Stream(B, S) {}
    char *End() { return buf + size; }
    char *Get(int len=0) {
      char *ret = buf + offset;
      if ((offset += len) > size) { error=1; return 0; }
      return ret;
    }
  };

  struct Header {
    static const int size = 4;
    unsigned short id, seq;

    void Out(Stream *o) const;
    void In(const Stream *i);
  };

  int Id;
  Serializable(int ID) : Id(ID) {}

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

#undef IN
struct DNS {
  UNALIGNED_struct Header {
    unsigned short id;
#ifdef LFL_BIG_ENDIAN
    unsigned short qr:1, opcode:4, aa:1, tc:1, rd:1, ra:1, unused:1, ad:1, cd:1, rcode:4;
#else
    unsigned short rd:1, tc:1, aa:1, opcode:4, qr:1, rcode:4, cd:1, ad:1, unused:1, ra:1;
#endif
    unsigned short qdcount, ancount, nscount, arcount;
    static const int size = 12;
  }; UNALIGNED_END(Header, Header::size);

  struct Type { enum { A=1, NS=2, MD=3, MF=4, CNAME=5, SOA=6, MB=7, MG=8, MR=9, _NULL=10, WKS=11, PTR=12, HINFO=13, MINFO=14, MX=15, TXT=16 }; };
  struct Class { enum { IN=1, CS=2, CH=3, HS=4 }; };
  typedef map<string, vector<IPV4::Addr> > AnswerMap;

  struct Record {
    string question, answer;
    unsigned short type=0, _class=0, ttl1=0, ttl2=0, pref=0;
    IPV4::Addr addr=0;
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

struct MultiProcessResource {
  static bool Read(const MultiProcessBuffer &mpb, int type, Serializable *out);
};

struct MultiProcessFileResource : public Serializable {
  static const int Type = 1<<11 | 1;
  StringPiece buf, name, type;
  MultiProcessFileResource() : Serializable(Type) {}
  MultiProcessFileResource(const string &b, const string &n, const string &t) :
    Serializable(Type), buf(b), name(n), type(t) {}

  int HeaderSize() const { return sizeof(int) * 3; }
  int Size() const { return HeaderSize() + 3 + buf.size() + name.size() + type.size(); }

  void Out(Serializable::Stream *o) const {
    o->Htonl   (buf.size()); o->Htonl   (name.size()); o->Htonl   (type.size());
    o->NTString(buf);        o->NTString(name);        o->NTString(type);
  }
  int In(const Serializable::Stream *i) {
    /**/      i->Ntohl(&buf.len); /**/         i->Ntohl(&name.len); /**/         i->Ntohl(&type.len);
    buf.buf = i->Get  ( buf.len+1); name.buf = i->Get  ( name.len+1); type.buf = i->Get  ( type.len+1);
    return i->Result();
  }
};

struct MultiProcessTextureResource : public Serializable {
  static const int Type = 1<<11 | 2;
  int width=0, height=0, pf=0, linesize=0;
  StringPiece buf;
  MultiProcessTextureResource() : Serializable(Type) {}
  MultiProcessTextureResource(const LFL::Texture &t) :
    Serializable(Type), width(t.width), height(t.height), pf(t.pf), linesize(t.LineSize()),
    buf(reinterpret_cast<const char *>(t.buf), t.BufferSize()) {}

  int HeaderSize() const { return sizeof(int) * 4; }
  int Size() const { return HeaderSize() + buf.size(); }

  void Out(Serializable::Stream *o) const {
    CHECK_EQ(linesize * height, buf.len);
    o->Htonl(width); o->Htonl(height); o->Htonl(pf); o->Htonl(linesize);
    o->String(buf);
  }
  int In(const Serializable::Stream *i) {
    i->Ntohl(&width); i->Ntohl(&height); i->Ntohl(&pf); i->Ntohl(&linesize);
    buf.buf = i->Get((buf.len = linesize * height));
    return i->Result();
  }
};

struct MultiProcessPaintResource : public Serializable {
  static const int Type = 1<<11 | 3;
  struct Iterator {
    int offset=0, type=0;
    StringPiece buf;
    Iterator(const StringPiece &b) : buf(b) { Load(); }
    template <class X> const X* Get() { return reinterpret_cast<const X*>(buf.buf + offset); }
    void Load() { type = (offset + sizeof(int) > buf.len) ? 0 : *Get<int>(); }
    void Next() { offset += CmdSize(type); Load(); }
  };

  UNALIGNED_struct Cmd { int type; Cmd(int T=0) : type(T) {} }; UNALIGNED_END(Cmd, 4);
  UNALIGNED_struct SetAttr : public Cmd {
    static const int Type=1, Size=76;
    int font_id, tex_id; Color fg, bg; Box scissor; bool underline, overline, midline, blink, blend, hfg, hbg, hs;
    SetAttr(const Drawable::Attr &a=Drawable::Attr()) : Cmd(Type), font_id(a.font?IPCClientFontEngine::GetId(a.font):0),
      tex_id(a.tex?a.tex->ID:0), fg(a.fg?*a.fg:Color()), bg(a.bg?*a.bg:Color()), scissor(a.scissor?*a.scissor:Box()),
      underline(a.underline), overline(a.overline), midline(a.midline), blink(a.blink), blend(a.blend),
      hfg(a.fg), hbg(a.bg), hs(a.scissor) {}
    void Update(Drawable::Attr *o, ProcessAPIClient *s) const;
  }; UNALIGNED_END(SetAttr, SetAttr::Size);

  UNALIGNED_struct InitDrawBox        : public Cmd { static const int Type=2, Size=12; point p;       InitDrawBox       (const point &P=point())       : Cmd(Type), p(P) {} };         UNALIGNED_END(InitDrawBox,        InitDrawBox::Size);
  UNALIGNED_struct InitDrawBackground : public Cmd { static const int Type=3, Size=12; point p;       InitDrawBackground(const point &P=point())       : Cmd(Type), p(P) {} };         UNALIGNED_END(InitDrawBackground, InitDrawBackground::Size);
  UNALIGNED_struct DrawBox            : public Cmd { static const int Type=4, Size=32; Box b; int id; DrawBox           (const Box &B=Box(), int ID=0) : Cmd(Type), b(B), id(ID) {} }; UNALIGNED_END(DrawBox,            DrawBox::Size);
  UNALIGNED_struct DrawBackground     : public Cmd { static const int Type=5, Size=28; Box b;         DrawBackground    (const Box &B=Box())           : Cmd(Type), b(B) {} };         UNALIGNED_END(DrawBackground,     DrawBackground::Size);
  UNALIGNED_struct PushScissor        : public Cmd { static const int Type=6, Size=28; Box b;         PushScissor       (const Box &B=Box())           : Cmd(Type), b(B) {} };         UNALIGNED_END(PushScissor,        PushScissor::Size);
  UNALIGNED_struct PopScissor         : public Cmd { static const int Type=7, Size=4;                 PopScissor        ()                             : Cmd(Type) {} };               UNALIGNED_END(PopScissor,         PopScissor::Size);

  StringBuffer data;
  mutable Drawable::Attr attr;
  MultiProcessPaintResource() : Serializable(Type) {}
  void Out(Serializable::Stream *o) const { o->BString(data.buf); }
  int In(const Serializable::Stream *i) { i->ReadString(&data.buf); return i->Result(); }
  int Size() const { return HeaderSize() + data.buf.size(); }
  int HeaderSize() const { return sizeof(int); }
  int Run(const Box&) const;

  static int CmdSize(int n) {
    switch(n) {
      case SetAttr           ::Type: return SetAttr           ::Size;
      case InitDrawBox       ::Type: return InitDrawBox       ::Size;
      case InitDrawBackground::Type: return InitDrawBackground::Size;
      case DrawBox           ::Type: return DrawBox           ::Size;
      case DrawBackground    ::Type: return DrawBackground    ::Size;
      case PushScissor       ::Type: return PushScissor       ::Size;
      case PopScissor        ::Type: return PopScissor        ::Size;
      default:                       FATAL("unknown cmd ", n);
    }
  }
};

struct MultiProcessPaintResourceBuilder : public MultiProcessPaintResource {
  int count=0; bool dirty=0;
  MultiProcessPaintResourceBuilder(int S=32768) { data.Resize(S); }

  int Count() const { return count; }
  void Clear() { data.Clear(); count=0; dirty=0; }
  void Add(const Cmd &cmd) { data.Add(&cmd, CmdSize(cmd.type)); count++; dirty=1; }
  void AddList(const MultiProcessPaintResourceBuilder &x)
  { data.Add(x.data.begin(), x.data.size()); count += x.count; dirty=1; }
};

struct MultiProcessLayerTree : public Serializable {
  static const int Type = 1<<11 | 4;
  ArrayPiece<LayersInterface::Node>  node_data;
  ArrayPiece<LayersInterface::Child> child_data;
  MultiProcessLayerTree() : Serializable(Type) {}
  MultiProcessLayerTree(const vector<LayersInterface::Node> &n, const vector<LayersInterface::Child> &c) :
    Serializable(Type), node_data(&n[0], n.size()), child_data(&c[0], c.size()) {}

  void Out(Serializable::Stream *o) const { o->AString(node_data); o->AString(child_data); }
  int In(const Serializable::Stream *i)
  { i->ReadUnalignedArray(&node_data); i->ReadUnalignedArray(&child_data); return i->Result(); }
  int Size() const { return HeaderSize() + node_data.Bytes() + child_data.Bytes(); }
  int HeaderSize() const { return sizeof(int)*2; }
  void AssignTo(LayersInterface *layers) const {
    layers->node .assign(node_data .data(), node_data .data() + node_data .size());
    layers->child.assign(child_data.data(), child_data.data() + child_data.size());
  }
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
      up_x  = (short)(up.x*scale);  up_y  = (short)(up.y*scale);  up_z =  (short)(up.z*scale);
    }
    void To(v3 *ort, v3 *up) {
      ort->x = (float)ort_x/scale; ort->y = (float)ort_y/scale; ort->z = (float)ort_z/scale;
      up->x  = (float) up_x/scale;  up->y = (float) up_y/scale;  up->z = (float) up_z/scale;
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
    static const int ID = 1;
    ChallengeRequest() : Serializable(ID) {}

    int HeaderSize() const { return 0; }
    int Size() const { return HeaderSize(); }
    void Out(Serializable::Stream *o) const {}
    int   In(const Serializable::Stream *i) { return 0; }
  };

  struct ChallengeResponse : public Serializable {
    static const int ID = 2;
    int token;
    ChallengeResponse() : Serializable(ID) {}

    int HeaderSize() const { return 4; }
    int Size() const { return HeaderSize(); }
    void Out(Serializable::Stream *o) const { o->Htonl( token); }
    int   In(const Serializable::Stream *i) { i->Ntohl(&token); return 0; }
  };

  struct JoinRequest : public Serializable {
    static const int ID = 3;
    int token;
    string PlayerName;
    JoinRequest() : Serializable(ID) {}

    int HeaderSize() const { return 4; }
    int Size() const { return HeaderSize() + PlayerName.size(); }
    void Out(Serializable::Stream *o) const { o->Htonl( token); o->String(PlayerName); }
    int   In(const Serializable::Stream *i) { i->Ntohl(&token); PlayerName = i->Get(); return 0; }
  };

  struct JoinResponse : public Serializable {
    static const int ID = 4;
    string rcon;
    JoinResponse() : Serializable(ID) {}

    int HeaderSize() const { return 0; }
    int Size() const { return rcon.size(); }
    void Out(Serializable::Stream *o) const { o->String(rcon); }
    int   In(const Serializable::Stream *i) { rcon = i->Get(); return 0; }
  };

  struct WorldUpdate : public Serializable {
    static const int ID = 5;
    unsigned short id;
    vector<Entity> entity;
    vector<Collision> collision;
    WorldUpdate() : Serializable(ID) {}

    int HeaderSize() const { return 6; }
    int Size() const { return HeaderSize() + entity.size() * Entity::size + collision.size() * Collision::size; }

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
    static const int ID = 6;
    unsigned short id_WorldUpdate, time_since_WorldUpdate;
    unsigned buttons;
    Orientation ort;
    PlayerUpdate() : Serializable(ID) {}

    int HeaderSize() const { return 8 + Orientation::size; }
    int Size() const { return HeaderSize(); }
    void Out(Serializable::Stream *o) const { o->Htons( id_WorldUpdate); o->Htons( time_since_WorldUpdate); o->Htonl( buttons); ort.Out(o); }
    int   In(const Serializable::Stream *i) { i->Ntohs(&id_WorldUpdate); i->Ntohs(&time_since_WorldUpdate); i->Ntohl(&buttons); ort.In(i); return 0; }
  };

  struct RconRequest : public Serializable {
    static const int ID = 7;
    string Text;
    RconRequest(const string &t=string()) : Serializable(ID), Text(t) {}

    int HeaderSize() const { return 0; }
    int Size() const { return HeaderSize() + Text.size(); }
    void Out(Serializable::Stream *o) const { o->String(Text); }
    int   In(const Serializable::Stream *i) { Text = i->Get(); return 0; }
  };

  struct RconResponse : public Serializable {
    static const int ID = 8;
    RconResponse() : Serializable(ID) {}

    int HeaderSize() const { return 0; }
    int Size() const { return HeaderSize(); }
    void Out(Serializable::Stream *o) const {}
    int   In(const Serializable::Stream *i) { return 0; }
  };

  struct PlayerList : public RconRequest {
    static const int ID = 9;
    PlayerList() { Id=ID; }
  };
};

}; // namespace LFL
#endif // LFL_LFAPP_WIRE_H__
