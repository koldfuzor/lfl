/*
 * $Id: lfapp.h 1335 2014-12-02 04:13:46Z justin $
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

#ifndef __LFL_LFAPP_LFAPP_H__
#define __LFL_LFAPP_LFAPP_H__

#ifndef _WIN32
#include <sstream>
#include <typeinfo>
#endif

#include <vector>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#define LFL_STL_NAMESPACE std

#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#define LFL_STL11_NAMESPACE std

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <malloc.h>
#include <io.h>
#include <sstream>
#include <typeinfo>
#define LFL_EXPORT __declspec(dllexport)
#define LFL_IMPORT __declspec(dllimport)
#define _USE_MATH_DEFINES
#define UNION struct
typedef SOCKET Socket;
inline int SystemBind(Socket s, const sockaddr *a, int al) { return bind(s, a, al); }
#else /* _WIN32 */
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#define LFL_EXPORT
#define LFL_IMPORT
#define UNION union
typedef int Socket;
#define SystemBind(s, a, al) ::bind(s, a, al)
#endif

using LFL_STL_NAMESPACE::min;
using LFL_STL_NAMESPACE::max;
using LFL_STL_NAMESPACE::swap;
using LFL_STL_NAMESPACE::pair;
using LFL_STL_NAMESPACE::vector;
using LFL_STL_NAMESPACE::string;
using LFL_STL_NAMESPACE::basic_string;
using LFL_STL_NAMESPACE::map;
using LFL_STL_NAMESPACE::set;
using LFL_STL_NAMESPACE::deque;
using LFL_STL_NAMESPACE::inserter;
using LFL_STL_NAMESPACE::auto_ptr;
using LFL_STL_NAMESPACE::search;
using LFL_STL_NAMESPACE::sort;
using LFL_STL_NAMESPACE::unique;
using LFL_STL_NAMESPACE::equal_to;
using LFL_STL_NAMESPACE::lower_bound;
using LFL_STL_NAMESPACE::set_difference;
using LFL_STL11_NAMESPACE::unordered_map;
using LFL_STL11_NAMESPACE::unordered_set;
using LFL_STL11_NAMESPACE::shared_ptr;
using LFL_STL11_NAMESPACE::function;
using LFL_STL11_NAMESPACE::bind;
using LFL_STL11_NAMESPACE::placeholders::_1;
using LFL_STL11_NAMESPACE::placeholders::_2;
using LFL_STL11_NAMESPACE::placeholders::_3;
using LFL_STL11_NAMESPACE::placeholders::_4;
using LFL_STL11_NAMESPACE::placeholders::_5;
using LFL_STL11_NAMESPACE::placeholders::_6;

#include <errno.h>
#include <math.h>
#include <cfloat>

#ifdef _WIN32
#include <float.h>
#include <direct.h>
#undef CALLBACK
#define isinf(x) (x <= -INFINITY || x >= INFINITY)
#define isnan _isnan
#define isfinite _finite
#define getcwd _getcwd
#define chdir _chdir
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf _snprintf
#define S_IFDIR _S_IFDIR
#define socklen_t int
int close(int socket);
extern char *optarg;
extern int optind;
#endif

#ifdef __APPLE__
#include <cmath>
extern "C" int isnan(double);
extern "C" int isinf(double);
#define isfinite(x) (!isnan(x) && !isinf(x))
#else
#define isfinite(x) finite(x)
#endif

#ifdef LFL_ANDROID
#include <sys/endian.h>
#include "lfjni/lfjni.h"
#endif

#ifdef __linux__
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if _WIN32 || _WIN64
 #if _WIN64
  #define LFL64
 #else
  #define LFL32
 #endif
#endif
#if __GNUC__
 #if __x86_64__ || __ppc64__ || __amd64__
  #define LFL64
 #else
  #define LFL32
 #endif
#endif

#define LFL_MOBILE (defined(LFL_ANDROID) || defined(LFL_IPHONE))
#define LFL_LINUX_SERVER (defined(__linux__) && !defined(LFL_MOBILE))

#define ASSETS_DIR "assets/"
#define M_TAU (M_PI + M_PI)
#define Hours(x) ((x)*3600000)
#define Minutes(x) ((x)*60000)
#define Seconds(x) ((x)*1000)
#define ToSeconds(x) ((x)/1000.0)
#define MilliSeconds(x) (x)
#define ToMilliSeconds(x) (x)
#define ToMicroSeconds(x) ((x)*1000)
#define Time2time_t(x) ((time_t)((x)/1000))
#define X_or_1(x) ((x) ? (x) : 1)
#define X_or_Y(x, y) ((x) ? (x) : (y))
#define XY_or_Y(x, y) ((x) ? ((x)*(y)) : (y))
#define X_or_Y_or_Z(x, y, z) ((x) ? (x) : ((y) ? (y) : (z)))

#define  INFO(...) lfapp_log(LogLevel::Info,  __FILE__, __LINE__, StrCat(__VA_ARGS__))
#define DEBUG(...) lfapp_log(LogLevel::Debug, __FILE__, __LINE__, StrCat(__VA_ARGS__))
#define ERROR(...) lfapp_log(LogLevel::Error, __FILE__, __LINE__, StrCat(__VA_ARGS__))
#define FATAL(...) { lfapp_log(LogLevel::Fatal, __FILE__, __LINE__, StrCat(__VA_ARGS__)); throw(0); }

#define  INFOf(fmt, ...) lfapp_log(LogLevel::Info,  __FILE__, __LINE__, StringPrintf(fmt, __VA_ARGS__))
#define DEBUGf(fmt, ...) lfapp_log(LogLevel::Debug, __FILE__, __LINE__, StringPrintf(fmt, __VA_ARGS__))
#define ERRORf(fmt, ...) lfapp_log(LogLevel::Error, __FILE__, __LINE__, StringPrintf(fmt, __VA_ARGS__))
#define FATALf(fmt, ...) { lfapp_log(LogLevel::Fatal, __FILE__, __LINE__, StringPrintf(fmt, __VA_ARGS__)); throw(0); }

#define DECLARE_FLAG(name, type) extern type FLAGS_ ## name
#define DECLARE_int(name) DECLARE_FLAG(name, int)
#define DECLARE_bool(name) DECLARE_FLAG(name, bool)
#define DECLARE_float(name) DECLARE_FLAG(name, float)
#define DECLARE_double(name) DECLARE_FLAG(name, double)
#define DECLARE_string(name) DECLARE_FLAG(name, string)

