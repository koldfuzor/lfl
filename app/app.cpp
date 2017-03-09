/*
 * $Id: lfapp.cpp 1335 2014-12-02 04:13:46Z justin $
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

#include "core/app/gui.h"
#include "core/app/ipc.h"

#include <time.h>
#include <fcntl.h>

#ifdef LFL_WINDOWS
#include <ShlObj.h>
#include <Windns.h>
#else
#include <signal.h>
#include <pthread.h>
#include <dlfcn.h>
#include <sys/resource.h>
#endif

#ifdef LFL_APPLE
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef LFL_ANDROID
#include <android/log.h>
#endif

extern "C" LFApp*       GetLFApp()                 { return LFL::app; }
extern "C" LFAppWindow* GetLFAppWindow()           { return LFL::app->focused; }
extern "C" const char*  GetLFAppSaveDir()          { return LFL::app->savedir.c_str(); }
extern "C" int          LFAppMain()                { return LFL::app->Main(); }
extern "C" int          LFAppMainLoop()            { return LFL::app->MainLoop(); }
extern "C" int          LFAppFrame(bool handle_ev) { return LFL::app->EventDrivenFrame(handle_ev, true); }
extern "C" void         LFAppTimerDrivenFrame()    { LFL::app->TimerDrivenFrame(true); }
extern "C" void         LFAppWakeup()              { return LFL::app->scheduler.Wakeup(LFL::app->focused); }
extern "C" void         LFAppResetGL()             { return LFL::app->ResetGL(); }
extern "C" void         LFAppAtExit()              { delete LFL::app; }
extern "C" void         LFAppShutdown()            { LFL::app->run=0; LFAppWakeup(); }
extern "C" void         BreakHook()                {}

extern "C" LFAppWindow *SetLFAppWindowByID(void *id) { return SetLFAppWindow(LFL::FindOrNull(LFL::app->windows, id)); }
extern "C" LFAppWindow *SetLFAppWindow(LFAppWindow *W) {
  CHECK(W);
  if (W == LFL::app->focused) return W;
  LFL::app->MakeCurrentWindow((LFL::app->focused = static_cast<LFL::Window*>(W)));
  return W;
}

extern "C" void SetLFAppMainThread() {
  LFL::Thread::id_t id = LFL::Thread::GetId();
  if (LFL::app->main_thread_id != id) INFOf("LFApp->main_thread_id changed from %llx to %llx", LFL::app->main_thread_id, id);
  LFL::app->main_thread_id = id; 
}

extern "C" unsigned LFAppNextRandSeed() {
  static LFL::mutex m;
  LFL::ScopedMutex sm(m);
  return (LFL::FLAGS_rand_seed = LFL::fnv32(&LFL::FLAGS_rand_seed, sizeof(unsigned)));
}

extern "C" void LFAppFatal() {
  ERROR("LFAppFatal");
  LFL::app->run = 0;
  if (bool suicide=true) *reinterpret_cast<volatile int*>(0) = 0;
  throw std::runtime_error("LFAppFatal");
}

#ifndef LFL_WINDOWS
extern "C" void HandleSigInt(int sig) { INFO("interrupt"); LFAppShutdown(); }
#else
extern "C" BOOL WINAPI HandleCtrlC(DWORD sig) { INFO("interrupt"); LFAppShutdown(); return TRUE; }
extern "C" void OpenSystemConsole(const char *title) {
  LFL::FLAGS_open_console=1;
  AllocConsole();
  SetConsoleTitle(title);
  freopen("CONOUT$", "wb", stdout);
  freopen("CONIN$", "rb", stdin);
  SetConsoleCtrlHandler(HandleCtrlC, 1);
}
extern "C" void CloseSystemConsole() {
  fclose(stdin);
  fclose(stdout);
  FreeConsole();
}
#endif

namespace LFL {
#ifdef LFL_DEBUG
const bool DEBUG = true;
#else
const bool DEBUG = false;
#endif
#ifdef LFL_MOBILE
const bool MOBILE = true;
#else
const bool MOBILE = false;
#endif
#ifdef LFL_IOS
const bool IOS = true;
#else
const bool IOS = false;
#endif
#ifdef LFL_ANDROID
const bool ANDROID = true;
#else
const bool ANDROID = false;
#endif

Application *app = nullptr;
const char *not_implemented = "not implemented";

DEFINE_int(loglevel, DEBUG ? 7 : 0, "Log level: [Fatal=-1, Error=0, Info=3, Debug=7]");
DEFINE_string(logfile, "", "Log file name");
DEFINE_bool(enable_audio, false, "Enable audio in/out");
DEFINE_bool(enable_video, false, "Enable OpenGL");
DEFINE_bool(enable_input, false, "Enable keyboard/mouse input");
DEFINE_bool(enable_network, false, "Enable asynchronous network engine");
DEFINE_bool(enable_camera, false, "Enable camera capture");
DEFINE_bool(enable_cuda, false, "Enable CUDA acceleration");
DEFINE_bool(daemonize, false, "Daemonize server");
DEFINE_bool(max_rlimit_core, true, "Max core dump rlimit");
DEFINE_bool(max_rlimit_open_files, false, "Max number of open files rlimit");
DEFINE_int(threadpool_size, 0, "Threadpool size");
DEFINE_int(target_fps, 0, "Max frames per second");
DEFINE_int(peak_fps, MOBILE ? 30 : 60, "Peak FPS");
DEFINE_unsigned(rand_seed, 0, "Random number generator seed");
DEFINE_bool(open_console, 0, "Open console on win32");
DEFINE_bool(cursor_grabbed, false, "Center cursor every frame");
DEFINE_bool(frame_debug, false, "Print each frame");
DEFINE_bool(rcon_debug, false, "Print game protocol commands");

void Log(int level, const char *file, int line, const string &m) {
  if (app) app->Log(level, file, line, m.c_str());
  else Application::WriteLogLine("", m.c_str(), file, line);
}

#ifdef LFL_APPLE
void NSLogString(const string&);
string GetNSDocumentDirectory();
#endif

#ifdef LFL_IOS
static pthread_key_t tls_key;
void ThreadLocalStorage::Init() { pthread_key_create(&tls_key, 0); ThreadInit(); }
void ThreadLocalStorage::Free() { ThreadFree(); pthread_key_delete(tls_key); }
void ThreadLocalStorage::ThreadInit() { pthread_setspecific(tls_key, new ThreadLocalStorage()); }
void ThreadLocalStorage::ThreadFree() { delete ThreadLocalStorage::Get(); }
ThreadLocalStorage *ThreadLocalStorage::Get() { return (ThreadLocalStorage*)pthread_getspecific(tls_key); }
#else
thread_local ThreadLocalStorage *tls_instance = 0;
void ThreadLocalStorage::Init() {}
void ThreadLocalStorage::Free() {}
void ThreadLocalStorage::ThreadInit() {}
void ThreadLocalStorage::ThreadFree() { delete tls_instance; tls_instance = nullptr; }
ThreadLocalStorage *ThreadLocalStorage::Get() { return tls_instance ? tls_instance : (tls_instance = new ThreadLocalStorage()); }
#endif
Allocator *ThreadLocalStorage::GetAllocator(bool reset_allocator) {
  ThreadLocalStorage *tls = Get();
  if (!tls->alloc) tls->alloc = make_unique<FixedAllocator<1024*1024>>();
  if (reset_allocator) tls->alloc->Reset();
  return tls->alloc.get();
}

/* FlagMap */

