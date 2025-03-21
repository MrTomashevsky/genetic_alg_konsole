#ifndef RANDOM_HPP_INCLUDED
#define RANDOM_HPP_INCLUDED

#include <climits>
#include <iostream>
#include <random>

namespace random_hpp {

    int random(int begin, int end);

    int random();

    bool probability(double your_probability);
}  // namespace random_hpp

#endif  // !RANDOM_HPP_INCLUDED