#define ONCE(x) { static bool once=0; if (!once && (once=1)) { x; } }
#define EVERY_N(x, y) { static int every_N=0; if (every_N++ % (x) == 0) { y; } }
#define EX_LT(x, a)    if (!((x) <  (a)))  INFO((x), " < ",  (a), ": EX_LT(",    #x, ", ", #a, ")"); 
#define EX_GT(x, a)    if (!((x) >  (a)))  INFO((x), " > ",  (a), ": EX_GT(",    #x, ", ", #a, ")"); 
#define EX_GE(x, a)    if (!((x) >= (a)))  INFO((x), " >= ", (a), ": EX_GE(",    #x, ", ", #a, ")"); 
#define EX_LE(x, a)    if (!((x) <= (a)))  INFO((x), " <= ", (a), ": EX_LE(",    #x, ", ", #a, ")"); 
#define EX_EQ(x, a)    if (!((x) == (a)))  INFO((x), " == ", (a), ": EX_EQ(",    #x, ", ", #a, ")");
#define EX_NE(x, a)    if (!((x) != (a)))  INFO((x), " != ", (a), ": EX_NE(",    #x, ", ", #a, ")");
#define CHECK_LT(x, a) if (!((x) <  (a))) FATAL((x), " < ",  (a), ": CHECK_LT(", #x, ", ", #a, ")"); 
#define CHECK_GT(x, a) if (!((x) >  (a))) FATAL((x), " > ",  (a), ": CHECK_GT(", #x, ", ", #a, ")"); 
#define CHECK_GE(x, a) if (!((x) >= (a))) FATAL((x), " >= ", (a), ": CHECK_GE(", #x, ", ", #a, ")"); 
#define CHECK_LE(x, a) if (!((x) <= (a))) FATAL((x), " <= ", (a), ": CHECK_LE(", #x, ", ", #a, ")"); 
#define CHECK_EQ(x, a) if (!((x) == (a))) FATAL((x), " == ", (a), ": CHECK_EQ(", #x, ", ", #a, ")");
#define CHECK_NE(x, a) if (!((x) != (a))) FATAL((x), " != ", (a), ": CHECK_NE(", #x, ", ", #a, ")");
#define CHECK_RANGE(x, y, z) { CHECK_GE(x, y); CHECK_LT(x, z); }
#define CHECK(x) if (!(x)) FATAL(#x)

#define memzero(x) memset(&x, 0, sizeof(x))
#define memzeros(x) memset(x, 0, sizeof(x))
#define memzerop(x) memset(x, 0, sizeof(*x))
#define sizeofarray(x) (sizeof(x) / sizeof((x)[0]))

struct FT_FaceRec_;
typedef struct CGFont    *CGFontRef;
typedef struct CGContext *CGContextRef;

extern "C" {
struct _IplImage;
struct SwrContext;
typedef struct bio_st BIO;
typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;
#ifdef LFL_FFMPEG
struct AVFormatContext;
struct AVStream;
struct AVPacket;
struct AVFrame;
struct SwsContext;
#endif
};

namespace google {
    namespace protobuf {
#ifdef LFL_PROTOBUF
        class Message;
#else
        class Message { int fake; };
#endif
    }; // namespace protobuf
#ifdef LFL_GLOG
    LFL_IMPORT void InstallFailureSignalHandler();
#endif
}; // namespace google

namespace LFL {
struct Box;
struct BoxArray;
struct Atlas;
struct Color;
struct Entity;
struct Asset;
struct SoundAsset;
struct MovieAsset;
struct Shader;
struct Texture;
struct Geometry;
struct Allocator;
struct ProtoHeader;
struct GraphicsDevice;
struct Connection;
struct Listener;
struct Service;
struct Font;
struct Flow;
struct FloatedBox;
struct FloatContainer;
struct GUI;
struct KeyboardGUI;
struct TextGUI;
struct Console;
struct Browser;
struct Dialog;
struct Tiles;
struct Bind;
struct BindMap;
struct StyleSheet;
struct StyleContext;
struct VideoAssetLoader;
struct Window;
namespace DOM { struct Node; };

typedef long long Time;
typedef google::protobuf::Message Proto;
typedef basic_string<short> String16;
typedef function<void()> Callback;

struct LogLevel { enum { Fatal=-1, Error=0, Info=3, Debug=7 }; int l; };
extern "C" void lfapp_log(int level, const char *file, int line, const string &message);

unsigned           fnv32(const void *buf, unsigned len=0, unsigned           hval=0);
unsigned long long fnv64(const void *buf, unsigned len=0, unsigned long long hval=0);

Time Now();
void Msleep(int x);
timeval Time2timeval(Time x);

template <class X> struct Singleton { static X *Get() { static X instance; return &instance; } };

struct Allocator {
    virtual ~Allocator() {}
    virtual const char *name() = 0;
    virtual void *malloc(int size) = 0;
    virtual void *realloc(void *p, int size) = 0;
    virtual void free(void *p) = 0;
    virtual void reset();
#define AllocatorNew(allocator, type, constructor_args) (new((allocator)->malloc(sizeof type )) type constructor_args)
};

struct typed_ptr {
    int type; void *value;
    typed_ptr() : type(0), value(0) {}
    typed_ptr(int T, void *P) : type(T), value(P) {}
};

struct Typed {
    template <class X> static void Swap(X& a, X& b) { X swap=a; a=b; b=swap; }
    template <class X> static void Replace(X** p, X* r) { delete (*p); (*p) = r; }
    template <class X> static void AllocReplace(X** p, X* r) { if (*p && (*p)->alloc) (*p)->alloc->free(*p); *p = r; }
    template <class X> static bool Changed(X* p, const X& r) { bool ret = *p != r; *p = r; return ret; }
    template <class X> static X Min(X a, X b) { return b < a ? b : a; }
    template <class X> static X Max(X a, X b) { return b > a ? b : a; }
    template <class X> static X Negate(X x) { return x ? -x : x; }
    template <class X> static bool Max(X *a, X b) { if (b <= *a) return 0; *a = b; return 1; }
    template <class X> static bool Min(X *a, X b) { if (b >= *a) return 0; *a = b; return 1; }
    template <class X> static bool Within(X x, X a, X b) { return x >= a && x <= b; }
    template <class X> static string Str(const X& x) { std::stringstream in; in << x; return string(in.str().c_str()); }
    template <class X> static int Id()   { static int ret = fnv32(typeid(X).name()); return ret; }
    template <class X> static int Id(X*) { static int ret = fnv32(typeid(X).name()); return ret; }
    template <class X> static typed_ptr Pointer(X* v) { return typed_ptr(Typed::Id<X>(), v); }
    template <class X> static void MinusPlus(X *m, X* p, X v) { *m -= v; *p += v; }
};

struct Scannable {
    static bool     Scan(const bool&,     const char  *v) { return *v ? atoi(v) : true; }
    static int      Scan(const int&,      const char  *v) { return atoi(v); }
    static unsigned Scan(const unsigned&, const char  *v) { return atoi(v); }
    static float    Scan(const float&,    const char  *v) { return atof(v); }
    static double   Scan(const double&,   const char  *v) { return atof(v); }
    static string   Scan(const string&,   const char  *v) { return string(v); }
    static String16 Scan(const String16&, const short *v) { return String16(v); }
};

struct Printable : public string {
    Printable(const void *x);
    Printable(const basic_string<short> &x);
    Printable(const string &x) : string(x) {}
    Printable(const char *x) : string(x) {}
    Printable(const int &x) : string(Typed::Str(x)) {}
    Printable(const long &x) : string(Typed::Str(x)) {}
    Printable(const unsigned char *x) : string((char*)x) {}
    Printable(const char &x) : string(Typed::Str(x)) {}
    Printable(const float &x) : string(Typed::Str(x)) {}
    Printable(const double &x) : string(Typed::Str(x)) {}
    Printable(const unsigned &x) : string(Typed::Str(x)) {}
    Printable(const long long &x) : string(Typed::Str(x)) {}
    Printable(const unsigned char &x) : string(Typed::Str(x)) {}
    Printable(const unsigned long &x) : string(Typed::Str(x)) {}
    Printable(const unsigned long long &x) : string(Typed::Str(x)) {}
    Printable(const vector<string> &x);
    Printable(const vector<double> &x);
    Printable(const vector<float> &x);
    Printable(const vector<int> &x);
    Printable(const Color &x);
};
inline string StrCat(const Printable &x1) { return x1; }

template <class X> struct ArrayPiece {
    const X *buf; int len;
    ArrayPiece()                  : buf(0), len(0) {}
    ArrayPiece(const X *b, int l) : buf(b), len(l) {}
    void clear() { buf=0; len=0; }
    bool null() const { return !buf; }
    bool empty() const { return !buf || len <= 0; }
    bool has_size() const { return len >= 0; }
    int size() const { return max(0, len); }
    void assign(const X *b, int l) { buf=b; len=l; }
    const X *data() const { return buf; }
    const X *end() const { return buf+len; }
};

template <class X> struct StringPieceT : public ArrayPiece<X> {
    StringPieceT() {}
    StringPieceT(const basic_string<X> &s) : ArrayPiece<X>(s.data(), s.size()) {}
    StringPieceT(const X *b, int l)        : ArrayPiece<X>(b,        l)        {}
    StringPieceT(const X *b)               : ArrayPiece<X>(b,        Len(b))   {}
    basic_string<X> str() const {
        if (this->buf && this->len < 0) return this->buf;
        return this->buf ? basic_string<X>(this->buf, this->len) : basic_string<X>();
    }
    bool end(const X* p) const { return (this->len >= 0 && p >= this->buf + this->len) || !*p; }
    static StringPieceT<X> Unbounded (const X *b) { return StringPieceT<X>(b, -1); }
    static StringPieceT<X> FromString(const X *b) { return StringPieceT<X>(b, b?Len(b):0); }
    static size_t Len(const X *b) { const X *p=b; while(*p) p++; return p-b; }
    static const X *Blank() { static X x[1] = {0}; return x; }
    static const X *Space() { static X x[2] = {' ',0}; return x; }
    static const X *NullSpelled() { static X x[7] = {'<','N','U','L','L','>',0}; return x; }
};
typedef StringPieceT<char> StringPiece;
typedef StringPieceT<short> String16Piece;

struct String {
    static void Copy(const string   &in,   string *out) { *out = in; }
    static void Copy(const String16 &in, String16 *out) { *out = in; }
    template <class X, class Y> static void Append(const basic_string<X> &in, basic_string<Y> *out) { Copy(in.data(), in.size(), out, out->size()); }
    template <class X, class Y> static void Copy(const basic_string<X> &in, basic_string<Y> *out, int offset=0) { return Copy(in.data(), in.size(), out, offset); }
    template <class X, class Y> static void Copy(const X *in, int len, basic_string<Y> *out, int offset=0)
    { out->resize(len + offset); for (int i=0; i<len; i++) (*out)[i + offset] = (Y)in[i]; }