string Flag::GetString() const { string v=Get(); return StrCat(name, v.size()?" = ":"", v.size()?v:"", " : ", desc); } 
string FlagMap::Get   (const string &k) const { Flag *f = FindOrNull(flagmap, k); return f ? f->Get()    : "";    }
bool   FlagMap::IsBool(const string &k) const { Flag *f = FindOrNull(flagmap, k); return f ? f->IsBool() : false; }

bool FlagMap::Set(const string &k, const string &v) {
  Flag *f = FindOrNull(flagmap, k);
  if (!f) return false;
  f->override = true;
  INFO("set flag ", k, " = ", v);
  if (f->Get() != v) dirty = true;
  else return true;
  f->Update(v.c_str());
  return true;
}

string FlagMap::Match(const string &key, const char *source_filename) const {
  vector<int> keyv(key.size()), dbiv;
  Vec<int>::Assign(&keyv[0], key.data(), key.size());

  vector<string> db;
  for (auto &i : flagmap) {
    if (source_filename && strcmp(source_filename, i.second->file)) continue;
    db.push_back(i.first);
  }

  string mindistval;
  double dist, mindist = INFINITY;
  for (auto &t : db) {
    dbiv.resize(t.size());
    Vec<int>::Assign(&dbiv[0], t.data(), t.size());
    if ((dist = Levenshtein(keyv, dbiv)) < mindist) { mindist = dist; mindistval = t; }
  }
  return mindistval;
}

