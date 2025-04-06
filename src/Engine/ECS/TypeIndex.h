//
// Created by SIMEON on 4/3/2025.
//

#ifndef TYPEINDEX_H
#define TYPEINDEX_H

#include <typeindex>
#include <unordered_map>

class TypeIndex {
public:
    template <typename T>
    static size_t get() {
         static size_t index = nextIndex++;
         return index;
    }
private:
    static std::atomic<size_t> nextIndex;
};

#endif //TYPEINDEX_H