    template <class X, class Y> static int
        Convert(const X *in, int len,      basic_string<Y> *out, const char *from, const char *to);
    template <class X>          static int
        Convert(const basic_string<X> &in, basic_string<X> *out, const char *from, const char *to) {
            if (!strcmp(from, to)) { *out = in; return in.size(); }
            return Convert(in.data(), in.size(), out, from, to);
        }
    template <class X, class Y> static int
        Convert(const basic_string<X> &in, basic_string<Y> *out, const char *from, const char *to)
        { return Convert(in.data(), in.size(), out, from, to); }

    static string   ToUTF8 (const char  *s, int l, int *lo=0) { if (lo) *lo=l; return string(s, l); }
    static string   ToUTF8 (const short *s, int l, int *lo=0) { return ToUTF8(String16(s, l), lo); }
    static string   ToUTF8 (const String16 &s, int *lo=0) { string v; int l=Convert(s, &v, "UTF-16LE", "UTF-8"); if (lo) *lo=l; return v; }
    static string   ToUTF8 (const string   &s, int *lo=0) { if (lo) *lo=s.size(); return s; }
    static String16 ToUTF16(const String16 &s, int *lo=0) { if (lo) *lo=s.size(); return s; }
    static String16 ToUTF16(const string   &s, int *lo=0) { String16 v; int l=Convert(s, &v, "UTF-8", "UTF-16LE"); if (lo) *lo=l; return v; }
    static string   ToAscii(const String16 &s, int *lo=0) { string v; int l=Convert(s, &v, "UCS-16LE", "US-ASCII"); if (lo) *lo=l; return v; }
    static string   ToAscii(const string   &s, int *lo=0) { if (lo) *lo=s.size(); return s; }
};

struct UTF8 {
    static string WriteGlyph(int codepoint);
    static int ReadGlyph(const StringPiece   &s, const char  *p, int *l);
};
struct UTF16 {
    static String16 WriteGlyph(int codepoint);
    static int ReadGlyph(const String16Piece &s, const short *p, int *l);
};
template <class X> struct UTF {};
template <> struct UTF<char> {
    static string WriteGlyph(int codepoint) { return UTF8::WriteGlyph(codepoint); }
    static int ReadGlyph(const StringPiece   &s, const char  *p, int *l) { return UTF8::ReadGlyph(s, p, l); }
    static int ReadGlyph(const String16Piece &s, const short *p, int *l) { FATAL("not implemented"); }
};
template <> struct UTF<short> {
    static String16 WriteGlyph(int codepoint) { return UTF16::WriteGlyph(codepoint); }
    static int ReadGlyph(const String16Piece &s, const short *p, int *l) { return UTF16::ReadGlyph(s, p, l); }
    static int ReadGlyph(const StringPiece   &s, const char  *p, int *l) { FATAL("not implemented"); }
};

struct Flag {
    const char *name, *desc, *file; int line;
    Flag(const char *N, const char *D, const char *F, int L) : name(N), desc(D), file(F), line(L) {}
    virtual ~Flag() {}

    string ToString() const;
    virtual void Update(const char *val) = 0;
    virtual string Get() const = 0;
    virtual bool IsBool() const = 0;
};

struct FlagMap {
    typedef map<string, Flag*> AllFlags;
    typedef map<string, vector<Flag*> > FileFlags;
    const char *optarg=0; int optind=0;
    AllFlags flagmap;
    FileFlags fileflags;
    bool dirty=0;
    FlagMap() {}

