#include "fitness.hpp"

namespace byte_code {

    bool is_null_fitness(const fitness_t &obj) {
        return std::accumulate(obj.cbegin(), obj.cend(), 0) == 0;
    }

    size_t sum_fitness(const fitness_t &obj1, const fitness_t &obj2) {
        size_t sum = 0;
        for (size_t i = 0; i < obj1.size(); ++i) {
            if (obj1[i])
                sum += obj1[i] <= obj2[i] * 1.25;
        }
        return sum;
    }

    size_t operator/(const byte_code::fitness_t &obj1, const byte_code::fitness_t &obj2) {
        return byte_code::sum_fitness(obj1, obj2);
    }
}  // namespace byte_code
