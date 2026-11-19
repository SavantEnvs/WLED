// Host in-memory filesystem with the Arduino FS/File API (LittleFS stand-in). Files live in a
// std::map that harnesses reset per input.
#pragma once
#include <Arduino.h>
#include <map>
#include <memory>
#include <vector>

namespace fs {
enum SeekMode { SeekSet = 0, SeekCur = 1, SeekEnd = 2 };
struct FSInfo { size_t totalBytes, usedBytes, blockSize, pageSize, maxOpenFiles, maxPathLength; };
typedef std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> MemFiles;

class File : public Stream {
 public:
  std::shared_ptr<std::vector<uint8_t>> data;
  std::string path;
  size_t pos = 0;
  bool writable = false, dir = false;
  std::vector<std::string> entries; size_t entryIdx = 0;
  MemFiles *owner = nullptr;
  File() {}
  operator bool() const { return data != nullptr || dir; }
  size_t write(uint8_t c) override { return write(&c, 1); }
  size_t write(const uint8_t *b, size_t n) override {
    if (!data || !writable) return 0;
    if (pos + n > data->size()) data->resize(pos + n);
    memcpy(data->data() + pos, b, n); pos += n; return n;
  }
  using Print::write;
  int available() override { return data ? (int)(data->size() - std::min(pos, data->size())) : 0; }
  int read() override { return (data && pos < data->size()) ? (*data)[pos++] : -1; }
  int peek() override { return (data && pos < data->size()) ? (*data)[pos] : -1; }
  size_t read(uint8_t *b, size_t n) { if (!data || pos >= data->size()) return 0; size_t k = std::min(n, data->size() - pos); memcpy(b, data->data() + pos, k); pos += k; return k; }
  size_t readBytes(char *b, size_t n) override { return read((uint8_t *)b, n); }
  using Stream::readBytes;
  bool seek(uint32_t p, SeekMode m = SeekSet) {
    if (!data) return false;
    long np = m == SeekSet ? (long)p : m == SeekCur ? (long)pos + (long)p : (long)data->size() + (long)p;
    if (np < 0 || (size_t)np > data->size()) return false;
    pos = (size_t)np; return true;
  }
  size_t position() const { return pos; }
  size_t size() const { return data ? data->size() : 0; }
  void close() { data.reset(); dir = false; }
  void flush() override {}
  bool find(const char *t) {
    if (!data || !t) return false;
    size_t n = strlen(t); if (!n) return true;
    while (pos < data->size()) {
      if (data->size() - pos >= n && memcmp(data->data() + pos, t, n) == 0) { pos += n; return true; }
      pos++;
    }
    return false;
  }
  bool find(char c) { char t[2] = {c, 0}; return find(t); }
  const char *name() const { size_t s = path.rfind('/'); return path.c_str() + (s == std::string::npos ? 0 : s + 1); }
  const char *path_c() const { return path.c_str(); }
  const char *fullName() const { return path.c_str(); }
  bool isDirectory() const { return dir; }
  bool isFile() const { return data != nullptr; }
  time_t getLastWrite() { return 0; }
  File openNextFile(const char * = "r");
  void rewindDirectory() { entryIdx = 0; }
};

class Dir {
 public:
  std::vector<std::string> entries; int idx = -1; MemFiles *owner = nullptr;
  bool next() { return ++idx < (int)entries.size(); }
  String fileName() { return idx >= 0 && idx < (int)entries.size() ? String(entries[idx].c_str()) : String(); }
  size_t fileSize() { if (idx < 0 || idx >= (int)entries.size()) return 0; auto it = owner->find(entries[idx]); return it == owner->end() ? 0 : it->second->size(); }
  bool isFile() { return true; }
  bool isDirectory() { return false; }
  File openFile(const char *mode);
};

class FS {
 public:
  MemFiles files;
  bool begin() { return true; }
  void end() {}
  bool format() { files.clear(); return true; }
  void clear() { files.clear(); }
  static std::string norm(const char *p) { std::string s = p ? p : ""; if (s.empty() || s[0] != '/') s = "/" + s; return s; }
  void put(const char *path, const uint8_t *d, size_t n) { files[norm(path)] = std::make_shared<std::vector<uint8_t>>(d, d + n); }
  File open(const char *path, const char *mode = "r") {
    File f; std::string p = norm(path); f.path = p; f.owner = &files;
    bool w = mode && (mode[0] == 'w' || mode[0] == 'a' || strchr(mode, '+'));
    if (p == "/") {
      f.dir = true; for (auto &kv : files) f.entries.push_back(kv.first); return f;
    }
    auto it = files.find(p);
    if (it == files.end()) {
      if (!w || mode[0] == 'r') return File();
      it = files.emplace(p, std::make_shared<std::vector<uint8_t>>()).first;
    }
    f.data = it->second; f.writable = w;
    if (mode && mode[0] == 'w') f.data->clear();
    if (mode && mode[0] == 'a') f.pos = f.data->size();
    return f;
  }
  File open(const String &p, const char *m = "r") { return open(p.c_str(), m); }
  bool exists(const char *path) { std::string p = norm(path); return p == "/" || files.count(p); }
  bool exists(const String &p) { return exists(p.c_str()); }
  bool remove(const char *path) { return files.erase(norm(path)) > 0; }
  bool remove(const String &p) { return remove(p.c_str()); }
  bool rename(const char *a, const char *b) { auto it = files.find(norm(a)); if (it == files.end()) return false; auto d = it->second; files.erase(it); files[norm(b)] = d; return true; }
  bool rename(const String &a, const String &b) { return rename(a.c_str(), b.c_str()); }
  bool mkdir(const char *) { return true; }
  bool rmdir(const char *) { return true; }
  bool info(FSInfo &i) { memset(&i, 0, sizeof(i)); i.totalBytes = 1 << 20; i.usedBytes = usedBytes(); i.blockSize = 4096; i.pageSize = 256; i.maxPathLength = 32; return true; }
  size_t totalBytes() { return 1 << 20; }
  size_t usedBytes() { size_t u = 0; for (auto &kv : files) u += kv.second->size(); return u; }
  Dir openDir(const char *) { Dir d; d.owner = &files; for (auto &kv : files) d.entries.push_back(kv.first); return d; }
  Dir openDir(const String &p) { return openDir(p.c_str()); }
};

inline File File::openNextFile(const char *) {
  if (!dir || !owner) return File();
  while (entryIdx < entries.size()) {
    auto it = owner->find(entries[entryIdx++]);
    if (it == owner->end()) continue;
    File f; f.path = it->first; f.data = it->second; f.owner = owner; return f;
  }
  return File();
}
inline File Dir::openFile(const char *) {
  File f; if (idx < 0 || idx >= (int)entries.size()) return f;
  auto it = owner->find(entries[idx]); if (it == owner->end()) return f;
  f.path = it->first; f.data = it->second; f.owner = owner; return f;
}
}  // namespace fs
using fs::File;
using fs::FS;
using fs::Dir;
using fs::FSInfo;
using fs::SeekSet;
using fs::SeekCur;
using fs::SeekEnd;
using fs::SeekMode;