    int getopt(int argc, const char **argv, const char *source_filename);
    void Add(Flag *f) { flagmap[f->name] = f; fileflags[f->file].push_back(f); }
    bool Set(const string &k, const string &v);
    bool IsBool(const string &k) const;
    string Get(const string &k) const;
    string Match(const string &k, const char *source_filename=0) const;
    void Print(const char *source_filename=0) const;
};

template <class X> struct FlagOfType : public Flag {
    X *v;
    virtual ~FlagOfType() {}
    FlagOfType(const char *N, const char *D, const char *F, int L, X *V)
        : Flag(N, D, F, L), v(V) { Singleton<FlagMap>::Get()->Add(this); } 

    void Update(const char *val) { if (val) *v = Scannable::Scan(*v, val); }
    string Get() const { return Typed::Str(*v); }
    bool IsBool() const { return Typed::Id<X>() == Typed::Id<bool>(); }
};

#define DEFINE_FLAG(name, type, initial, description) \
    type FLAGS_ ## name = initial; \
    FlagOfType<type> FLAGS_ ## name ## _WRAPPER(#name, description, __FILE__, __LINE__, &FLAGS_ ## name)

#define DEFINE_int(name, initial, description) DEFINE_FLAG(name, int, initial, description)
#define DEFINE_bool(name, initial, description) DEFINE_FLAG(name, bool, initial, description)
#define DEFINE_float(name, initial, description) DEFINE_FLAG(name, float, initial, description)
#define DEFINE_double(name, initial, description) DEFINE_FLAG(name, double, initial, description)
#define DEFINE_string(name, initial, description) DEFINE_FLAG(name, string, initial, description)

DECLARE_bool(lfapp_audio);
DECLARE_bool(lfapp_video);
DECLARE_bool(lfapp_input);
DECLARE_bool(lfapp_network);
DECLARE_bool(lfapp_camera);
DECLARE_bool(lfapp_cuda);
DECLARE_bool(lfapp_multithreaded);
DECLARE_bool(lfapp_wait_forever);
DECLARE_bool(lfapp_debug);
DECLARE_int(target_fps);
DECLARE_int(min_fps);
DECLARE_bool(max_rlimit_core);
DECLARE_bool(max_rlimit_open_files);
DECLARE_bool(open_console);

bool Running();
bool MainThread();
void RunMainThreadCallback(Callback *cb);
double FPS();
double CamFPS();
void break_hook();
void press_any_key();
bool input_fgets(char *buf, int size);
bool nb_fgets(FILE*, char *buf, int size);
int nb_read(int fd, char *buf, int size);
string nb_read(int fd, int size);
const char *dldir();

template <int V>          int                 isint (int N) { return N == V; }
template <int V1, int V2> int                 isint2(int N) { return (N == V1) || (N == V2); }
template <int V1, int V2, int V3>         int isint3(int N) { return (N == V1) || (N == V2) || (N == V3); }
template <int V1, int V2, int V3, int V4> int isint4(int N) { return (N == V1) || (N == V2) || (N == V3) || (N == V4); }
int isfileslash(int c);
int isdot(int c);
int iscomma(int c);
int isand(int c);
int isdquote(int c);
int issquote(int c);
int istick(int c);
int isdig(int c);
int isnum(int c);
int isquote(int c);
int notspace(int c);
int notalpha(int c);
int notalnum(int c);
int notnum(int c);
int notcomma(int c);
int notdot(int c);
float my_atof(const char *v);
inline double atof(const string &v) { return ::atof(v.c_str()); }
inline int    atoi(const string &v) { return ::atoi(v.c_str()); }

int atoi(const char  *v);
int atoi(const short *v);
template <int F, int T>                 int tochar (int i) { return i == F ? T :  i; }
template <int F, int T, int F2, int T2> int tochar2(int i) { return i == F ? T : (i == F2 ? T2 : i); }

int doubleSort(double a, double b);
int doubleSort (const void *a, const void *b);
int doubleSortR(const void *a, const void *b);
int next_multiple_of_power_of_two(int input, int align);
double squared(double n);
float rand(float a, float b);

template <class X> const X *BlankNull(const X *x) { return x ? x : StringPieceT<X>::Blank(); }
template <class X> const X *SpellNull(const X *x) { return x ? x : StringPieceT<X>::NullSpelled(); }
const char *Default(const char *x, const char *default_x);
string TrimWhiteSpace(const string &s);
string   ReplaceEmpty (const string   &in, const string   &replace_with);
String16 ReplaceEmpty (const String16 &in, const string   &replace_with);
String16 ReplaceEmpty (const String16 &in, const String16 &replace_with);
string ReplaceNewlines(const string   &in, const string   &replace_with);
string StringPrintf(const char *fmt, ...);
string WStringPrintf(const wchar_t *fmt, ...);
String16 String16Printf(const char *fmt, ...);
void StringAppendf(string *out, const char *fmt, ...);
void StringAppendf(String16 *out, const char *fmt, ...);
int sprint(char *out, int len, const char *fmt, ...);
string StrCat(const Printable &x1, const Printable &x2);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12, const Printable &x13);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12, const Printable &x13, const Printable &x14);
string StrCat(const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12, const Printable &x13, const Printable &x14, const Printable &x15);

void StrAppend(string *out, const Printable &x1);
void StrAppend(string *out, const Printable &x1, const Printable &x2);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12);
void StrAppend(string *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12, const Printable &x13);

void StrAppend(String16 *out, const Printable &x1);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12);
void StrAppend(String16 *out, const Printable &x1, const Printable &x2, const Printable &x3, const Printable &x4, const Printable &x5, const Printable &x6, const Printable &x7, const Printable &x8, const Printable &x9, const Printable &x10, const Printable &x11, const Printable &x12, const Printable &x13);

bool PrefixMatch(const short    *in, const short    *pref, int case_sensitive=true);
bool PrefixMatch(const char     *in, const char     *pref, int case_sensitive=true);
bool PrefixMatch(const char     *in, const string   &pref, int case_sensitive=true);
bool PrefixMatch(const string   &in, const char     *pref, int case_sensitive=true);
bool PrefixMatch(const string   &in, const string   &pref, int case_sensitive=true);
bool PrefixMatch(const String16 &in, const String16 &pref, int case_sensitive=true);
bool PrefixMatch(const String16 &in, const char     *pref, int case_sensitive=true);

bool SuffixMatch(const short    *in, const short    *pref, int case_sensitive=true);
bool SuffixMatch(const char     *in, const char     *pref, int case_sensitive=true);
bool SuffixMatch(const char     *in, const string   &pref, int case_sensitive=true);
bool SuffixMatch(const string   &in, const char     *pref, int case_sensitive=true);
bool SuffixMatch(const string   &in, const string   &pref, int case_sensitive=true);
bool SuffixMatch(const String16 &in, const string   &pref, int case_sensitive=true);
bool SuffixMatch(const String16 &in, const String16 &pref, int case_sensitive=true);

bool StringEquals(const String16 &s1, const String16 &s2, int case_sensitive=false);
bool StringEquals(const String16 &s1, const char     *s2, int case_sensitive=false);
bool StringEquals(const string   &s1, const string   &s2, int case_sensitive=false);
bool StringEquals(const char     *s1, const string   &s2, int case_sensitive=false);
bool StringEquals(const string   &s1, const char     *s2, int case_sensitive=false);
bool StringEquals(const char     *s1, const char     *s2, int case_sensitive=false);
bool StringEquals(const short    *s1, const short    *s2, int case_sensitive=false);

bool StringEmptyOrEquals(const string   &in, const string   &ref, int case_sensitive=false);
bool StringEmptyOrEquals(const String16 &in, const String16 &ref, int case_sensitive=false);
bool StringEmptyOrEquals(const String16 &in, const string   &ref, int case_sensitive=false);
bool StringEmptyOrEquals(const string   &in, const string   &ref1, const string   &ref2, int case_sensitive=false);
bool StringEmptyOrEquals(const String16 &in, const String16 &ref1, const String16 &ref2, int case_sensitive=false);
bool StringEmptyOrEquals(const String16 &in, const string   &ref1, const string   &ref2, int case_sensitive=false);
bool StringReplace(string *text, const string &needle, const string &replace);

