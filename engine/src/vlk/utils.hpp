#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstring>
#include <type_traits>

class Utils {
public:
  template <typename T> inline static void zeroInitializeStruct(T &a) {
    static_assert(std::is_class<T>::value, "T must be a struct! (or class)");
    memset(&a, 0, sizeof(a));
  }
};

#endif
