#ifndef FITNESS_HPP_INCLUDED
#define FITNESS_HPP_INCLUDED

#include <algorithm>
#include <numeric>
#include <vector>

#include "genetic.hpp"
#include "global.hpp"

namespace byte_code {

    using fitness_t = std::vector<size_t>;

    bool is_null_fitness(const fitness_t &obj);

    size_t sum_fitness(const fitness_t &obj1, const fitness_t &obj2 = global_control().graphs.paths);

    size_t operator/(const byte_code::fitness_t &obj1, const byte_code::fitness_t &obj2);

}  // namespace byte_code

#endif  // FITNESS_HPP_INCLUDED
