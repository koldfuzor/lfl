/*
 * $Id: video.h 1336 2014-12-08 09:29:59Z justin $
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

#ifndef __LFL_LFAPP_VIDEO_H__
#define __LFL_LFAPP_VIDEO_H__

namespace LFL {
DECLARE_bool(gd_debug);
DECLARE_bool(swap_axis);
DECLARE_int(dots_per_inch);
DECLARE_float(field_of_view);
DECLARE_float(near_plane);
DECLARE_float(far_plane);

struct DrawMode { enum { _2D=0, _3D=1, NullOp=2 }; int m; };
struct TexGen { enum { LINEAR=1, REFLECTION=2 }; };

struct Depth {
  enum { _16=1 }; 
  static int OpenGLID(int d);
};

struct CubeMap {
  enum { PX=1, NX=2, PY=3, NY=4, PZ=5, NZ=6 };
  static int OpenGLID(int target);
};

struct ColorChannel {
  enum { Red=1, Green=2, Blue=3, Alpha=4 };
  static int PixelOffset(int c);
};

struct Pixel {
  enum { RGB32=1, BGR32=2, RGBA=3, BGRA=4,
    RGB24=5, BGR24=6, 
    RGB555=7, BGR555=8, RGB565=9, BGR565=10,
    YUV410P=11, YUV420P=12, YUYV422=13, YUVJ420P=14, YUVJ422P=15, YUVJ444P=16,
    GRAY8=17, GRAYA8=18, LCD=19 };

  static const char *Name(int id);
  static int size(int p);
  static int OpenGLID(int p);

#ifdef LFL_FFMPEG
  static int FromFFMpegId(int fmt);
  static int ToFFMpegId(int fmt);
#endif
};

struct Color {
  float x[4];
  Color() { r()=0; g()=0; b()=0; a()=1; }
  Color(const float *f) { r()=f[0]; g()=f[1]; b()=f[2]; a()=f[3]; }
  Color(double R, double G, double B) { r()=R; g()=G; b()=B; a()=1.0; }
  Color(double R, double G, double B, double A) { r()=R; g()=G; b()=B; a()=A; }
  Color(int R, int G, int B) { r()=R/255.0; g()=G/255.0; b()=B/255.0; a()=1.0; }
  Color(int R, int G, int B, int A) { r()=R/255.0; g()=G/255.0; b()=B/255.0; a()=A/255.0; }
  Color(unsigned v, bool has_alpha=true) { r()=((v>>16)&0xff)/255.0; g()=((v>>8)&0xff)/255.0; b()=(v&0xff)/255.0; a()=(has_alpha ? ((v>>24)&0xff)/255.0 : 1.0); };
  Color(const StringPiece &hs) { *this = Color(strtoul(hs.data(), 0, 16), false); }
  Color(const Color &c, double A) { *this = c; a() = A; }
  Color operator+(const Color &y) const { Color ret = *this; for (int i=0;i<4;i++) ret.x[i] += y.x[i]; return ret; }
  Color operator-(const Color &y) const { Color ret = *this; for (int i=0;i<4;i++) ret.x[i] -= y.x[i]; return ret; }
  bool operator< (const Color &y) const { SortImpl4(x[0], y.x[0], x[1], y.x[1], x[2], y.x[2], x[3], y.x[3]); }
  bool operator==(const Color &y) const { return R()==y.R() && G()==y.G() && B()==y.B() && A()==y.A(); }
  bool operator!=(const Color &y) const { return !(*this == y); }
  bool Transparent() const { return a() == 0; }
  string IntString() const { return StrCat("Color(", R(), ",", G(), ",", B(), ",", A(), ")"); }
  string HexString() const { return StringPrintf("%02X%02X%02X", R(), G(), B()); }
  string DebugString() const { return StringPrintf("%02X%02X%02X%02X", R(), G(), B(), A()); }
  unsigned AsUnsigned() const { return (unsigned char)R()<<24 | (unsigned char)G()<<16 | (unsigned char)B()<<8 | (unsigned char)A(); }
  const float &r() const { return x[0]; }
  const float &g() const { return x[1]; }
  const float &b() const { return x[2]; }     
  const float &a() const { return x[3]; }
  float       &r()       { return x[0]; }     
  float       &g()       { return x[1]; }
  float       &b()       { return x[2]; }     
  float       &a()       { return x[3]; }
  int          R() const { return RoundF(x[0]*255); } 
  int          G() const { return RoundF(x[1]*255); }
  int          B() const { return RoundF(x[2]*255); } 
  int          A() const { return RoundF(x[3]*255); }
  Color r(float v) const { Color c=*this; c.r() = v; return c; }
  Color g(float v) const { Color c=*this; c.g() = v; return c; }
  Color b(float v) const { Color c=*this; c.b() = v; return c; }
  Color a(float v) const { Color c=*this; c.a() = v; return c; }
  void scale(float f) { r() = Clamp(r()*f, 0, 1); g() = Clamp(g()*f, 0, 1); b() = Clamp(b()*f, 0, 1); }
  void ToHSV(float *h, float *s, float *v) const;
  static Color FromHSV(float h, float s, float v);
  static Color Red  (float v=1.0) { return Color(  v, 0.0, 0.0, 0.0); }
  static Color Green(float v=1.0) { return Color(0.0,   v, 0.0, 0.0); }
  static Color Blue (float v=1.0) { return Color(0.0, 0.0,   v, 0.0); }
  static Color Alpha(float v=1.0) { return Color(0.0, 0.0, 0.0,   v); }
  static Color fade(float v);
  static Color Interpolate(Color l, Color r, float mix) { l.scale(mix); r.scale(1-mix); return add(l,r); }
  static Color add(const Color &l, const Color &r) { return Color(Clamp(l.r()+r.r(), 0, 1), Clamp(l.g()+r.g(), 0, 1), Clamp(l.b()+r.b(), 0, 1), Clamp(l.a()+r.a(), 0, 1)); }
  static Color white, black, red, green, blue, cyan, yellow, magenta, grey90, grey80, grey70, grey60, grey50, grey40, grey30, grey20, grey10, clear;
};

struct Material {
  Color ambient, diffuse, specular, emissive;
  Material() {}
  void SetLightColor(const Color &color);
  void SetMaterialColor(const Color &color);
};

struct Light { v4 pos; Material color; };

struct Border {
  int top, right, bottom, left;
  Border() : top(0), right(0), bottom(0), left(0) {}
  Border(int T, int R, int B, int L) : top(T), right(R), bottom(B), left(L) {}
  Border &operator+=(const Border &v) { top+=v.top; right+=v.right; bottom+=v.bottom; left+=v.left; return *this; }
  Border &operator-=(const Border &v) { top-=v.top; right-=v.right; bottom-=v.bottom; left-=v.left; return *this; }
  int Width() const { return right + left; }
  int Height() const { return top + bottom; }
  Border TopBottom() const { return Border(top, 0, bottom, 0); }
  Border LeftRight() const { return Border(0, right, 0, left); }
  string DebugString() const { return StrCat("{", top, ", ", right, ", ", bottom, ", ", left, "}"); }
};

struct Box {
  int x, y, w, h;
  void clear() { x=y=w=h=0; }
  Box() : x(0), y(0), w(0), h(0) {}
  Box(int W, int H) : x(0), y(0), w(W), h(H) {}
  Box(const point &D) : x(0), y(0), w(D.x), h(D.y) {}
  Box(int X, int Y, int W, int H) : x(X), y(Y), w(W), h(H) {}
  Box(const point &P, int W, int H) : x(P.x), y(P.y), w(W), h(H) {}
  Box(const point &P, const point &D) : x(P.x), y(P.y), w(D.x), h(D.y) {}
  Box(float X, float Y, float W, float H, bool round);
  Box(const float *v4, bool round);

  virtual const FloatContainer *AsFloatContainer() const { return 0; }
  virtual       FloatContainer *AsFloatContainer()       { return 0; }
  virtual float baseleft (float py, float ph, int *ao=0) const { if (ao) *ao=-1; return x;   }
  virtual float baseright(float py, float ph, int *ao=0) const { if (ao) *ao=-1; return x+w; }
  virtual string DebugString() const;
  point Position () const { return point(x, y); }
  point Dimension() const { return point(w, h); }

  Box &SetX        (int nx)         { x=nx;         return *this; }
  Box &SetY        (int ny)         { y=ny;         return *this; }
  Box &SetPosition (const v2 &p)    { x=p.x; y=p.y; return *this; }
  Box &SetPosition (const point &p) { x=p.x; y=p.y; return *this; }
  Box &SetDimension(const v2 &p)    { w=p.x; h=p.y; return *this; }
  Box &SetDimension(const point &p) { w=p.x; h=p.y; return *this; }
  Box &operator+=(const point &p) { x+=p.x; y+=p.y; return *this; }
  Box &operator-=(const point &p) { x-=p.x; y-=p.y; return *this; }
  Box  operator+ (const point &p) const { return Box(x+p.x, y+p.y, w, h); }
  Box  operator- (const point &p) const { return Box(x-p.x, y-p.y, w, h); }
  int top    () const { return y+h; }
  int right  () const { return x+w; }
  int centerX() const { return x+w/2; }
  int centerY() const { return y+h/2; }
  v2  center () const { return v2(centerX(), centerY()); }
  Box center(const Box &w) const { return Box(centerX(w.w), centerY(w.h), w.w, w.h); }
  int centerX (int cw)   const { return x+(w-cw)/2; }
  int centerY (int ch)   const { return y+(h-ch)/2; }
  int percentX(float px) const { return x+w*px; }
  int percentY(float py) const { return y+h*py; }
  bool Empty() const { return !w && !h; }
  bool within(const point &p) const { return p.x >= x && p.x < right() && p.y >= y && p.y < top(); }
  bool operator==(const Box &c) const { return x == c.x && y == c.y && w == c.w && h == c.h; }
  bool operator!=(const Box &c) const { return !(*this == c); }
  bool operator<(const Box &c) const { SortImpl4(x, c.x, y, c.y, w, c.w, h, c.h); }
  void scale(float xf, float yf) { x = RoundF(x*xf); w = RoundF(w*xf); y = RoundF(y*yf); h = RoundF(h*yf); }
  void swapaxis(int width, int height) { x += w; y += h; swap(x,y); swap(w,h); y = width - y; x = height - x; } 
  void AddBorder(const Border &b) { *this = AddBorder(*this, b); }
  void DelBorder(const Border &b) { *this = DelBorder(*this, b); }
  Box Intersect(const Box &w) const { Box ret(max(x, w.x), max(y, w.y), min(right(), w.right()), min(top(), w.top())); ret.w -= ret.x; ret.h -= ret.y; return (ret.w >= 0 && ret.h >= 0) ? ret : Box(); }
  Box BottomLeft(const Box &sub) const { return Box(x+sub.x, y+sub.y,           sub.w, sub.h); }
  Box    TopLeft(const Box &sub) const { return Box(x+sub.x, top()-sub.y-sub.h, sub.w, sub.h); }
  point  TopLeft   () const { return point(x,       top()); }
  point  TopRight  () const { return point(right(), top()); }
  point BottomLeft () const { return point(x,       y);     }
  point BottomRight() const { return point(right(), y);     }
  void Draw(const float *texcoord=0) const;
  void DrawCrimped(const float *texcoord, int orientation, float scrollX=0, float scrollY=0) const;

  static float ScrollCrimped(float tex0, float tex1, float scroll, float *min, float *mid1, float *mid2, float *max);
  static bool   VerticalIntersect(const Box &w1, const Box &w2) { return w1.y < (w2.y + w2.h) && w2.y < (w1.y + w1.h); }
  static bool HorizontalIntersect(const Box &w1, const Box &w2) { return w1.x < (w2.x + w2.w) && w2.x < (w1.x + w1.w); }
  static Box Add(const Box &w, const point &p) { return Box(w.x+p.x, w.y+p.y, w.w, w.h); }
  static Box AddBorder(const Box &w, int xb, int yb) { return Box(w.x-RoundF(xb/2.0, 1), w.y-RoundF(yb/2.0, 1), max(0,w.w+xb), max(0,w.h+yb)); }
  static Box DelBorder(const Box &w, int xb, int yb) { return Box(w.x+RoundF(xb/2.0, 1), w.y+RoundF(yb/2.0, 1), max(0,w.w-xb), max(0,w.h-yb)); }
  static Box AddBorder(const Box &w, int tb, int rb, int bb, int lb) { return Box(w.x-lb, w.y-bb, max(0,w.w+lb+rb), max(0,w.h+tb+bb)); }
  static Box DelBorder(const Box &w, int tb, int rb, int bb, int lb) { return Box(w.x+lb, w.y+bb, max(0,w.w-lb-rb), max(0,w.h-tb-bb)); }
  static Box AddBorder(const Box &w, const Border &b) { return AddBorder(w, b.top, b.right, b.bottom, b.left); }
  static Box DelBorder(const Box &w, const Border &b) { return DelBorder(w, b.top, b.right, b.bottom, b.left); }
  static Box TopBorder(const Box &w, const Border &b) { return Box(w.x, w.top()-b.top, w.w, b.top); }
  static Box BotBorder(const Box &w, const Border &b) { return Box(w.x, w.y,           w.w, b.bottom); }
};

struct Box3 {
  Box v[3];
  Box3() {}
  Box3(const Box &cont, const point &pb, const point &pe, int first_line_height, int last_line_height);

  const Box *begin() const { return &v[0]; }
  const Box *end()   const { return &v[0] + 3; }
  const Box &operator[](int i) const { return v[i]; }
  Box       &operator[](int i)       { return v[i]; }
  bool Null() const { return !v[0].h; }
  void Clear() { for (int i=0; i<3; i++) v[i].clear(); }
  Box3 &operator+=(const point &p) { for (int i=0; i<3; i++) if (!i || v[i].h) v[i] += p; return *this; }
  Box3 &operator-=(const point &p) { for (int i=0; i<3; i++) if (!i || v[i].h) v[i] -= p; return *this; }
  string DebugString() const { string ret = "Box3{"; for (int i=0; i<3; i++) if (!i || v[i].h) StrAppend(&ret, v[i].DebugString(), ", "); return ret + "}"; }
  void AddBorder(const Border &b, Box3 *out) const { for (int i=0; i<3; i++) if (!i || v[i].h) out->v[i] = Box::AddBorder(v[i], b); }
  void DelBorder(const Border &b, Box3 *out) const { for (int i=0; i<3; i++) if (!i || v[i].h) out->v[i] = Box::DelBorder(v[i], b); }
  bool VerticalIntersect(const Box &w) const { for (int i=0; i<3; i++) if (v[i].h && Box::VerticalIntersect(v[i], w)) return 1; return 0; }
  void Draw(const point &p=point(), const Color *c=0) const;
  Box BoundingBox() const;
};

struct Drawable {
  struct Attr { 
    Font *font=0;
    const Color *fg=0, *bg=0;
    const Texture *tex=0;
    const LFL::Box *scissor=0;
    bool underline=0, overline=0, midline=0, blink=0, blend=0;
    constexpr Attr(Font *F=0, const Color *FG=0, const Color *BG=0, bool UL=0, bool B=0) : font(F), fg(FG), bg(BG), underline(UL), blend(B) {}
    bool operator==(const Attr &y) const { return font==y.font && fg==y.fg && bg==y.bg && tex==y.tex && scissor==y.scissor && underline==y.underline && overline==y.overline && midline==y.midline && blink==y.blink && blend == y.blend; }
    bool operator!=(const Attr &y) const { return !(*this == y); }
    void Clear() { font=0; fg=bg=0; tex=0; scissor=0; underline=overline=midline=blink=0; }
    string DebugString() const;
  };
  struct AttrSource { virtual const Attr *GetAttr(int attr_id) const = 0; };
  struct AttrVec : public AttrSource, public vector<Attr> {
    Attr current;
    AttrSource *source=0;
    RefSet font_refs;
    AttrVec() {}
    const Attr *GetAttr(int attr_id) const { return source ? source->GetAttr(attr_id) : &(*this)[attr_id-1]; }
    int GetAttrId(const Attr &v) { CHECK(!source); if (empty() || this->back() != v) Insert(v); return size(); }
    void Insert(const Attr &v);
  };

  virtual int  Id()                                            const { return 0; }
  virtual int  TexId()                                         const { return 0; }
  virtual bool Wide()                                          const { return 0; }
  virtual int  LeftBearing(                   const Attr *A=0) const { return 0; }
  virtual int  Ascender   (const LFL::Box *B, const Attr *A=0) const { return B ? B->h : 0; }
  virtual int  Advance    (const LFL::Box *B, const Attr *A=0) const { return B ? B->w : 0; }
  virtual int  Layout     (      LFL::Box *B, const Attr *A=0) const { return B ? B->w : 0; }
  virtual void Draw       (const LFL::Box &B, const Attr *A=0) const = 0;
};

struct DrawableNullOp : public Drawable { void Draw(const LFL::Box &B, const Drawable::Attr *A=0) const {} };
#define DrawableNop() Singleton<DrawableNullOp>::Get()

struct DrawableBox {
  LFL::Box box;
  const Drawable *drawable;
  int attr_id, line_id;
  DrawableBox(                   const Drawable *D = 0, int A = 0, int L = -1) :         drawable(D), attr_id(A), line_id(L) {}
  DrawableBox(const LFL::Box &B, const Drawable *D = 0, int A = 0, int L = -1) : box(B), drawable(D), attr_id(A), line_id(L) {}
  bool operator<(const DrawableBox &x) const { return box < x.box; }
  int LeftBound (const Drawable::Attr *A) const { return box.x - (drawable ? drawable->LeftBearing(A) : 0); }
  int RightBound(const Drawable::Attr *A) const { return box.x + (drawable ? (drawable->Advance(&box, A) - drawable->LeftBearing(A)) : box.w); }
  int TopBound  (const Drawable::Attr *A) const { return box.y + (drawable ? drawable->Ascender(&box, A) : box.h); }
  int Id() const { return drawable ? drawable->Id() : 0; }
};
typedef ArrayMemberPairSegmentIter<DrawableBox, int, &DrawableBox::attr_id, &DrawableBox::line_id> DrawableBoxIterator;
typedef ArrayMemberSegmentIter    <DrawableBox, int, &DrawableBox::attr_id>                        DrawableBoxRawIterator;

struct Texture : public Drawable {
  static const int preferred_pf;
  unsigned ID;
  unsigned char *buf;
  bool owner, buf_owner;
  int width, height, pf, cubemap;
  float coord[4];

  Texture(int w=0, int h=0, int PF=preferred_pf, unsigned id=0) : ID(id), buf(0), owner(0), buf_owner(1), width(w), height(h), pf(PF), cubemap(0) { Coordinates(coord,1,1,1,1); }
  Texture(int w,   int h,   int PF,           unsigned char *B) : ID(0),  buf(B), owner(0), buf_owner(0), width(w), height(h), pf(PF), cubemap(0) { Coordinates(coord,1,1,1,1); }
  Texture(const Texture &t) : ID(t.ID), buf(t.buf), owner(0), buf_owner(buf?0:1), width(t.width), height(t.height), pf(t.pf), cubemap(t.cubemap) { memcpy(&coord, t.coord, sizeof(coord)); }
  virtual ~Texture() { ClearBuffer(); if (owner) ClearGL(); }

  void Bind() const;
  int TexId() const { return ID; }
  string DebugString() const { return StrCat("Texture(", width, ", ", height, ", ", Pixel::Name(pf), ")"); }
  string HexDump() const { string v; for (int ls=LineSize(), i=0; i<height; i++) StrAppend(&v, Vec<unsigned char>::Str(buf+i*ls, ls, "%02x"), "\n"); return v; }
  point Dimension() const { return point(width, height); }
  int PixelSize() const { return Pixel::size(pf); }
  int LineSize() const { return width * PixelSize(); }
  int BufferSize() const { return height * LineSize(); }
  int GLPixelType() const { return Pixel::OpenGLID(pf); }
  int GLTexType() const { return CubeMap::OpenGLID(cubemap); }
  int GLBufferType() const;

  /// ClearGL() is thread-safe.
  void ClearGL();
  void RenewGL() { ClearGL(); Create(width, height); }
  void ClearBuffer() { if (buf_owner) delete [] buf; buf = 0; buf_owner = 1; }
  unsigned char *NewBuffer() const { return new unsigned char [BufferSize()](); }
  unsigned char *RenewBuffer() { ClearBuffer(); buf = NewBuffer(); return buf; }
  unsigned char *ReleaseBuffer() { unsigned char *ret=0; swap(ret, buf); ClearBuffer(); return ret; }

  struct Flag { enum { CreateGL=1, CreateBuf=2, FlipY=4, Resample=8 }; };
  void Create      (int PF=0)               { Create(width, height, PF); }
  void Create      (int W, int H, int PF=0) { Resize(W, H, PF, Flag::CreateGL); }
  void CreateBacked(int W, int H, int PF=0) { Resize(W, H, PF, Flag::CreateGL | Flag::CreateBuf); }
  void Resize(int W, int H, int PF=0, int flag=0);

  void AssignBuffer(Texture *t, bool become_owner=0) { AssignBuffer(t->buf, point(t->width, t->height), t->pf, become_owner); if (become_owner) t->buf_owner=0; }
  void AssignBuffer(      unsigned char *B, const point &dim, int PF, bool become_owner=0) { buf=B; width=dim.x; height=dim.y; pf=PF; buf_owner=become_owner; }
  void LoadBuffer  (const unsigned char *B, const point &dim, int PF, int linesize, int flag=0);
  void UpdateBuffer(const unsigned char *B, const point &dim, int PF, int linesize, int flag=0);
  void UpdateBuffer(const unsigned char *B, const ::LFL::Box &box, int PF, int linesize, int blit_flag=0);
  void FlipBufferY() { Texture t; t.LoadBuffer(buf, Dimension(), pf, LineSize(), Flag::FlipY); ClearBuffer(); swap(buf, t.buf); }

  void LoadGL  (const MultiProcessTextureResource&);
  void LoadGL  (const unsigned char *B, const point &dim, int PF, int linesize, int flag=0);
  void UpdateGL(const unsigned char *B, const ::LFL::Box &box, int flag=0);
  void UpdateGL(const ::LFL::Box &b, int flag=0) { return UpdateGL(buf ? (buf+(b.y*width+b.x)*PixelSize()) : 0, b, flag); }
  void UpdateGL() { UpdateGL(LFL::Box(0, 0, width, height)); }
  void LoadGL() { LoadGL(buf, point(width, height), pf, LineSize()); }
  void DumpGL(unsigned tex_id=0);

  virtual int Id() const { return 0; }
  virtual int LayoutAtPoint(const point &p, LFL::Box *out) const { *out = LFL::Box(p, width, height); return width; } 
  virtual void Draw(const LFL::Box &B, const Drawable::Attr *A=0) const { Bind(); B.Draw(coord); }
  virtual void DrawCrimped(const LFL::Box &B, int ort, float sx, float sy) const { Bind(); B.DrawCrimped(coord, ort, sx, sy); }

  void Screenshot();
  void ScreenshotBox(const Box &b, int flag);
  void ToIplImage(_IplImage *out);
#ifdef __APPLE__
  CGContextRef CGBitMap();
  CGContextRef CGBitMap(int X, int Y, int W, int H);
#endif
#ifdef WIN32
  HBITMAP CreateGDIBitMap(HDC dc);
#endif
  static void Coordinates(float *texcoord, int w, int h, int wd, int hd);
  static const int CoordMinX, CoordMinY, CoordMaxX, CoordMaxY;
};

struct DepthTexture {
  unsigned ID;
  int width, height, df;
  DepthTexture(int w=0, int h=0, int DF=Depth::_16, unsigned id=0) : ID(id), width(w), height(h), df(DF) {}

  struct Flag { enum { CreateGL=1 }; };
  void Create(int W, int H, int DF=0) { Resize(W, H, DF, Flag::CreateGL); }
  void Resize(int W, int H, int DF=0, int flag=0);
};

struct FrameBuffer {
  unsigned ID;
  int width, height;
  Texture tex;
  DepthTexture depth;
  FrameBuffer(int w=0, int h=0, unsigned id=0) : ID(id), width(w), height(h) {}
  void Reset() { ID=width=height=0; tex=Texture(); depth=DepthTexture(); }

  struct Flag { enum { CreateGL=1, CreateTexture=2, CreateDepthTexture=4, ReleaseFB=8, NoClampToEdge=16 }; };
  void Create(int W, int H, int flag=0) { Resize(W, H, Flag::CreateGL | flag); }
  void Resize(int W, int H, int flag=0);

  void AllocTexture(Texture *out, bool clamp_to_edge=true);
  void AllocTexture(unsigned *out, bool clamp_to_edge=true) { Texture tex; AllocTexture(&tex, clamp_to_edge); *out = tex.ID; } 
  void AllocDepthTexture(DepthTexture *out);

  void Attach(int ct=0, int dt=0);
  void Release();
};

struct ShaderDefines {
  string text;
  bool vertex_color, normals, tex_2d, tex_cube;
  ShaderDefines(                 bool vc=0, bool n=0, bool t2D=0, bool tC=0) : ShaderDefines("", vc, n, t2D, tC) {}
  ShaderDefines(const string &t, bool vc=0, bool n=0, bool t2D=0, bool tC=0) : text(t), vertex_color(vc), normals(n), tex_2d(t2D), tex_cube(tC) {
    if (vertex_color) StrAppend(&text, "#define VERTEXCOLOR\r\n");
    if (normals)      StrAppend(&text, "#define NORMALS\r\n");
    if (tex_2d)       StrAppend(&text, "#define TEX2D\r\n");
    if (tex_cube)     StrAppend(&text, "#define TEXCUBE\r\n");
  }
};

struct Shader {
  static const int MaxVertexAttrib = 4;
  string name;
  float scale=0;
  int unused_attrib_slot[MaxVertexAttrib];
  bool dirty_material=0, dirty_light_pos[4], dirty_light_color[4];
  int ID=0, slot_position=-1, slot_normal=-1, slot_tex=-1, slot_color=-1, uniform_modelview=-1, uniform_modelviewproj=-1,
      uniform_tex=-1,uniform_cubetex=-1, uniform_normalon=-1, uniform_texon=-1, uniform_coloron=-1, uniform_cubeon=-1,
      uniform_colordefault=-1, uniform_material_ambient=-1, uniform_material_diffuse=-1, uniform_material_specular=-1,
      uniform_material_emission=-1, uniform_light0_pos=-1, uniform_light0_ambient=-1, uniform_light0_diffuse=-1,
      uniform_light0_specular=-1;
  Shader() { memzeros(dirty_light_pos); memzeros(dirty_light_color); }

  static int Create(const string &name, const string &vertex_shader, const string &fragment_shader, const ShaderDefines&, Shader *out);
  static int CreateShaderToy(const string &name, const string &fragment_shader, Shader *out);
  int GetUniformIndex(const string &name);
  void SetUniform1i(const string &name, float v);
  void SetUniform1f(const string &name, float v);
  void SetUniform2f(const string &name, float v1, float v2);
  void SetUniform3f(const string &name, float v1, float v2, float v3);
  void SetUniform4f(const string &name, float v1, float v2, float v3, float v4);
  void SetUniform3fv(const string &name, const float *v);
  void SetUniform3fv(const string &name, int n, const float *v);

  static void SetGlobalUniform1f(const string &name, float v);
  static void SetGlobalUniform2f(const string &name, float v1, float v2);
};

#ifndef LFL_QT
struct GraphicsDevice {
#else
struct GraphicsDevice : public QOpenGLFunctions {
#endif
  static const int Float, Points, Lines, LineLoop, Triangles, TriangleStrip, Polygon, Texture2D, UnsignedInt;
  static const int Ambient, Diffuse, Specular, Emission, Position;
  static const int One, SrcAlpha, OneMinusSrcAlpha, OneMinusDstColor;
  static const int Fill, Line, Point, GLPreferredBuffer, GLInternalFormat;

  int default_draw_mode = DrawMode::_2D, draw_mode = 0, default_framebuffer = 0;
  string vertex_shader, pixel_shader;
  bool blend_enabled = 0;
  Shader *shader = 0;
  vector<Color> default_color;
  vector<vector<Box> > scissor_stack;
  GraphicsDevice() : scissor_stack(1) {}

  virtual void Init() = 0;
  virtual bool ShaderSupport() = 0;
  virtual void EnableTexture() = 0;
  virtual void DisableTexture() = 0;
  virtual void EnableLighting() = 0;
  virtual void DisableLighting() = 0;
  virtual void EnableNormals() = 0;
  virtual void DisableNormals() = 0;
  virtual void EnableVertexColor() = 0;
  virtual void DisableVertexColor() = 0;
  virtual void EnableLight(int n) = 0;
  virtual void DisableLight(int n) = 0;
  virtual void DisableCubeMap() = 0;
  virtual void BindCubeMap(int n) = 0;
  virtual void TextureGenLinear() = 0;
  virtual void TextureGenReflection() = 0;
  virtual void Material(int t, float *color) = 0;
  virtual void Light(int n, int t, float *color) = 0;
  virtual void BindTexture(int t, int n) = 0;
  virtual void ActiveTexture(int n) = 0;
  virtual bool VertexPointer(int m, int t, int w, int o, float *verts, int l, int *out, bool dirty, int pt=0) = 0;
  virtual void TexPointer   (int m, int t, int w, int o, float *tex,   int l, int *out, bool dirty) = 0;
  virtual void ColorPointer (int m, int t, int w, int o, float *verts, int l, int *out, bool dirty) = 0;
  virtual void NormalPointer(int m, int t, int w, int o, float *verts, int l, int *out, bool dirty) = 0;
  virtual void Color4f(float r, float g, float b, float a) = 0;
  virtual void UpdateColor() = 0;
  virtual void MatrixProjection() = 0;
  virtual void MatrixModelview() = 0;
  virtual void LoadIdentity() = 0;
  virtual void PushMatrix() = 0;
  virtual void PopMatrix() = 0;
  virtual void GetMatrix(m44 *out) = 0;
  virtual void PrintMatrix() = 0;
  virtual void Scalef(float x, float y, float z) = 0;
  virtual void Rotatef(float angle, float x, float y, float z) = 0;
  virtual void Ortho(float l, float r, float b, float t, float nv, float fv) = 0;
  virtual void Frustum(float l, float r, float b, float t, float nv, float fv) = 0;
  virtual void Mult(const float *m) = 0;
  virtual void Translate(float x, float y, float z) = 0;
  virtual void UseShader(Shader *shader) = 0;
  virtual void DrawElements(int pt, int np, int it, int o, void *index, int l, int *out, bool dirty) = 0;
  virtual void DrawArrays(int t, int o, int n) = 0;
  virtual void DeferDrawArrays(int t, int o, int n) = 0;
  virtual void ClearDeferred() {}

  // Shader interface
  int CreateProgram();
  int CreateShader(int t);
  void ShaderSource(int shader, int count, const char **source, int *len);
  void CompileShader(int shader);
  void AttachShader(int prog, int shader);
  void BindAttribLocation(int prog, int loc, const string &name);
  void LinkProgram(int prog);
  void GetProgramiv(int p, int t, int *out);
  void GetIntegerv(int t, int *out);
  int GetAttribLocation(int prog, const string &name);
  int GetUniformLocation(int prog, const string &name);
  void Uniform1i(int u, int v);
  void Uniform1f(int u, float v);
  void Uniform2f(int u, float v1, float v2);
  void Uniform3f(int u, float v1, float v2, float v3);
  void Uniform4f(int u, float v1, float v2, float v3, float v4);
  void Uniform3fv(int u, int n, const float *v);

  // Common layer
  void Flush();
  void Clear();
  void ClearColor(const Color &c);
  void FillColor(const Color &c) { DisableTexture(); SetColor(c); };
  void SetColor(const Color &c) { Color4f(c.r(), c.g(), c.b(), c.a()); }
  void PushColor(const Color &c) { PushColor(); SetColor(c); }
  void PushColor();
  void PopColor();
  void PointSize(float n);
  void LineWidth(float n);
  void GenTextures(int t, int n, unsigned *out);
  void DelTextures(int n, const unsigned *id);
  void DelTexture(unsigned id) { DelTextures(1, &id); }
  void CheckForError(const char *file, int line);
  void EnableDepthTest();
  void DisableDepthTest();
  void EnableBlend();
  void DisableBlend();
  void BlendMode(int sm, int tm);
  void RestoreViewport(int drawmode);
  void DrawMode(int drawmode, bool flush=1);
  void DrawMode(int drawmode, int W, int H, bool flush=1);
  void EnableLayering() { DisableDepthTest(); DisableLighting(); EnableBlend(); EnableTexture(); }
  void LookAt(const v3 &pos, const v3 &targ, const v3 &up);
  void ViewPort(Box w);
  void Scissor(Box w);
  void PushScissorOffset(const Box &t, const Box &w) { PushScissor(Box(w.x-t.x, w.y-t.y, w.w, w.h)); }
  void PushScissor(Box w);
  void PopScissor();
  void PushScissorStack();
  void PopScissorStack();
  void DrawPixels(const Box &b, const Texture &tex);
  void GenRenderBuffers(int n, unsigned *out);
  void BindRenderBuffer(int id);
  void RenderBufferStorage(int d, int w, int h);
  void GenFrameBuffers(int n, unsigned *out);
  void BindFrameBuffer(int id);
  void FrameBufferTexture(int id);
  void FrameBufferDepthTexture(int id);
  int CheckFrameBufferStatus();

  static int VertsPerPrimitive(int gl_primtype);
};

extern Window *screen;
struct Window : public NativeWindow {
  typedef function<void(Window*)> StartCB;
  typedef function<int(Window*, bool, int)> FrameCB;
  GraphicsDevice *gd=0;
  point mouse, mouse_wheel;
  string caption;
  StartCB start_cb;
  FrameCB frame_cb;
  Timer frame_time;
  RollingAvg<unsigned> fps;
  Entity *cam=0;
  BindMap *binds=0;
  Dialog *top_dialog=0;
  vector<Dialog*> dialogs;
  vector<GUI*> mouse_gui;
  vector<KeyboardGUI*> keyboard_gui;
  vector<InputController*> input_bind;
  Console *lfapp_console=0;

  Window();
  virtual ~Window();

  void SetCaption(const string &c);
  void SetResizeIncrements(float x, float y);
  void SetTransparency(float v);
  void Reshape(int w, int h);
  void Reshaped(int w, int h);
  void Closed()      { Window::Close(this); }
  void Minimized()   { minimized=1; }
  void UnMinimized() { minimized=0; }
  void ResetGL();
  void SwapAxis();
  int  Frame(unsigned clicks, int flag);
  void RenderToFrameBuffer(FrameBuffer *fb);

  void ClearEvents();
  void ClearGesture();
  void ClearMouseGUIEvents();
  void ClearKeyboardGUIEvents();
  void ClearInputBindEvents();
  void InitLFAppConsole();
  void DrawDialogs();

  LFL::Box Box() const { return LFL::Box(0, 0, width, height); }
  LFL::Box Box(float xs, float ys) const { return LFL::Box(0, 0, width*xs, height*ys); }
  LFL::Box Box(float xp, float yp, float xs, float ys, float xbl=0, float ybt=0, float xbr=-INFINITY, float ybb=-INFINITY) const;

  typedef unordered_map<void*, Window*> WindowMap;
  static WindowMap active;
  static Window *Get() { return screen; }
  static Window *Get(void *id);
  static bool Create(Window *W);
  static void Close(Window *W);
  static void MakeCurrent(Window *W);
};

struct Scissor {
  Scissor(int x, int y, int w, int h) { screen->gd->PushScissor(Box(x, y, w, h)); }
  Scissor(const Box &w) { screen->gd->PushScissor(w); }
  ~Scissor()            { screen->gd->PopScissor(); }
};

struct ScissorStack {
  ScissorStack()  { screen->gd->PushScissorStack(); }
  ~ScissorStack() { screen->gd->PopScissorStack(); }
};

struct ScopedDrawMode {
  int prev_mode; bool nop;
  ScopedDrawMode(int dm) : prev_mode(screen->gd->draw_mode), nop(dm == DrawMode::NullOp) { if (!nop) screen->gd->DrawMode(dm,        0); }
  ~ScopedDrawMode()                                                                      { if (!nop) screen->gd->DrawMode(prev_mode, 0); }
};

struct Video : public Module {
  Shader shader_default, shader_normals, shader_cubemap, shader_cubenorm;
  int opengles_version = 2;
  bool opengl_framebuffer = 1, opengles_cubemap = 1;
  Module *impl = 0;

  int Init();
  int Free();
  int Swap();

  static void *BeginGLContextCreate(Window *);
  static void *CompleteGLContextCreate(Window *, void *gl_context);
  static void CreateGraphicsDevice(Window *);
  static void InitGraphicsDevice(Window *);
  static void InitFonts();
  static int InitFontWidth();
  static int InitFontHeight();
};

struct VideoResamplerInterface {
  int s_fmt=0, d_fmt=0, s_width=0, d_width=0, s_height=0, d_height=0;
  virtual ~VideoResamplerInterface() {}
  virtual bool Opened()                                                                                           = 0;
  virtual void Open(int sw, int sh, int sf, int dw, int dh, int df)                                               = 0;
  virtual void Resample(const unsigned char *s, int sls, unsigned char *d, int dls, bool flip_x=0, bool flip_y=0) = 0;
};

struct SimpleVideoResampler : public VideoResamplerInterface {
  virtual ~SimpleVideoResampler() {}
  virtual bool Opened();
  virtual void Open(int sw, int sh, int sf, int dw, int dh, int df);
  virtual void Resample(const unsigned char *s, int sls, unsigned char *d, int dls, bool flip_x=0, bool flip_y=0);

  static bool Supports(int fmt);
  static void CopyPixel(int s_fmt, int d_fmt, const unsigned char *sp, unsigned char *dp, bool sxb, bool sxe, int flag=0);
  static void RGB2BGRCopyPixels(unsigned char *dst, const unsigned char *src, int l, int bpp);

  struct Flag { enum { FlipY=1, TransparentBlack=2, ZeroOnly=4 }; };

  static void Blit(const unsigned char *src, unsigned char *dst, int w, int h,
                   int sf, int sls, int sx, int sy,
                   int df, int dls, int dx, int dy, int flag=0);

  static void Filter(unsigned char *dst, int w, int h,
                     int pf, int ls, int x, int y, Matrix *kernel, int channel, int flag=0);

  static void CopyColorChannelsToMatrix(const unsigned char *buf, int w, int h,
                                        int pw, int ls, int x, int y, Matrix *out, int po);
  static void CopyMatrixToColorChannels(const Matrix *M, int w, int h,
                                        int pw, int ls, int x, int y, unsigned char *out, int po);
};

#ifdef LFL_FFMPEG
struct FFMPEGVideoResampler : public SimpleVideoResampler {
  void *conv; bool simple_resampler_passthru;
  FFMPEGVideoResampler() : conv(0), simple_resampler_passthru(0) {}
  virtual ~FFMPEGVideoResampler();
  virtual bool Opened();
  virtual void Open(int sw, int sh, int sf, int dw, int dh, int df);
  virtual void Resample(const unsigned char *s, int sls, unsigned char *d, int dls, bool flip_x=0, bool flip_y=0);
};
typedef FFMPEGVideoResampler VideoResampler;
#else
typedef SimpleVideoResampler VideoResampler;
#endif

}; // namespace LFL
#endif // __LFL_LFAPP_VIDEO_H__