int FlagMap::getopt(int argc, const char* const* argv, const char *source_filename) {
  for (optind=1; optind<argc; /**/) {
    const char *arg = argv[optind], *key = arg + 1, *val = "";
    if (*arg != '-' || *(arg+1) == 0) break;

    if (++optind < argc && !(IsBool(key) && *(argv[optind]) == '-')) val = argv[optind++];
    if (!strcmp(key, "fullhelp")) { Print(); return -1; }

    if (!Set(key, val)) {
#ifdef LFL_APPLE
      if (PrefixMatch(key, "psn_")) continue;
#endif
      INFO("unknown flag: ", key);
      string nearest1 = Match(key), nearest2 = Match(key, source_filename);
      INFO("Did you mean -", nearest2.size() ? StrCat(nearest2, " or -") : "", nearest1, " ?");
      INFO("usage: ", argv[0], " -k v");
      Print(source_filename);
      return -1;
    }
  }
  return optind;
}

void FlagMap::Print(const char *source_filename) const {
  for (AllFlags::const_iterator i = flagmap.begin(); i != flagmap.end(); i++) {
    if (source_filename && strcmp(source_filename, i->second->file)) continue;
    INFO(i->second->GetString());
  }
  if (source_filename) INFO("fullhelp : Display full help"); 
}

/* Thread */

void Thread::Start() {
  ScopedMutex sm(start_mutex);
  impl = make_unique<std::thread>(bind(&Thread::ThreadProc, this));
  id = std::hash<std::thread::id>()(impl->get_id());
}

void Thread::ThreadProc() {
  { ScopedMutex sm(start_mutex); }
  INFOf("Started thread(%llx)", id);
  ThreadLocalStorage::ThreadInit();
  cb();
  ThreadLocalStorage::ThreadFree();
}

void ThreadPool::Write(Callback *cb) {
  worker[round_robin_next].queue->Write(cb);
  round_robin_next = (round_robin_next + 1) % worker.size();
}

Time Timer::GetTime(bool do_reset) {
  if (!do_reset) return GetTime();
  Time last_begin = Reset();
  return max(Time(0), begin - last_begin);
}

void RateLimiter::Limit() {
  Time since = timer.GetTime(true), targetframe(1000 / *target_hz);
  Time sleep = max(Time(0), targetframe - since - FMilliseconds(sleep_bias.Avg()));
  if (sleep != Time(0)) { MSleep(sleep.count()); sleep_bias.Add((timer.GetTime(true) - sleep).count()); }
}

/* Application */

Application::Application(int ac, const char* const* av) : argc(ac), argv(av) {
  run=1; initialized=0; main_thread_id=0; frames_ran=0; memzero(log_time); 
  fonts = make_unique<Fonts>();
}

void Application::Log(int level, const char *file, int line, const char *message) {
  {
    ScopedMutex sm(log_mutex);
    char tbuf[64];
    tm last_log_time = log_time;
    logtime(tbuf, sizeof(tbuf), &log_time);
    if (DayChanged(log_time, last_log_time)) 
      WriteLogLine(tbuf, StrCat("Date changed to ", logfileday(log_time)).c_str(), __FILE__, __LINE__);
    WriteLogLine(log_pid ? StrCat("[", pid, "] ", tbuf).c_str() : tbuf, message, file, line);
  }
  if (level == LFApp::Log::Fatal) LFAppFatal();
  if (run && FLAGS_enable_video && focused && focused->console) focused->console->Write(message);
}

void Application::WriteLogLine(const char *tbuf, const char *message, const char *file, int line) {
  if (app) {
    if (app->logout) {
      fprintf(app->logout, "%s %s (%s:%d)\r\n", tbuf, message, file, line);
      fflush(app->logout);
    }
    if (app->logfile) {
      fprintf(app->logfile, "%s %s (%s:%d)\r\n", tbuf, message, file, line);
      fflush(app->logfile);
    }
  }
#ifdef LFL_IOS
  NSLogString(StringPrintf("%s (%s:%d)", message, file, line));
#endif
#ifdef LFL_ANDROID
  __android_log_print(ANDROID_LOG_INFO, app ? app->name.c_str() : "lfl", "%s (%s:%d)", message, file, line);
#endif
}