template <class X> void AccumulateAsciiDigit(X *v, unsigned char c) { *v = *v * 10 + (c - '0'); }
template <class X> bool ArrayEquals(const X *x, const X *y, int l) { for (int i=0; i<l; i++) if (x[i] != y[i]) return false; return true; }
template <class X> int isnl(const X *str);
template <class X> int chompnl(X *str, int len);
template <class X> int chompnl_len(const X *str, int len);
int dirnamelen(const char  *text, int len=0, bool include_slash=false);
int dirnamelen(const short *text, int len=0, bool include_slash=false);
int basedir(const char *path, const char *cmp);
const char  *basename   (const char  *text, int len=0, int *outlen=0);
const char  *nextline   (const char  *text, int len=0, bool final=0, int *outlen=0);
const short *nextline   (const short *text, int len=0, bool final=0, int *outlen=0);
const char  *nextlineraw(const char  *text, int len=0, bool final=0, int *outlen=0);
const short *nextlineraw(const short *text, int len=0, bool final=0, int *outlen=0);
const char  *nextproto  (const char  *text, int len=0, bool final=0, int *outlen=0);
template <class X>       X *nextchar(      X *text, int (*ischar)(int), int len=0, int *outlen=0);
template <class X> const X *nextchar(const X *text, int (*ischar)(int), int len=0, int *outlen=0);
template <class X>       X *nextchar(      X *text, int (*ischar)(int), int (*isquote)(int), int len=0, int *outlen=0);
template <class X> const X *nextchar(const X *text, int (*ischar)(int), int (*isquote)(int), int len=0, int *outlen=0);
template <class X> int  lengthchar(const X *text, int (*ischar)(int), int len=0);
template <class X> int rlengthchar(const X *text, int (*ischar)(int), int len=0);

int Split(const char   *in, int (*ischar)(int), string *left, string *right);
int Split(const string &in, int (*ischar)(int), string *left, string *right);
void Join(string *out, const vector<string> &in);
void Join(string *out, const vector<string> &in, int inB, int inE);
string Join(const vector<string> &strs, const string &separator);
string Join(const vector<string> &strs, const string &separator, int beg_ind, int end_ind);
string strip(const char *s, int (*stripchar)(int), int (*stripchar2)(int)=0);
string togrep(const char *s, int (*grepchar)(int), int (*grepchar2)(int)=0);
string   toconvert(const char     *text, int (*tochar)(int), int (*ischar)(int)=0);
string   toconvert(const string   &text, int (*tochar)(int), int (*ischar)(int)=0);
String16 toconvert(const short    *text, int (*tochar)(int), int (*ischar)(int)=0);
String16 toconvert(const String16 &text, int (*tochar)(int), int (*ischar)(int)=0);
string   toupper(const char     *text);
string   toupper(const string   &text);
String16 toupper(const short    *text);
String16 toupper(const String16 &text);
string   tolower(const char     *text);
string   tolower(const string   &text);
String16 tolower(const short    *text);
String16 tolower(const String16 &text);
string CHexEscape(const string &text);

void localtm(time_t, struct tm *t);
void GMTtm(time_t, struct tm *t);
string logtime(Time t);
int logtime(char *buf, int size);
int logtime(Time time, char *buf, int size);
int logtime(time_t secs, int ms, char *buf, int size);
int logtime(struct tm*, int ms, char *buf, int size);
string logfileday(Time t);
int logfileday(char *buf, int size);
int logfileday(time_t t, char *buf, int size);
int logfileday(struct tm *tm, char *buf, int size);
string logfiledaytime(Time t);
int logfiledaytime(char *buf, int size);
int logfiledaytime(time_t t, char *buf, int size);
int logfiledaytime(struct tm *tm, char *buf, int size);
int httptime(char *buf, int size);
int httptime(time_t time, char *buf, int size);
int httptime(struct tm*, char *buf, int size);
int localhttptime(char *buf, int size);
int localhttptime(time_t time, char *buf, int size);
int localhttptime(struct tm*, char *buf, int size);
string localhttptime(Time t);
int localsmtptime(char *buf, int size);
int localsmtptime(time_t time, char *buf, int size);
int localsmtptime(struct tm*, char *buf, int size);
string localsmtptime(Time t);
int localmboxtime(char *buf, int size);
int localmboxtime(time_t time, char *buf, int size);
int localmboxtime(struct tm*, char *buf, int size);
string localmboxtime(Time t);
int intervaltime(time_t t, int ms, char *buf, int size);
string intervaltime(Time t);
int intervalminutes(time_t t, int ms, char *buf, int size);
string intervalminutes(Time t);

int RFC822TimeZone(const char *text);
Time RFC822Date(const char *text);
Time NumericDate(const char *datetext, const char *timetext, const char *timezone);

Time SinceDayBegan(Time, int gmt_offset_hrs);
const char *LocalTimeZone(Time t=0);
bool IsDaylightSavings(Time t=0);

struct NullAlloc : public Allocator {
    const char *name() { return "NullAlloc"; }
    void *malloc(int size) { return 0; }
    void *realloc(void *p, int size) { return 0; }
    void free(void *p) {}
};

struct MallocAlloc : public Allocator {
    const char *name() { return "MallocAlloc"; }
    void *malloc(int size);
    void *realloc(void *p, int size);
    void free(void *p);
};

struct NewAlloc : public Allocator {
    const char *name() { return "NewAlloc"; }
    void *malloc(int size) { return new char[size]; }
    void *realloc(void *p, int size) { return !p ? malloc(size) : 0; }
    void free(void *p) { delete [] (char *)p; }
};

struct MMapAlloc : public Allocator {
#ifdef _WIN32
    HANDLE file, map; void *addr; long long size;
    MMapAlloc(HANDLE File, HANDLE Map, void *Addr, int Size) : file(File), map(Map), addr(Addr), size(Size) {}
#else
    void *addr; long long size;
    MMapAlloc(void *Addr, long long Size) : addr(Addr), size(Size) {}
#endif
    virtual ~MMapAlloc();
    static MMapAlloc *open(const char *fn, const bool logerror=true, const bool readonly=true, long long size=0);
    
    const char *name() { return "MMapAlloc"; }
    void *malloc(int size) { return 0; }
    void *realloc(void *p, int size) { return 0; }
    void free(void *p) { delete this; }
};

template <int S> struct FixedAlloc : public Allocator {
    const char *name() { return "FixedAlloc"; }
    static const int size = S;
    char buf[S]; int len;
    FixedAlloc() { reset(); }
    virtual void reset() { len=0; }
    virtual void *malloc(int n) { if (len+n > size) FATALf("FixedAlloc::malloc(): %d > %d", len+n, size); char *ret = &buf[len]; len += next_multiple_of_power_of_two(n, 16); return ret; }
    virtual void *realloc(void *p, int n) { if (p) FATALf("FixedAlloc::realoc(%p, %d)", p, size); return this->malloc(n); }
    virtual void free(void *p) {}
};

struct BlockChainAlloc : public Allocator {
    const char *name() { return "BlockChainAlloc"; }
    struct Block { char *buf; int size, len; Block(char *B, int S) : buf(B), size(S), len(0) {} };
    vector<Block> blocks; int block_size, cur_block_ind;
    BlockChainAlloc(int s=1024*1024) : block_size(s), cur_block_ind(-1) {}
    ~BlockChainAlloc() { for (int i=0; i<blocks.size(); i++) delete [] blocks[i].buf; }
    void reset() { for (int i=0; i<blocks.size(); i++) blocks[i].len = 0; cur_block_ind = blocks.size() ? 0 : -1; }
    void *realloc(void *p, int n) { if (p) FATAL("BlockAlloc::realoc(", p, ", ", n, ")"); return this->malloc(n); }
    void *malloc(int n);
    void free(void *p) {}
};

