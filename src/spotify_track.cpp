#include "spotify_track.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <string>

const char kScript[] = R"OSA(tell application "Spotify"
	if not running then
		return "Spotify не запущен"
	end if
	try
		set trackName to name of current track
		set trackArtist to artist of current track
		return trackName & " - " & trackArtist
	on error
		return "Нет активного трека (или ошибка AppleScript)"
	end try
end tell
)OSA";

bool write_all(int fd, const char *data, size_t len) {
  while (len > 0) {
    ssize_t n = ::write(fd, data, len);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    data += static_cast<size_t>(n);
    len -= static_cast<size_t>(n);
  }
  return true;
}

void trim_trailing_newlines(std::string &s) {
  while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) {
    s.pop_back();
  }
}

std::string get_spotify_track() {
  char tmpl[] = "/tmp/spotify_now_XXXXXX";
  int fd = ::mkstemp(tmpl);
  if (fd < 0) {
    return "ошибка получения трека";
  }

  const size_t script_len = sizeof(kScript) - 1;
  if (!write_all(fd, kScript, script_len)) {
    ::close(fd);
    ::unlink(tmpl);
    return "ошибка получения трека";
    ;
  }
  ::close(fd);

  std::string cmd = std::string("osascript ") + tmpl + " 2>&1";
  FILE *pipe = ::popen(cmd.c_str(), "r");

  if (!pipe) {
    ::unlink(tmpl);
    return "ошибка получения трека";
    ;
  }

  std::string out = "";
  char buf[4096];
  while (std::fgets(buf, sizeof(buf), pipe) != nullptr) {
    out += buf;
  }

  const int pclose_status = ::pclose(pipe);
  ::unlink(tmpl);
  trim_trailing_newlines(out);

  return out;
}