void Application::WriteDebugLine(const char *message, const char *file, int line) {
  bool write_to_logfile = false;
  if (app && app->logerr) fprintf(app->logerr, "%s (%s:%d)\n", message, file, line);
#ifdef LFL_IOS
  write_to_logfile = true;
  NSLogString(StringPrintf("%s (%s:%d)", message, file, line));
#endif
#ifdef LFL_ANDROID
  __android_log_print(ANDROID_LOG_INFO, app ? app->name.c_str() : "", "%s (%s:%d)", message, file, line);
#endif
  if (write_to_logfile && app && app->logfile) {
    fprintf(app->logfile, "%s (%s:%d)\r\n", message, file, line);
    fflush(app->logfile);
  }
}

void Application::CreateNewWindow() {
  Window *orig_window = focused, *new_window = Window::Create();
  if (window_init_cb) window_init_cb(new_window);
  new_window->gd = CreateGraphicsDevice(new_window, opengles_version).release();
  CHECK(Video::CreateWindow(new_window));
  if (!new_window->started && (new_window->started = true)) {
    MakeCurrentWindow(new_window);
    StartNewWindow(new_window);
    MakeCurrentWindow(orig_window);
  }
}

void Application::StartNewWindow(Window *new_window) {
  if (new_window->gd) {
    if (!new_window->gd->done_init) new_window->gd->Init(new_window->Box());
    new_window->default_font.Load();
  }
  if (window_start_cb) window_start_cb(new_window);
  Video::StartWindow(new_window);
}

SocketServicesThread *Application::CreateNetworkThread(bool detach, bool start) {
  CHECK(net);
  if (detach) VectorEraseByValue(&modules, static_cast<Module*>(net.get()));
  network_thread = make_unique<SocketServicesThread>(net.get(), !detach);
  if (start) network_thread->thread->Start();
  return network_thread.get();
}

void *Application::GetSymbol(const string &n) {
#ifdef LFL_WINDOWS
  return GetProcAddress(GetModuleHandle(NULL), n.c_str());
#else
  return dlsym(RTLD_DEFAULT, n.c_str());
#endif
}

StringPiece Application::LoadResource(int id) {
#ifdef LFL_WINDOWS
  HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(id), MAKEINTRESOURCE(900));
  HGLOBAL resource_data = ::LoadResource(NULL, resource);
  return StringPiece((char*)LockResource(resource_data), SizeofResource(NULL, resource));
#else
  return StringPiece();
#endif
}

void Application::Daemonize(const char *dir, const char *progname) {
#ifndef LFL_WINDOWS
  char fn1[256], fn2[256];
  snprintf(fn1, sizeof(fn1), "%s%s.stdout", dir, progname);
  snprintf(fn2, sizeof(fn2), "%s%s.stderr", dir, progname);
  FILE *fout = fopen(fn1, "a"); fprintf(stderr, "open %s %s\n", fn1, fout ? "OK" : strerror(errno));
  FILE *ferr = fopen(fn2, "a"); fprintf(stderr, "open %s %s\n", fn2, ferr ? "OK" : strerror(errno));
  Daemonize(fout, ferr);
#endif
}

void Application::Daemonize(FILE *fout, FILE *ferr) {
#ifndef LFL_WINDOWS
  int pid = fork();
  if (pid < 0) { fprintf(stderr, "fork: %d\n", pid); exit(-1); }
  if (pid > 0) { fprintf(stderr, "daemonized pid: %d\n", pid); exit(0); }

  int sid = setsid();
  if (sid < 0) { fprintf(stderr, "setsid: %d\n", sid); exit(-1); }

  close(STDIN_FILENO); 
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  if (fout) dup2(fileno(fout), 1);
  if (ferr) dup2(fileno(ferr), 2);
#endif
}

int Application::Create(const char *source_filename) {
#ifdef LFL_GLOG
  google::InstallFailureSignalHandler();
#endif
  SetLFAppMainThread();
  time_started = Now();
  progname = argv[0];
  startdir = LocalFile::CurrentDirectory();

#ifdef LFL_WINDOWS
  bindir = progname.substr(0, DirNameLen(progname, true));

  { /* winsock startup */
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2), &wsadata);
  }

  string console_title = StrCat(name, " console");
  if (argc > 1) OpenSystemConsole(console_title.c_str());

#else
  pid = getpid();
  bindir = LocalFile::JoinPath(startdir, progname.substr(0, DirNameLen(progname, true)));

  /* handle SIGINT */
  signal(SIGINT, HandleSigInt);

  { /* ignore SIGPIPE */
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;

    if (sigaction(SIGPIPE, &sa, NULL) == -1) return -1;
  }
