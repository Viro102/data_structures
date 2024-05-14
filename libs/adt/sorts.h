#pragma once

#include "../amt/implicit_sequence.h"
#include "array.h"
#include "queue.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <type_traits>

namespace ds::adt {
template <typename T> struct Sort {
  public:
    virtual void sort(amt::ImplicitSequence<T> &is,
                      std::function<bool(const T &, const T &)> compare) = 0;

    void sort(amt::ImplicitSequence<T> &is) {
        sort(is, [](const T &a, const T &b) -> bool { return a < b; });
    }
};

//----------

template <typename T> class SelectSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;
};

//----------

template <typename T> class InsertSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;
};

//----------

template <typename T> class BubbleSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;
};

//----------

template <typename T> class QuickSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;

  private:
    void quick(amt::ImplicitSequence<T> &is, std::function<bool(const T &, const T &)> compare,
               size_t min, size_t max);
};

//----------

template <typename T> class HeapSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;
};

//----------

template <typename T> class ShellSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;

  private:
    void shell(amt::ImplicitSequence<T> &is, std::function<bool(const T &, const T &)> compare,
               size_t k);
};

//----------

template <typename Key, typename T> class RadixSort : public Sort<T> {
    static_assert(std::is_integral_v<Key>, "Radix sort supports only integral types.");

  public:
    RadixSort();

    explicit RadixSort(std::function<Key(const T &)> getKey);

    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;

  private:
    std::function<Key(const T &)> getKey_;
};

//----------

template <typename T> class MergeSort : public Sort<T> {
  public:
    void sort(amt::ImplicitSequence<T> &is,
              std::function<bool(const T &, const T &)> compare) override;

  private:
    void split(size_t n);

    void merge(std::function<bool(const T &, const T &)> compare, size_t n);

    ImplicitQueue<T> *queue1_{nullptr};
    ImplicitQueue<T> *queue2_{nullptr};
    ImplicitQueue<T> *mergeQueue_{nullptr};
};

//----------

template <typename T>
void SelectSort<T>::sort(amt::ImplicitSequence<T> &is,
                         std::function<bool(const T &, const T &)> compare) {
    for (size_t i = 0; i < is.size(); ++i) {
        size_t minIndex = i;
        for (size_t j = i + 1; j < is.size(); ++j) {
            if (compare(is.access(j)->data_, is.access(minIndex)->data_)) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            std::swap(is.access(i)->data_, is.access(minIndex)->data_);
        }
    }
}

template <typename T>
void InsertSort<T>::sort(amt::ImplicitSequence<T> &is,
                         std::function<bool(const T &, const T &)> compare) {
    for (size_t i = 1; i < is.size(); ++i) {
        T key = is.access(i)->data_;
        size_t j = i - 1;

        while (j != static_cast<size_t>(-1) && compare(key, is.access(j)->data_)) {
            is.access(j + 1)->data_ = is.access(j)->data_;
            j--;
        }
        is.access(j + 1)->data_ = key;
    }
}

template <typename T>
void BubbleSort<T>::sort(amt::ImplicitSequence<T> &is,
                         std::function<bool(const T &, const T &)> compare) {
    bool swapped;
    do {
        swapped = false;
        for (size_t i = 0; i < is.size() - 1; ++i) {
            if (compare(is.access(i + 1)->data_, is.access(i)->data_)) {
                using std::swap;
                swap(is.access(i + 1)->data_, is.access(i)->data_);
                swapped = true;
            }
        }
    } while (swapped);
}

template <typename T>
void QuickSort<T>::sort(amt::ImplicitSequence<T> &is,
                        std::function<bool(const T &, const T &)> compare) {
    if (!is.isEmpty()) {
        quick(is, compare, 0, is.size() - 1);
    }
}

template <typename T>
void QuickSort<T>::quick(amt::ImplicitSequence<T> &is,
                         std::function<bool(const T &, const T &)> compare, size_t min,
                         size_t max) {
    T pivot = is.access(std::midpoint(min, max))->data_;
    size_t left = min;
    size_t right = max;

    while (left <= right) {
        while (compare(is.access(left)->data_, pivot)) {
            ++left;
        }
        while (right > 0 && compare(pivot, is.access(right)->data_)) {
            --right;
        }

        if (left <= right) {
            std::swap(is.access(left)->data_, is.access(right)->data_);
            ++left;
            if (right > 0) {
                --right;
            }
        }
    }
    if (min < right) {
        quick(is, compare, min, right);
    }
    if (left < max) {
        quick(is, compare, left, max);
    }
}

template <typename T>
void HeapSort<T>::sort(amt::ImplicitSequence<T> &is,
                       std::function<bool(const T &, const T &)> compare) {
    //  TODO: implement
}

template <typename T>
void ShellSort<T>::sort(amt::ImplicitSequence<T> &is,
                        std::function<bool(const T &, const T &)> compare) {
    shell(is, compare, static_cast<size_t>(std::ceil(std::log10(is.size()))));
}

template <typename T>
void ShellSort<T>::shell(amt::ImplicitSequence<T> &is,
                         std::function<bool(const T &, const T &)> compare, size_t k) {
    for (size_t gap = k; gap > 0; gap /= 2) {
        for (size_t i = gap; i < is.size(); i += 1) {
            T temp = is.access(i)->data_;

            size_t j;
            for (j = i; j >= gap && compare(temp, is.access(j - gap)->data_); j -= gap)
                is.access(j)->data_ = is.access(j - gap)->data_;

            is.access(j)->data_ = temp;
        }
    }
}

template <typename Key, typename T>
RadixSort<Key, T>::RadixSort() : getKey_([](auto const &x) { return x; }) {}

template <typename Key, typename T>
RadixSort<Key, T>::RadixSort(std::function<Key(const T &)> getKey) : getKey_(getKey) {}

template <typename Key, typename T>
void RadixSort<Key, T>::sort(amt::ImplicitSequence<T> &is,
                             std::function<bool(const T &, const T &)> compare) {
    static_assert(std::is_integral_v<Key>, "Radix sort supports only integral types.");
    const int maxDigits = std::numeric_limits<Key>::digits10 + 1; // Max decimal digits in Key
    std::vector<std::vector<T>> buckets(10);                      // Buckets for digits 0-9

    for (int exp = 1; exp <= std::pow(10, maxDigits - 1); exp *= 10) {
        for (auto it = is.begin(); it != is.end(); ++it) {
            int digit = (getKey_(*it) / exp) % 10;
            buckets[digit].push_back(*it);
        }

        auto it = is.begin();
        for (auto &bucket : buckets) {
            for (auto &item : bucket) {
                *it++ = item;
            }
            bucket.clear();
        }
    }
}

template <typename T>
void MergeSort<T>::sort(amt::ImplicitSequence<T> &is,
                        std::function<bool(const T &, const T &)> compare) {
    //  TODO: implement
}

template <typename T> void MergeSort<T>::split(size_t n) {
    //  TODO: implement
}

template <typename T>
void MergeSort<T>::merge(std::function<bool(const T &, const T &)> compare, size_t n) {
    //  TODO: implement
}
} // namespace ds::adt