struct ThreadLocalStorage {
    Allocator *alloc;
#ifndef _WIN32
    ThreadLocalStorage() : alloc(0) {}
    ~ThreadLocalStorage() { delete alloc; }
#endif

    static void Init();
    static void Free();
    static void ThreadInit();
    static void ThreadFree();

    static ThreadLocalStorage *Get();
    static Allocator *GetAllocator(bool reset_allocator=true);
};

struct ScopedThreadLocalStorage {
    ScopedThreadLocalStorage()  { ThreadLocalStorage::ThreadInit(); }
    ~ScopedThreadLocalStorage() { ThreadLocalStorage::ThreadFree(); }
};

#ifdef _WIN32
struct Thread {
    typedef int (*CB)(void *);
    HANDLE impl; unsigned id; CB cb; void *arg; bool started;
    Thread() : impl(0), cb(0), arg(0), started(0) {}
    Thread(CB c, void *a) : impl(0), cb(c), arg(a), started(0) {}
    void Open(CB c, void *a) { cb=c; arg=a; }
    void Wait() { WaitForSingleObject(impl, INFINITE); CloseHandle(impl); }
    bool Start() { return (started = (impl = (HANDLE)_beginthreadex(0, 0, Run, this, 0, &id)) != 0); }
    static unsigned long long Id() { return GetCurrentThreadId(); }
    static unsigned __stdcall Run(void *arg) {
        ScopedThreadLocalStorage tls;
        Thread *t = (Thread*)arg;
        return t->cb ? t->cb(t->arg) : 0;
    }
};
struct Mutex {
    CRITICAL_SECTION impl;
    Mutex() { InitializeCriticalSection(&impl); }
    ~Mutex() { DeleteCriticalSection(&impl); }
    void enter() { EnterCriticalSection(&impl); }
    void leave() { LeaveCriticalSection(&impl); }
};
#else /* _WIN32 */
struct Thread {
    typedef int (*CB)(void *);
    pthread_t impl; CB cb; void *arg; bool started;
    Thread() : impl(0), cb(0), arg(0), started(0) {}
    Thread(CB c, void *a) : impl(0), cb(c), arg(a), started(0) {}
    void Open(CB c, void *a) { cb=c; arg=a; }
    void Wait() { pthread_join(impl, 0); }
    bool Start() { return (started = !pthread_create(&impl, 0, Run, this)); }
    static unsigned long long Id() { return (unsigned long long)pthread_self(); }
    static void* Run(void* arg) {
        ScopedThreadLocalStorage tls;
        Thread *t = (Thread*)arg;
        return t->cb ? (void*)(long)t->cb(t->arg) : 0;
    }
};
struct Mutex {
    pthread_mutex_t impl;
    Mutex() { pthread_mutex_init(&impl, 0); }
    ~Mutex() { pthread_mutex_destroy(&impl); }
    void enter() { pthread_mutex_lock(&impl); }
    void leave() { pthread_mutex_unlock(&impl); }
};
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

struct ScopedMutex {
    Mutex &m;
    ScopedMutex(Mutex &M) : m(M) { m.enter(); }
    ~ScopedMutex() { m.leave(); }
};

struct Process {
    int pid; FILE *in, *out;
    virtual ~Process() { Close(); }
    Process() : pid(0), in(0), out(0) {}
    int Open(const char **argv);
    int OpenPTY(const char **argv);
    int Close();
    static void Daemonize(const char *dir="");
    static void Daemonize(FILE *fout, FILE *ferr);
};

struct MIMEType {
    static bool Jpg(const string &mt) { return mt == "image/jpg" || mt == "image/jpeg"; }
    static bool Png(const string &mt) { return mt == "image/png"; }
};

struct FileSuffix {
    static bool HTML (const string &url) { return SuffixMatch(url, ".html", 0) || SuffixMatch(url, ".txt", 0); }
    static bool Image(const string &url) { return Jpg(url) || Png(url) || Gif(url) || Bmp(url); }
    static bool Jpg  (const string &url) { return SuffixMatch(url, ".jpg", 0) || SuffixMatch(url, ".jpeg", 0); }
    static bool Png  (const string &url) { return SuffixMatch(url, ".png", 0); }
    static bool Gif  (const string &url) { return SuffixMatch(url, ".gif", 0); }
    static bool Bmp  (const string &url) { return SuffixMatch(url, ".bmp", 0); }
};

struct File {
    virtual ~File() {}
    virtual bool opened() = 0;
    virtual void close() = 0;
    virtual bool open(const string &path, const string &mode, bool pre_create=0) = 0;
    virtual const char *filename() const = 0;
    virtual int size() = 0;
    virtual void reset() = 0;

    struct Whence { enum { SET=SEEK_SET, CUR=SEEK_CUR, END=SEEK_END }; int x; };
    virtual long long seek(long long offset, int whence) = 0;
    virtual int read(void *buf, int size) = 0;
    virtual int write(const void *buf, int size=-1) = 0;
    virtual bool flush() { return false; }

    struct NextRecord { 
        string buf; bool buf_dirty; int buf_offset, file_offset, record_offset, record_len;
        NextRecord() { reset(); }
        void reset() { buf.clear(); buf_dirty = 0; buf_offset = file_offset = record_offset = record_len = 0; }
        void set_file_offset(int v) { file_offset = v; buf_dirty = 1; }
        typedef const char* (*NextRecordCB)(const char *, int, bool, int *);
        const char *nextrecord(File *f, int *offset, int *nextoffset, NextRecordCB cb); 
    } nr;

    string contents();
    const char *nextline   (int *offset=0, int *nextoffset=0);
    const char *nextlineraw(int *offset=0, int *nextoffset=0);
    const char *nextchunk  (int *offset=0, int *nextoffset=0);
    const char *nextproto  (int *offset=0, int *nextoffset=0, ProtoHeader *phout=0);

    int write(const string &b) { return write(b.c_str(), b.size()); }
    int writeproto(ProtoHeader *hdr, const Proto *msg, bool flush=0);
    int writeproto(const ProtoHeader *hdr, const Proto *msg, bool flush=0);
    int writeprotoflag(const ProtoHeader *hdr, bool flush=0);

    static bool ReadSuccess(File *f, void *out, int len) { return f->read(out, len) == len; }
    static bool SeekSuccess(File *f, long long pos) { return f->seek(pos, Whence::SET) == pos; }
    static bool SeekReadSuccess(File *f, long long pos, void *out, int len) { if (!SeekSuccess(f, pos)) return false; return ReadSuccess(f, out, len); }
};

struct BufferFile : public File {
    string buf, fn; int rdo, wro;
    BufferFile(const char *in, int inlen, const char *FN=0) : buf(in, inlen), fn(FN?FN:""), rdo(0), wro(0) {}
    ~BufferFile() { close(); }

    bool opened() { return true; }
    bool open(const string &path, const string &mode, bool pre_create=0) { return false; }
    const char *filename() const { return fn.c_str(); }
    int size() { return buf.size(); }
    void reset() { rdo=wro=0; nr.reset(); }
    void close() { buf.clear(); reset(); }

    long long seek(long long pos, int whence);
    int read(void *out, int size);
    int write(const void *in, int size=-1);
};

struct LocalFile : public File {
    void *impl; string fn; bool writable;
    virtual ~LocalFile() { close(); }
    LocalFile() : impl(0), writable(0) {}
    LocalFile(const string &path, const string &mode, bool pre_create=0) : impl(0) { open(path, mode, pre_create); }
    static int whencemap(int n);

