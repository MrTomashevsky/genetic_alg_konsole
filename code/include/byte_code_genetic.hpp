#ifndef BYTE_CODE_GEN_HPP_INCLUDED
#define BYTE_CODE_GEN_HPP_INCLUDED

#include <ctime>
#include <mutex>
#include <sstream>

#include "byte_code.hpp"
#include "fitness.hpp"
#include "genetic.hpp"
#include "global.hpp"
#include "random.hpp"

namespace byte_code {

    class genom : public byte_code_basic, public genalg::interface_genom<fitness_t> {
        byte_code_t text;
        size_t count_commands = 0;
        fitness_t length_vec  = fitness_t(global_control().graphs.graphs.size(), 0);

      public:
        constexpr static size_t
            COEF_SIZE_COM = 100,
            MIN_SIZE_COM  = 100;

        static inline double
            PROBABILITY_MUTATION = 0.1;

      private:
        inline static size_t create_count_commands();

      public:
        using genom_vector = std::vector<genom>;

        friend struct generate_text;

        friend void calculation_fitness_functions(genom_vector &vec);
        friend bool end_function(const genom_vector &vec, size_t step);
        friend void crossingover(genalg::pairs_t pairs, genom_vector &out_vec, const genom_vector &vec);
        friend void mutation(genom_vector &vec);
        friend void log_print(size_t step, const genom_vector &vec);

        struct generate_text {
            struct module_coords {
                size_t begin, end;
            };

            std::vector<uint8_t> com_stack;
            std::vector<type_t> stack_types;
            std::vector<INT_T> stack_pointers;
            std::vector<size_t> vid;
            size_t inc_adress = 0;
            std::vector<size_t> vec_inc_adress;
            std::vector<byte_code_t::const_iterator> while_adresses;
            std::vector<byte_code_t::const_iterator> func_adresses;

            generate_text() : vid(1, 1), vec_inc_adress(1, 0), stack_types(1, INTARR), stack_pointers(1, 0) {}

            size_t count_of_vars_this_types(type_t command_code) const;
            auto find_in_stack_types(type_t command_code) const;
            void update_stack_types(const uint8_t command_code);
            inline uint8_t generate_command_code();
            bool set_command_args(const uint8_t command_code, byte_code_command &back);
            static generate_text go_to_index(size_t index, const byte_code_t &text);
        };

        genom(byte_code_t bc) : text(bc), count_commands(text.size()) {}
        genom(std::nullptr_t);
        genom();
        void calculation_fintess_function();

        const fitness_t &fitness_function() const override {
            return length_vec;
        }

        const fitness_t &ideal_fitness_function() const override {
            return global_control().graphs.paths;
        }

        std::string to_esc_string() const;
        std::string to_string() const;

        static auto search_for_neighbor(const genom_vector &vec, size_t index1, const genom_vector::const_iterator ignor, const bool flag) {
            return [vec, index1, ignor, flag](const genom &g1, const genom &g2) {
                const auto pred = [vec, index1](const genom &g) {
                    auto it = g.text.cbegin();
                    return std::count_if(vec[index1].text.cbegin(), vec[index1].text.cend(), [&it, g](const byte_code::byte_code_basic::byte_code_command &text) {
                        return it < g.text.cend() && text == *it++;
                    });
                };
                return g1.text == ignor->text ? flag : g2.text == ignor->text ? !flag
                                                                              : pred(g1) < pred(g2);
            };
        }
    };

    using genom_vector  = genom::genom_vector;
    using search_parents = genalg::search_parents<genom, genom_vector, decltype(operator/), operator/ >;

    constexpr auto
        panmixia    = search_parents::panmixia,
        inbreeding  = search_parents::inbreeding,
        outbreening = search_parents::outbreening,
        tournament  = search_parents::tournament,
        roulette    = search_parents::roulette;

    void calculation_fitness_functions(genom_vector &vec);
    bool end_function(const genom_vector &vec, size_t step);
    void crossingover(genalg::pairs_t pairs, genom_vector &out_vec, const genom_vector &vec);
    void mutation(genom_vector &vec);
    void log_print(size_t step, const genom_vector &vec);

    void genetic(const size_t MAX_POPULATION, const size_t index_selection);

}  // namespace byte_code

#endif // !BYTE_CODE_GEN_HPP_INCLUDED
