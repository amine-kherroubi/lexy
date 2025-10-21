#pragma once

#include "types.hpp"

template <typename MapType>
Vector<typename MapType::key_type> get_keys(const MapType &mapping) {
  Vector<typename MapType::key_type> keys;
  keys.reserve(mapping.size());
  for (const auto &entry : mapping)
    keys.push_back(entry.first);
  return keys;
}