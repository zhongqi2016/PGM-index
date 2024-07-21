#pragma once

#include <vector>
#define CACHE_LINE_SIZE 64
using size_t = decltype(sizeof(0));

template<typename T>
class EtzingerList {
private:
    std::vector<T> _etzinger_list;
    std::vector<size_t> _index_list;
    size_t _size;
    const int _block_size;
    T generate_eytzinger(const std::vector<T> &original_list, size_t i = 0, size_t k = 1) {
        if (k < _size) {
            i = generate_eytzinger(original_list, i, 2 * k);
            _index_list[k] = i;
            _etzinger_list[k] = original_list[i++];
            i = generate_eytzinger(original_list, i, 2 * k + 1);
        }
        return i;
    }

public:
    explicit EtzingerList(const std::vector<T> &original_list)
        : _etzinger_list(std::vector<T>(original_list.size() + 1)),
          _index_list(std::vector<size_t>(original_list.size() + 1)),
          _size(original_list.size()),
          _block_size(CACHE_LINE_SIZE / sizeof(T)) {
        generate_eytzinger(original_list);
    }

    size_t search(T x) {
        int k = 1;
        while (k <= _size) {
            __builtin_prefetch(_etzinger_list.data() + k * _block_size);
            k = 2 * k + (_etzinger_list[k] < x);
        }
        k >>= __builtin_ffs(~k);
        return _index_list[k];
    }

    [[nodiscard]] size_t size() const {
        return _size;
    }

    std::vector<T> &get_etzinger_list() const {
        return _etzinger_list;
    }
};