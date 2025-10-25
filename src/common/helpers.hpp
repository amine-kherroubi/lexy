#pragma once

#include "types.hpp"
#include <sys/stat.h>

template <typename MapType>
Vector<typename MapType::key_type> get_keys(const MapType &mapping) {
  Vector<typename MapType::key_type> keys;
  keys.reserve(mapping.size());
  for (const auto &entry : mapping)
    keys.push_back(entry.first);
  return keys;
}

// Helper function to create directory if it doesn't exist
inline void createDirectory(const String &path) {
#ifdef _WIN32
  _mkdir(path.c_str());
#else
  mkdir(path.c_str(), 0755);
#endif
}

// Helper to check .lexy extension
inline bool hasLexyExtension(const String &filename) {
  const String ext = ".lexy";
  if (filename.size() < ext.size())
    return false;
  return filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
}

// Extract base filename from path (strips mandatory .lexy extension)
inline String getBaseName(const String &path) {
  size_t slash = path.find_last_of("/\\");
  size_t start = (slash == String::npos) ? 0 : slash + 1;

  String filename = path.substr(start);
  size_t lexy_pos = filename.find(".lexy");

  return filename.substr(0, lexy_pos); // .lexy is mandatory, just strip it
}