    static const char Slash;
    static bool mkdir(const string &dir, int mode);
    static int isdirectory(const string &localfilename);
    static string filecontents(const string &localfilename) { return LocalFile(localfilename, "r").contents(); }
    static int writefile(const string &path, const StringPiece &sp) {
        LocalFile file(path, "w");
        return file.opened() ? file.write(sp.data(), sp.size()) : -1;
    }

    bool opened() { return impl; }
    bool open(const string &path, const string &mode, bool pre_create=0);
    const char *filename() const { return fn.c_str(); }
    int size();
    void reset();
    void close();

    long long seek(long long pos, int whence);
    int read(void *buf, int size);
    int write(const void *buf, int size=-1);
    bool flush();
};

template <class X> struct IterT {
    virtual ~IterT() {}
    virtual void reset() {}
    virtual const X *next() = 0;
    template <class Y> void ScanN(Y *out, int N)
    { for (int i=0; i<N; i++) { const X *v=next(); out[i] = v ? Scannable::Scan(Y(), v): 0; } }
};
typedef IterT<char> Iter;

struct DirectoryIter : public Iter {
    typedef map<string, int> Map;
    string pathname;
    Map filemap;
    Map::iterator iter;
    const char *P, *S;
    bool init;
    DirectoryIter() : P(0), S(0), init(0) {}
    DirectoryIter(const char *path, int dirs=0, const char *FilePrefix=0, const char *FileSuffix=0);
    const char *next();
    static void add(void *self, const char *k, int v) { ((DirectoryIter*)self)->filemap[k] = v; }
};

struct FileLineIter : public Iter {
    File *f;
    FileLineIter(File *F) : f(F) {}
    const char *next() { return f->nextline(); }
    void reset() { f->reset(); }
};

struct LocalFileLineIter : public Iter {
    LocalFile f;
    LocalFileLineIter(const string &path) : f(path, "r") {};
    const char *next() { return f.nextline(); }
    void reset() { f.reset(); }
};
   
struct BufferFileLineIter : public Iter {
    BufferFile f;
    BufferFileLineIter(const char *buf, int len) : f(buf, len) {};
    const char *next() { return f.nextline(); }
    void reset() { f.reset(); }
};

template <class X> struct StringLineIterT : public IterT<X> {
    const X *in;
    basic_string<X> buf;
    int len, linelen, offset, flag; bool first;
    struct Flag { enum { BlankLines=1, InPlace=2 }; };
    StringLineIterT(const X *B, int L=0, int F=0) : in(B), len(L), linelen(0), offset(0), flag(F), first(1) {}
    StringLineIterT(const basic_string<X> &S, int F=0) : StringLineIterT(S.c_str(), S.size(), F) {}
    StringLineIterT() : in(0), len(0), linelen(0), offset(-1), flag(0), first(0) {}
    const X *next();
};
typedef StringLineIterT<char>  StringLineIter;
typedef StringLineIterT<short> StringLine16Iter;

template <class X> struct StringWordIterT : public IterT<X> {
    const X *in;
    basic_string<X> buf;
    int len, wordlen, offset, (*IsSpace)(int), (*IsQuote)(int), flag; 
    struct Flag { enum { BlankLines=1, InPlace=2 }; };
    StringWordIterT(const X *instr, int len=0, int (*IsSpace)(int)=0, int(*IsQuote)(int)=0, int Flag=0);
    StringWordIterT() : in(0), len(0), wordlen(0), offset(0), IsSpace(0), flag(0) {};
    const X *next();
    const X *remaining();
};
typedef StringWordIterT<char>  StringWordIter;
typedef StringWordIterT<short> StringWord16Iter;

struct IterWordIter : public Iter {
    Iter *iter;
    StringWordIter word;
    int line_count;
    bool own_iter;
    ~IterWordIter() { if (own_iter) delete iter; }
    IterWordIter(Iter *i, bool owner=false) : iter(i), line_count(0), own_iter(owner) {};
    void reset() { if (iter) iter->reset(); line_count=0; }
    const char *next();
};

struct ArchiveIter : public Iter {
    void *impl, *entry, *dat;
    ArchiveIter(const char *path);
    ArchiveIter(){};
    ~ArchiveIter();
    void skip();
    const char *next();
    long long size();
    const void *data();
};

template <class X, class Y> int Split(const X *in, int (*ischar)(int), int (*isquote)(int), vector<Y> *out) {
    out->clear(); if (!in) return 0;
    StringWordIterT<X> words(in, 0, ischar, isquote);
    for (const X *word = words.next(); word; word = words.next()) out->push_back(Scannable::Scan(Y(), word));
    return out->size();
}
template <class X> int Split(const string   &in, int (*ischar)(int), int (*isquote)(int), vector<X> *out) { return Split<char,  X>(in.c_str(), ischar, isquote, out); }
template <class X> int Split(const string   &in, int (*ischar)(int),                      vector<X> *out) { return Split<char,  X>(in.c_str(), ischar, NULL,    out); }
template <class X> int Split(const char     *in, int (*ischar)(int), int (*isquote)(int), vector<X> *out) { return Split<char,  X>(in, ischar, isquote, out); }
template <class X> int Split(const char     *in, int (*ischar)(int),                      vector<X> *out) { return Split<char,  X>(in, ischar, NULL,    out); }
template <class X> int Split(const String16 &in, int (*ischar)(int), int (*isquote)(int), vector<X> *out) { return Split<short, X>(in.c_str(), ischar, isquote, out); }
template <class X> int Split(const String16 &in, int (*ischar)(int),                      vector<X> *out) { return Split<short, X>(in.c_str(), ischar, NULL,    out); }
template <class X> int Split(const short    *in, int (*ischar)(int), int (*isquote)(int), vector<X> *out) { return Split<short, X>(in, ischar, isquote, out); }
template <class X> int Split(const short    *in, int (*ischar)(int),                      vector<X> *out) { return Split<short, X>(in, ischar, NULL,    out); }

template <class X> int Split(const char   *in, int (*ischar)(int), int (*isquote)(int), set<X> *out) {
    out->clear(); if (!in) return 0;
    StringWordIter words(in, 0, ischar, isquote);
    for (const char *word = words.next(); word; word = words.next()) out->insert(Scannable::Scan(X(), word));
    return out->size();
}
template <class X> int Split(const char   *in, int (*ischar)(int),                      set<X> *out) { return Split(in, ischar, NULL, out); }
template <class X> int Split(const string &in, int (*ischar)(int), int (*isquote)(int), set<X> *out) { return Split(in.c_str(), ischar, isquote, out); }
template <class X> int Split(const string &in, int (*ischar)(int),                      set<X> *out) { return Split(in, ischar, NULL, out); }

struct Timer {
    Time begin;
    Timer() { reset(); }
    Time reset() { Time last_begin=begin; begin=Now(); return last_begin; }
    Time time() const { return Now() - begin; }
    Time time(bool do_reset) {
        if (!do_reset) return time();
        Time last_begin = reset();
        return max(0LL, begin - last_begin);
    }
};

