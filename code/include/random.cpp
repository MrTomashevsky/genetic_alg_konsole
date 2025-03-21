#include "random.hpp"

namespace random_hpp {

    int random(int begin, int end) {
        static const struct s {
            time_t tmp;
            s() {
                tmp = time(NULL);
                srand(tmp);
            }
            auto operator()() const { return rand(); }
        } ss;

        if (!end) end = 1;
        return ss() % end + begin;
    }

    int random() {
        return random(0, INT_MAX);
    }

    bool probability(double your_probability) {
        return double(random(0, 100)) / 100 <= your_probability;
    }
}  // namespace random_hpp
