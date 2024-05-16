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
    int n = is.size();
    bool swap;
    int current, parent, left, right, max;

    // Build heap
    for (int i = 1; i < n; i++) {
        current = i;
        do {
            swap = false;
            parent = (current - 1) / 2;
            if (current > 0 && compare(is.access(parent)->data_, is.access(current)->data_)) {
                std::swap(is.access(current)->data_, is.access(parent)->data_);
                current = parent;
                swap = true;
            }
        } while (swap);
    }

    // Extract elements from heap
    for (int i = n - 1; i > 0; i--) {
        std::swap(is.access(0)->data_, is.access(i)->data_);
        current = 0;
        do {
            swap = false;
            left = 2 * current + 1;
            right = 2 * current + 2;
            if (left < i && right < i) {
                max = compare(is.access(left)->data_, is.access(right)->data_) ? right : left;
            } else {
                max = (left < i) ? left : right;
            }
            if (max < i && compare(is.access(current)->data_, is.access(max)->data_)) {
                std::swap(is.access(current)->data_, is.access(max)->data_);
                current = max;
                swap = true;
            }
        } while (swap);
    }
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
    // queue1_ = new ImplicitQueue<T>(is.size());
    // queue2_ = new ImplicitQueue<T>(is.size());
    // mergeQueue_ = new ImplicitQueue<T>(is.size());

    // // Initialize mergeQueue_ with elements from inputSequence
    // for (const T &element : is) {
    //     mergeQueue_->push(element);
    // }

    // size_t i{1};

    // for (; i < is.size() - 1;) {
    //     split(i);
    //     merge(compare, i);
    //     i = i * 2;
    // }

    // split(i);
    // merge(compare, i);

    // // Transfer sorted elements back to is
    // for (size_t i = 0; i < is.size() - 1; ++i) {
    //     is.access(i)->data_ = mergeQueue_->pop();
    // }

    // delete queue1_;
    // delete queue2_;
    // delete mergeQueue_;
    // queue1_ = nullptr;
    // queue2_ = nullptr;
    // mergeQueue_ = nullptr;
}

template <typename T> void MergeSort<T>::split(size_t n) {
    // int count = 0;
    // bool isFirst = true;

    // while (!mergeQueue_->isEmpty()) {
    //     if (count % n == 0) {
    //         count = 0;
    //         isFirst = !isFirst;
    //     }

    //     if (isFirst) {
    //         queue1_->push(mergeQueue_->pop());
    //     } else {
    //         queue2_->push(mergeQueue_->pop());
    //     }
    //     ++count;
    // }
}

template <typename T>
void MergeSort<T>::merge(std::function<bool(const T &, const T &)> compare, size_t n) {
    // int firstCount = 0;
    // int secondCount = 0;

    // do {
    //     if (firstCount == 0 && secondCount == 0) {
    //         firstCount = std::min(n, queue1_->size());
    //         secondCount = std::min(n, queue2_->size());
    //     }

    //     const T *key1 = (firstCount > 0) ? &queue1_->peek() : nullptr;
    //     const T *key2 = (secondCount > 0) ? &queue2_->peek() : nullptr;

    //     if (key1 != nullptr && key2 != nullptr) {
    //         if (compare(*key1, *key2)) {
    //             mergeQueue_->push(queue1_->pop());
    //             --firstCount;
    //         } else {
    //             mergeQueue_->push(queue2_->pop());
    //             --secondCount;
    //         }
    //     } else {
    //         if (key1 != nullptr) {
    //             mergeQueue_->push(queue1_->pop());
    //             --firstCount;
    //         } else if (key2 != nullptr) {
    //             mergeQueue_->push(queue2_->pop());
    //             --secondCount;
    //         }
    //     }
    // } while (!queue1_->isEmpty() || !queue2_->isEmpty());
}
} // namespace ds::adt