struct PerformanceTimers {
    struct Accumulator {
        string name; Time time;
        Accumulator() : time(0) {}
        Accumulator(const string &n) : name(n), time(0) {}
    };
    vector<Accumulator> timers; Timer cur_timer; int cur_timer_id;
    PerformanceTimers() { cur_timer_id = Create("Default"); }
    int Create(const string &n) { timers.push_back(Accumulator(n)); return timers.size()-1; }
    void AccumulateTo(int timer_id) { timers[cur_timer_id].time += cur_timer.time(true); cur_timer_id = timer_id; }
    string DebugString() { string v; for (int i = 0; i < timers.size(); i++) StrAppend(&v, timers[i].name, " ", timers[i].time / 1000.0, "\n"); return v; }
};

struct Module {
    virtual int Init ()         { return 0; }
    virtual int Start()         { return 0; }
    virtual int Frame(unsigned) { return 0; }
    virtual int Free ()         { return 0; }
};

struct FrameFlag { enum { DontSkip=8 }; };
typedef function<int (LFL::Window*, unsigned, unsigned, bool, int)> FrameCB;
void default_lfapp_window_closed_cb();
}; // namespace LFL

#include "lfapp/math.h"
#include "lfapp/lfexport.h"
#include "lfapp/lftypes.h"
#include "lfapp/audio.h"
#include "lfapp/video.h"
#include "lfapp/input.h"
#include "lfapp/scene.h"
#include "lfapp/assets.h"
#include "lfapp/network.h"
#include "lfapp/camera.h"

namespace LFL {
struct MessageQueue {
    struct Entry { int id; void *opaque; };
    deque<Entry> queue;
    Mutex mutex;
    void write(int n, void *x) { ScopedMutex sm(mutex); Entry e = { n, x }; queue.push_back(e); }
    int read(void** out) { ScopedMutex sm(mutex); if (!queue.size()) return 0; Entry e = PopBack(queue); *out = e.opaque; return e.id; }
};

struct ThreadPool {
    int next_thread;
    vector<Thread*>       thread;
    vector<MessageQueue*> queue;
    ThreadPool() : next_thread(0) {}
    ~ThreadPool() { for (int i=0, n=thread.size(); i<n; i++) { delete queue[i]; delete thread[i]; } }
    void Open(int num) {
        CHECK(thread.empty() && queue.empty());
        thread.resize(num, 0);
        queue .resize(num, 0);
        for (int i=0, n=thread.size(); i<n; i++) {
            queue [i] = new MessageQueue();
            thread[i] = new Thread(HandleMessagesLoop, queue[i]);
        }
    }
    void Start() { for (int i=0, n=thread.size(); i<n; i++) thread[i]->Start(); }
    void Stop()  { for (int i=0, n=thread.size(); i<n; i++) thread[i]->Wait();  }
    void Write(int mid, void *m) {
        queue[next_thread]->write(mid, m);
        next_thread = (next_thread + 1) % thread.size();
    }
    static const int messageQueueCallback = 1;
    static int HandleMessages(void *q);
    static int HandleMessagesLoop(void *q);
};

struct FrameRateLimitter {
    int *target_hz; float avgframe; Timer timer; RollingAvg sleep_bias;
    FrameRateLimitter(int *HZ) : target_hz(HZ), avgframe(0), sleep_bias(32) {}
    void Limit() {
        int since = timer.time(true);
        int targetframe = (int)(1000.0 / *target_hz), sleep = (int)max(0.0, targetframe - since - sleep_bias.avg());
        if (sleep) { Msleep(sleep); sleep_bias.add(timer.time(true) - sleep); }
    }
};

struct Regex {
    struct Match {
        int begin, end;
        string text(const string &t) const { return t.substr(begin, end - begin); }
        float floatval(const string &t) const { return atof(text(t).c_str()); }
    };
    static int Run(const string &pattern, const string &text, vector<Match> *out);
};

struct Base64 {
    Base64();
    string encoding_table, decoding_table; int mod_table[3];
    string Encode(const char *in,   size_t input_length);
    string Decode(const char *data, size_t input_length);
};

struct Crypto {
    string MD5(const string &in);
    string Blowfish(const string &passphrase, const string &in, bool encrypt_or_decrypt);
};

struct LuaContext {
    virtual ~LuaContext() {}
    virtual string Execute(const string &s) = 0;
};
LuaContext *CreateLuaContext();

struct JSContext {
    virtual ~JSContext() {}
    virtual string Execute(const string &s) = 0;
};
JSContext *CreateV8JSContext(Console *js_console=0, LFL::DOM::Node *document=0);

struct Browser {
    virtual void Draw(Box *viewport) = 0;
    virtual void Open(const string &url) = 0;
    virtual void Navigate(const string &url) { Open(url); }
    virtual Asset *OpenImage(const string &url) { return 0; }
    virtual void OpenStyleImport(const string &url) {}
    virtual void MouseMoved(int x, int y) = 0;
    virtual void MouseButton(int b, bool d) = 0;
    virtual void MouseWheel(int xs, int ys) = 0;
    virtual void KeyEvent(int key, bool down) = 0;
    virtual void BackButton() = 0;
    virtual void ForwardButton() = 0;
    virtual void RefreshButton() = 0;
    virtual string GetURL() = 0;
};

struct SystemBrowser { static void open(const char *url); };
struct Clipboard { static const char *get(); static void set(const char *s); };
struct Mouse { static void grabFocus(); static void releaseFocus(); };
struct TouchDevice { static void openKeyboard(); static void closeKeyboard(); };
struct Advertising { static void showAds(); static void hideAds(); };
struct CUDA : public Module { int Init(); };

struct Application : public ::LFApp, public Module {
    string progname, logfilename;
    FILE *logfile;
    FrameCB frame_cb;
    void (*reshaped_cb)(), (*window_closed_cb)();
    Mutex frame_mutex, wait_mutex, log_mutex;
    Timer appTime, frameTime;
    ThreadPool threadPool;
    MessageQueue messageQueue;
    FrameRateLimitter maxfps;
    ValueSet<int> fillMode, grabMode, texMode;
    Audio audio;
    Video video;
    Input input;
    Assets assets;
    Network network;
    Camera camera;
    CUDA cuda;
    Shell shell;
    vector<Module*> modules;
    void LoadModule(Module *M) { modules.push_back(M); M->Init(); }

    Application() : logfile(0), reshaped_cb(0), window_closed_cb(default_lfapp_window_closed_cb),
    maxfps(&FLAGS_target_fps), fillMode(3, GraphicsDevice::Fill, GraphicsDevice::Line, GraphicsDevice::Point),
    grabMode(2, false, true), texMode(2, true, false) { run=1; opened=0; main_thread_id=frames_ran=pre_frames_ran=samples_read=samples_read_last=0; }

    int Create(int argc, const char **argv, const char *source_filename);
    int Init();
    int Start();
    int PreFrame(unsigned clicks, unsigned *mic_samples, bool *camera_sample);
    int PostFrame();
    int Wakeup();
    int Frame();
    int Main();
    int Free();
    int Exiting();
};

#if defined(LFL_QT)
#define main LFL_QT_main
#elif defined(LFL_IPHONE)
#define main iphone_main
int iphone_video_init();
int iphone_video_swap();
int iphone_input(unsigned clicks, unsigned *events);
int iphone_open_browser(const char *url_text);
#endif

extern Application *app;
typedef int (*MainCB)(int argc, const char **argv);

struct NTService {
    static int Install  (const char *name, const char *path);
    static int Uninstall(const char *name);
    static int MainWrapper(const char *name, MainCB mainCB, int argc, const char **argv);
};

}; // namespace LFL

extern "C" int main(int argc, const char **argv);

#endif
