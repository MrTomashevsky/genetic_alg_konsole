#ifndef GENETIC_HPP_INCLUDED
#define GENETIC_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <iterator>
#include <numeric>
#include <vector>

#include "random.hpp"
#include "global.hpp"

namespace genalg {

    using optimal_fitness_t = double;

    using pair    = std::pair<size_t, size_t>;
    using pairs_t = std::vector<pair>;

    template <typename fitness_t>
    class interface_genom {
      public:
        virtual const fitness_t &fitness_function() const              = 0;
        virtual const fitness_t &ideal_fitness_function() const        = 0;
        virtual bool operator<(const interface_genom &obj) const final { return fitness_function() < obj.fitness_function(); }
    };

    template <class genom, class genom_vector, class operator_t, operator_t op_div>
    class search_parents {
      public:
        constexpr static size_t
            TOURNAMENT_T = 6;

        static pairs_t panmixia(const genom_vector &vec) {
            COUT_MESSAGE_FUNC();
            if (!vec.empty()) {
                pairs_t return_value(vec.size() / 2, {0, 0});
                std::vector<bool> visted_peaks(vec.size(), false);
                size_t i = 0;
                for (intmax_t res; (res = std::accumulate(visted_peaks.cbegin(), visted_peaks.cend(), intmax_t(0))) < vec.size() - 1; i++) {
                    size_t
                        index1 = 0,
                        index2 = 0;

                    while (index1 == index2 or visted_peaks[index1] or visted_peaks[index2]) {
                        index1 = random_hpp::random(0, vec.size());
                        index2 = random_hpp::random(0, vec.size());
                    }
                    return_value[i]      = {index1, index2};
                    visted_peaks[index1] = visted_peaks[index2] = true;
                }
                return return_value;
            }
            return {};
        }

      private:
        static size_t get_good_genom(const genom_vector &vec) {

            size_t return_value = random_hpp::random(0, vec.size());
            for (size_t i = 0; i < TOURNAMENT_T; ++i) {
                int value;
                while (true) {
                    value = random_hpp::random(0, vec.size());
                    if (vec[return_value].fitness_function() <= vec[value].fitness_function()) {
                        return_value = value;
                        break;
                    }
                }
            }
            return return_value;
        }

      public:
        static pairs_t tournament(const genom_vector &vec) {
            COUT_MESSAGE_FUNC();

            if (vec.size() >= TOURNAMENT_T) {
                pairs_t return_value(vec.size(), pair{0, 0});
                for (size_t i = 0; i < return_value.size(); ++i) {
                    return_value[i].first  = get_good_genom(vec);
                    return_value[i].second = get_good_genom(vec);
                }
                return return_value;
            } else {
            }
            return {};
        }

        static pairs_t roulette(const genom_vector &vec) {
            COUT_MESSAGE_FUNC();
            std::vector<size_t> indexs;

            auto size = vec.size();
            for (size_t i = 0; i < vec.size(); ++i) {
                const optimal_fitness_t p = (op_div(vec[i].fitness_function(), vec[i].ideal_fitness_function()) / size) * 100;
                if (p >= 1 || random_hpp::probability(p)) {
                    indexs.push_back(i);
                } else {
                    --size;
                }
            }

            auto print_indexs = [indexs]() {
                for (size_t i = 0; i < indexs.size(); ++i) {
                    if (i % 10 == 0) std::cout << std::endl
                                               << "\t";
                    std::cout << indexs.size();
                    if (i != indexs.size() - 1) std::cout << ",\t";
                }

                std::cout << std::endl;
                return "";
            };

            COUT_GLOBAL_CONTROL_MESSAGE(print_indexs());

            pairs_t return_value(indexs.size() / 2, {0, 0});
            for (size_t i = 0, j = 0; i < return_value.size(); ++i) {
                return_value[i].first  = indexs[j++];
                return_value[i].second = indexs[j++];
            }

            return return_value;
        }

        static pairs_t inbreeding(const genom_vector &vec) {
            COUT_MESSAGE_FUNC();

            pairs_t return_value(vec.size() / 2, {0, 0});
            for (auto &i : return_value) {
                size_t
                    index1 = 0,
                    index2 = 0;
                while (index1 == index2) {
                    index1 = random_hpp::random(0, vec.size());
                    index2 = std::distance(vec.cbegin(), std::max_element(vec.cbegin(), vec.cend(), genom::search_for_neighbor(vec, index1, vec.cbegin() + index1, true)));
                }
            }
            return return_value;
        }

        static pairs_t outbreening(const genom_vector &vec) {
            COUT_MESSAGE_FUNC();

            pairs_t return_value(vec.size() / 2, {0, 0});
            for (auto &i : return_value) {
                size_t
                    index1 = 0,
                    index2 = 0;
                while (index1 == index2) {
                    index1 = random_hpp::random(0, vec.size());
                    index2 = std::distance(vec.cbegin(), std::min_element(vec.cbegin(), vec.cend(), genom::search_for_neighbor(vec, index1, vec.cbegin() + index1, false)));
                }
            }
            return return_value;
        }
    };

    std::vector<size_t>
    crossingover_get_points(const size_t count_of_points, const size_t max_point);

    template <class genom, class genom_vector>
    void genetic(const size_t MAX_START_POPULATION,
                 void (*calculation_fitness_functions)(genom_vector &vec),
                 bool (*end_function)(const genom_vector &vec, size_t step),
                 pairs_t (*search_parents)(const genom_vector &vec),
                 void (*crossingover)(pairs_t pairs, genom_vector &out_vec, const genom_vector &vec),
                 void (*mutation)(genom_vector &vec),
                 void (*log_print)(size_t step, const genom_vector &vec)) {
        auto begin_vec = new genom_vector();
        std::generate_n(std::back_inserter(*begin_vec), MAX_START_POPULATION, []() { return genom(); });

        for (size_t i = 0; !end_function(*begin_vec, i); i++) {

            calculation_fitness_functions(*begin_vec);

            auto tmp_vec = new genom_vector();
            // search parents and reproduction

            crossingover(search_parents(*begin_vec), *tmp_vec, *begin_vec);
            delete begin_vec;

            // mutation
            mutation(*tmp_vec);

            // log print
            log_print(i, *tmp_vec);

            begin_vec = tmp_vec;
        }

        delete begin_vec;
    }
}


#endif // !GENETIC_HPP_INCLUDED
