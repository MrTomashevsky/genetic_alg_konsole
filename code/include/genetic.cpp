#include "genetic.hpp"

namespace genalg {

    std::vector<size_t> crossingover_get_points(const size_t count_of_points, const size_t max_point) {
        std::vector<size_t> return_value(count_of_points, 0);
        for (size_t i = 0; i < count_of_points;) {
            auto new_point = random_hpp::random() % max_point;
            if (std::find(return_value.cbegin(), return_value.cend(), new_point) == return_value.cend())
                return_value[i++] = new_point;
        }
        std::sort(return_value.begin(), return_value.end());
        return return_value;
    }

}  // namespace genalg