#endif

  {
#if defined(LFL_ANDROID)
#elif defined(LFL_APPLE)
    char rpath[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef respath = CFBundleCopyResourcesDirectoryURL(mainBundle);
    CFURLGetFileSystemRepresentation(respath, true, MakeUnsigned(rpath), sizeof(rpath));
    CFRelease(respath);
    if (PrefixMatch(rpath, startdir+"/")) assetdir = StrCat(rpath + startdir.size()+1, "/assets/");
    else assetdir = StrCat(rpath, "/assets/"); 
#else
    assetdir = StrCat(bindir, "assets/"); 
#endif
  }

  if (Singleton<FlagMap>::Get()->getopt(argc, argv, source_filename) < 0) return -1;
  if (!FLAGS_rand_seed) FLAGS_rand_seed = fnv32(&pid, sizeof(int), time(0));
  unsigned init_rand_seed = FLAGS_rand_seed;
  srand(init_rand_seed);

  ThreadLocalStorage::Init();
  Singleton<NullAllocator>::Get();
  Singleton<MallocAllocator>::Get();

#ifdef LFL_WINDOWS
  if (argc > 1) {
    if (!FLAGS_open_console) CloseSystemConsole();
  }
  else if (FLAGS_open_console) OpenSystemConsole(console_title.c_str());
  if (argc > 1) OpenSystemConsole(console_title.c_str());
#endif

  {
#if defined(LFL_ANDROID)
    JNI *jni = LFL::Singleton<LFL::JNI>::Get();
    jmethodID mid = CheckNotNull(jni->env->GetMethodID(jni->activity_class, "getFilesDirCanonicalPath", "()Ljava/lang/String;"));
    jstring path = jstring(jni->env->CallObjectMethod(jni->activity, mid));
    savedir = StrCat(jni->GetJString(path), "/");
    jni->env->DeleteLocalRef(path);
#elif defined(LFL_APPLE) && !defined(LFL_IOS_SIM)
    savedir = StrCat(GetNSDocumentDirectory(), "/");
#elif defined(LFL_WINDOWS)
    char path[MAX_PATH] = { 0 };
    if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, path))) return -1;
    savedir = StrCat(path, "/");
#endif
  }

#ifdef LFL_DEBUG
  if (FLAGS_logfile.empty() && !FLAGS_logfile_.override) FLAGS_logfile = "\x01";
#endif
  if (!FLAGS_logfile.empty()) {
    if (FLAGS_logfile == "\x01") FLAGS_logfile = StrCat(savedir, name, ".txt");
    logfile = fopen(FLAGS_logfile.c_str(), "a");
    if (logfile) SystemNetwork::SetSocketCloseOnExec(fileno(logfile), 1);
    INFO("logfile = ", FLAGS_logfile, " (opened=", logfile != nullptr, ")");
  }

  INFO("startdir = ", startdir);
  INFO("assetdir = ", assetdir);
  INFO("savedir = ", savedir);
  INFO("rand_seed = ", init_rand_seed);

#ifndef LFL_WINDOWS
  if (FLAGS_max_rlimit_core) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) return ERRORv(-1, "core getrlimit ", strerror(errno));

    rl.rlim_cur = rl.rlim_max;
    if (setrlimit(RLIMIT_CORE, &rl) == -1) return ERRORv(-1, "core setrlimit ", strerror(errno));
  }

#ifndef LFL_MOBILE
  if (FLAGS_max_rlimit_open_files) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) return ERRORv(-1, "files getrlimit ", strerror(errno));
#ifdef LFL_APPLE
    rl.rlim_cur = rl.rlim_max = OPEN_MAX;
#else
    rl.rlim_cur = rl.rlim_max; // 999999
#endif
    INFO("setrlimit(RLIMIT_NOFILE, ", rl.rlim_cur, ")");
    if (setrlimit(RLIMIT_NOFILE, &rl) == -1) return ERRORv(-1, "files setrlimit ", strerror(errno));
  }
#endif // LFL_MOBILE
#endif // LFL_WINDOWS

  if (FLAGS_daemonize) {
    Daemonize("", progname.c_str());
    SetLFAppMainThread();
  }

  if (FLAGS_enable_video && FLAGS_font.empty())
    fonts->DefaultFontEngine()->SetDefault();

  return 0;
}

