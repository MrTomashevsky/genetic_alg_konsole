#ifndef ARRAY_DIGITS_HPP
#define ARRAY_DIGITS_HPP

#include <vector>

template <typename Type>
struct array_digits {
    struct digit {
        Type value, max, min;
    };

    std::vector<digit> arr;

    array_digits() {
        reset();
    }

    void update() {
        for (auto &i : arr) {
            if (!(i.value < i.max && i.value >= i.min)) {
                i.value = i.value + i.min % i.max;
            }
        }
    }

    const Type &operator[](size_t i) const {
        return arr[i].value;
    }

    const digit &get(size_t i) const {
        return arr[i];
    }

    void reset() {
        for (auto &i : arr) {
            i.value = i.min;
        }
    }

    void inc() {
        for (int i = arr.size() - 1; i >= 0; --i) {
            if (arr[i].value < arr[i].max) {
                ++arr[i].value;
                break;
            } else {
                arr[i].value = arr[i].min;
            }
        }
    }

    void dec() {
        for (int i = arr.size() - 1; i >= 0; --i) {
            if (arr[i].value > arr[i].min) {
                --arr[i].value;
                break;
            } else {
                arr[i].value = arr[i].max;
            }
        }
    }

    bool is_min() const {
        for (int i = 0; i < arr.size(); ++i)
            if (arr[i].value != arr[i].min)
                return false;
        return true;
    }

    bool is_max() const {
        for (int i = 0; i < arr.size(); ++i)
            if (arr[i].value != arr[i].max)
                return false;
        return true;
    }
};

#endif  // ARRAY_DIGITS_HPP