int Application::Init() {
  if (LoadModule((framework = unique_ptr<Module>(CreateFrameworkModule())).get()))
    return ERRORv(-1, "platform init failed");

  thread_pool.Open(X_or_1(FLAGS_threadpool_size));
  if (FLAGS_threadpool_size) thread_pool.Start();

  if (focused) {
    if (FLAGS_enable_video) {
      if (!focused->gd) focused->gd = CreateGraphicsDevice(focused, opengles_version).release();
      shaders = make_unique<Shaders>();
      focused->gd->Init(focused->Box());
#ifdef LFL_WINDOWS
      if (splash_color) DrawSplash(*splash_color);
#endif
    } else { windows[focused->id] = focused; }
  }

  if (FLAGS_enable_audio) {
    if (LoadModule((audio = make_unique<Audio>()).get())) return ERRORv(-1, "audio init failed");
  }
  else { FLAGS_chans_in=FLAGS_chans_out=1; }

  if (FLAGS_enable_audio || FLAGS_enable_video) {
    if ((asset_loader = make_unique<AssetLoader>())->Init()) return ERRORv(-1, "asset loader init failed");
  }

  if (focused) {
    if (FLAGS_enable_video) fonts->LoadDefaultFonts();
    focused->default_font = FontRef(FontDesc::Default(), false);
  }

  if (FLAGS_enable_input) {
    if (LoadModule((input = make_unique<Input>()).get())) return ERRORv(-1, "input init failed");
  }

  if (FLAGS_enable_network) {
    if (LoadModule((net = make_unique<SocketServices>()).get())) return ERRORv(-1, "network init failed");
  }

  if (FLAGS_enable_camera) {
    if (LoadModule((camera = make_unique<Camera>()).get())) return ERRORv(-1, "camera init failed");
  }

  if (FLAGS_enable_cuda) {
    (cuda = make_unique<CUDA>())->Init();
  }

  scheduler.Init();
  if (scheduler.monolithic_frame) frame_time.GetTime(true);
  else if (focused)      focused->frame_time.GetTime(true);
  INFO("Application::Init() succeeded");
  initialized = true;
  return 0;
}

int Application::Start() {
  if (FLAGS_enable_audio && audio->Start()) return ERRORv(-1, "lfapp audio start failed");
  return 0;
}

int Application::HandleEvents(unsigned clicks) {
  int events = 0, module_events;
  for (auto i = modules.begin(); i != modules.end() && run; ++i)
    if ((module_events = (*i)->Frame(clicks)) > 0) events += module_events;

  // handle messages sent to main thread
  if (run) events += message_queue.HandleMessages();

  // fake threadpool that executes in main thread
  if (run && !FLAGS_threadpool_size) events += thread_pool.worker[0].queue->HandleMessages();

  return events;
}

int Application::EventDrivenFrame(bool handle_events, bool draw_frame) {
  if (!MainThread()) ERROR("Frame() called from thread ", Thread::GetId());
  unsigned clicks = focused->frame_time.GetTime(true).count();
  if (handle_events) HandleEvents(clicks);

  if (!draw_frame) return clicks;
  int ret = focused->Frame(clicks, 0);
  if (FLAGS_frame_debug) INFO("frame_debug Application::Frame Window ", focused->id, " = ", ret);

  frames_ran++;
  return clicks;
}

int Application::TimerDrivenFrame(bool got_wakeup) {
  if (!MainThread()) ERROR("MonolithicFrame() called from thread ", Thread::GetId());
  unsigned clicks = frame_time.GetTime(true).count();
  int events = HandleEvents(clicks) + got_wakeup;
  if (frame_disabled) return clicks;

  for (auto i = windows.begin(); run && i != windows.end(); ++i) {
    auto w = i->second;
#ifdef LFL_ANDROID
    if (w->minimized || (!w->target_fps && !events)) continue;
#else
    if (w->minimized || !w->target_fps) continue;
#endif
    int ret = w->Frame(clicks, 0);
    if (FLAGS_frame_debug) INFO("frame_debug Application::Frame Window ", w->id, " = ", ret);
  }

  frames_ran++;
  return clicks;
}

int Application::Main() {
  ONCE({ scheduler.Start(); });
  if (Start()) return -1;
  if (!scheduler.run_main_loop) return 0;
  return MainLoop();
}

int Application::MainLoop() {
  INFO("MainLoop: Begin, run=", run);
  while (run) {
    bool got_wakeup = scheduler.MainWait();
    TimerDrivenFrame(got_wakeup);
#ifdef LFL_ANDROID
    if (focused->minimized) { INFO("MainLoop: minimized"); return 0; }
#endif
    if (scheduler.rate_limit && app->run && FLAGS_target_fps) scheduler.maxfps.Limit();
    MSleep(1);
  }
  INFO("MainLoop: End, run=", run);
  return 0;
}

void Application::DrawSplash(const Color &c) {
  focused->gd->ClearColor(c);
  focused->gd->Clear();
  focused->gd->Flush();
  Video::Swap();
  focused->gd->ClearColor(focused->gd->clear_color);
}

void Application::ResetGL() {
  INFO("Application::ResetGL");
  fonts->ResetGL();
  for (auto &a : asset.vec) a.ResetGL();
  for (auto &w : windows) w.second->ResetGL();
}

Application::~Application() {
  run = 0;
  INFO("exiting with ", windows.size(), " open windows");
  vector<Window*> close_list;
  for (auto &i : windows) close_list.push_back(i.second);
  for (auto &i : close_list) CloseWindow(i);
  if (network_thread) {
    network_thread->Write(new Callback([](){}));
    network_thread->thread->Wait();
    network_thread->net->Free();
  }
  if (fonts) fonts.reset();
  if (shaders) shaders.reset();
  if (!FLAGS_threadpool_size && thread_pool.worker.size()) thread_pool.worker[0].queue->HandleMessages();
  else thread_pool.Stop();
  message_queue.HandleMessages();
  if (exit_cb) exit_cb();
  if (cuda) cuda->Free();
  for (auto &m : modules) m->Free();
  scheduler.Free();
  if (logfile) fclose(logfile);
#ifdef LFL_WINDOWS
  if (FLAGS_open_console) PromptFGets("Press [enter] to continue...");
#endif
}

/* Window */

Window::Window() : caption(app->name), fps(128), tex_mode(2, 1, 0), grab_mode(2, 0, 1),
  fill_mode(3, GraphicsDevice::Fill, GraphicsDevice::Line, GraphicsDevice::Point) {
  id = 0;
  started = minimized = cursor_grabbed = frame_init = animating = 0;
  resize_increment_x = resize_increment_y = 0;
  target_fps = FLAGS_target_fps;
  multitouch_keyboard_x = .93; 
  SetBox(LFL::Box(0, 0, 640, 480));
}

Window::~Window() {
  ClearChildren();
  if (console) console->WriteHistory(app->savedir, StrCat(app->name, "_console"), "");
  console.reset();
  delete gd;
}

void Window::ClearChildren() {
  dialogs.clear();
  my_gui.clear();
  my_input.clear();
}

Box Window::Box(float xp, float yp, float xs, float ys, float xbl, float ybt, float xbr, float ybb) const {
  if (isinf(xbr)) xbr = xbl;
  if (isinf(ybb)) ybb = ybt;
  return LFL::Box(x + width  * (xp + xbl),
                  y + height * (yp + ybb),
                  width  * xs - width  * (xbl + xbr),
                  height * ys - height * (ybt + ybb), false);
}

void Window::SetBox(const LFL::Box &b) {
  Assign(&x, &y, b.x, b.y);
  Assign(&width, &height, b.w, b.h);
}

void Window::InitConsole(const Callback &animating_cb) {
  gui.push_back((console = make_unique<Console>(this, animating_cb)).get());
  console->ReadHistory(app->savedir, StrCat(app->name, "_console"));
  console->Write(StrCat(caption, " started"));
  console->Write("Try console commands 'cmds' and 'flags'");
}

size_t Window::NewGUI() { my_gui.emplace_back(unique_ptr<GUI>()); return my_gui.size()-1; }
void Window::DelGUI(GUI *g) { RemoveGUI(g); VectorRemoveUnique(&my_gui, g); }

void Window::OnDialogAdded(Dialog *d) {
  // if (dialogs.size() == 1)
    BringDialogToFront(d);
}

void Window::BringDialogToFront(Dialog *d) {
  if (top_dialog == d) return;
  if (top_dialog) top_dialog->LoseFocus();
  int zsort_ind = 0;
  for (auto &d : dialogs) d->zsort = ++zsort_ind;
  d->zsort = 0;
  sort(dialogs.begin(), dialogs.end(), Dialog::LessThan);
  (top_dialog = d)->TakeFocus();
}

void Window::GiveDialogFocusAway(Dialog *d) {
  if (top_dialog == d) { top_dialog=0; d->LoseFocus(); }
}

void Window::DrawDialogs() {
  for (auto i = dialogs.begin(), e = dialogs.end(); i != e; ++i) (*i)->Draw();
  if (console) console->Draw();
  if (FLAGS_draw_grid) {
    Color c(.7, .7, .7);
    glIntersect(gd, mouse.x, mouse.y, &c);
    default_font->Draw(StrCat("draw_grid ", mouse.x, " , ", mouse.y), point(0,0));
  }
}

void Window::Reshaped(const LFL::Box &b) {
  INFO("Window::Reshaped(", b.DebugString(), ")");
  SetBox(b);
  if (!gd) return;
  gd->ViewPort(LFL::Box(b.right(), b.top()));
  gd->DrawMode(gd->default_draw_mode);
  for (auto g = gui.begin(); g != gui.end(); ++g) (*g)->Layout();
  if (reshaped_cb) reshaped_cb();
}

void Window::ResetGL() {
  INFO("Window::ResetGL");
  for (auto b : gd->buffers) *b = -1;
  gd->Init(Box());
  for (auto &g : gui    ) g->ResetGL();
  for (auto &g : dialogs) g->ResetGL();
}

void Window::SwapAxis() {
  FLAGS_rotate_view = FLAGS_rotate_view ? 0 : -90;
  FLAGS_swap_axis = FLAGS_rotate_view != 0;
  Reshaped(LFL::Box(y, x, height, width));
}

int Window::Frame(unsigned clicks, int flag) {
  if (app->focused != this) app->MakeCurrentWindow(this);

  if (FLAGS_enable_video) {
    if (!frame_init && (frame_init = true))  {
#ifdef LFL_IOS
      gd->GetIntegerv(GraphicsDevice::FramebufferBinding, &gd->default_framebuffer);
      INFO("default_framebuffer = ", gd->default_framebuffer);
#endif
    }
    gd->DrawMode(gd->default_draw_mode);
    gd->Clear();
    gd->LoadIdentity();
  }

  /* frame */
  int ret = frame_cb ? frame_cb(this, clicks, flag) : 0;

  /* allow app to skip frame */
  if (ret < 0) return ret;
  fps.Add(clicks);

  if (FLAGS_enable_video) {
    Video::Swap();
  }
  return ret;
}

void Window::RenderToFrameBuffer(FrameBuffer *fb) {
  int dm = gd->draw_mode;
  fb->Attach();
  // gd->ViewPort(Box(fb->tex.width, fb->tex.height));
  gd->DrawMode(gd->default_draw_mode);
  gd->Clear();
  frame_cb(this, 0, 0);
  fb->Release();
  gd->RestoreViewport(dm);
}

/* FrameScheduler */

void FrameScheduler::Init() { 
  if (app->focused) app->focused->target_fps = FLAGS_target_fps;
  wait_forever = !FLAGS_target_fps;
  maxfps.timer.GetTime(true);
  if (wait_forever && synchronize_waits) frame_mutex.lock();
}

void FrameScheduler::Free() { 
  if (wait_forever && synchronize_waits) frame_mutex.unlock();
  if (wait_forever && wait_forever_thread) wakeup_thread.Wait();
}

void FrameScheduler::Start() {
  if (!wait_forever) return;
  if (wait_forever_thread) wakeup_thread.Start();
}

bool FrameScheduler::MainWait() {
  bool ret = false;
  if (wait_forever && !FLAGS_target_fps) {
    if (synchronize_waits) {
      wait_mutex.lock();
      frame_mutex.unlock();
    }
    ret = DoMainWait();
    if (synchronize_waits) {
      frame_mutex.lock();
      wait_mutex.unlock();
    }
  }
  return ret;
}

void FrameScheduler::UpdateTargetFPS(Window *w, int fps) {
  w->target_fps = fps;
  if (monolithic_frame) {
    int next_target_fps = 0;
    for (const auto &wi : app->windows) Max(&next_target_fps, wi.second->target_fps);
    FLAGS_target_fps = next_target_fps;
  }
  CHECK(w->id);
  UpdateWindowTargetFPS(w);
}

void FrameScheduler::SetAnimating(Window *w, bool is_animating) {
  w->animating = is_animating;
  int target_fps = is_animating ? FLAGS_peak_fps : 0;
  if (target_fps != w->target_fps) {
    UpdateTargetFPS(w, target_fps);
    Wakeup(w);
  }
}

}; // namespace LFL
