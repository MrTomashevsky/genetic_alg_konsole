#include "byte_code_genetic.hpp"

namespace byte_code {

    void print(std::string str) {
        COUT_MESSAGE_IS_ERROR(global_control().fout_result);
        (*global_control().fout_result) << str << std::flush;
        str.erase(std::remove_if(str.begin(), str.end(), [](char c) {
                      return c == '\n';
                  }),
                  str.end());
        output(str);
    }

    inline size_t genom::create_count_commands() {
        return MIN_SIZE_COM + random_hpp::random(0, COEF_SIZE_COM);
    }

    size_t genom::generate_text::count_of_vars_this_types(type_t command_code) const {
        return std::count_if(stack_types.cbegin(), stack_types.cend(), [command_code](auto &i) { return i == command_code; });
    }

    auto genom::generate_text::find_in_stack_types(type_t command_code) const {
        if (std::find(stack_types.cbegin(), stack_types.cend(), command_code) == stack_types.cend())
            return stack_types.cend();
        while (true) {
            auto index = random_hpp::random(0, stack_types.size());
            if (stack_types[index] == command_code)
                return stack_types.cbegin() + index;
        }
        return stack_types.cend();
    }

    void genom::generate_text::update_stack_types(const uint8_t command_code) {
        switch (command_code) {
            case alloc_int:
            case alloc_int_const:
                stack_types.push_back(INT);
                stack_pointers.push_back(0);
                break;
            case alloc_arr:
            case alloc_arr_const:
            case alloc_arr_p_const:
            case alloc_arr_copy:
                stack_types.push_back(INTARR);
                stack_pointers.push_back(0);
                break;
            case _while:
            case _if:
                vid.push_back(0);
                break;
            case _end_if:
            case _continue:
            case _end_while:
            case _break:
                for (size_t i = 0; i < vid.back(); i++) {
                    stack_types.pop_back();
                    stack_pointers.pop_back();
                }
                vid.pop_back();
                break;
            case exit:
                break;
            case _function:
                break;
            case _return:
            case _end_function:
                for (size_t i = 0; i < vid.back(); i++) {
                    stack_types.pop_back();
                    stack_pointers.pop_back();
                }
                vid.pop_back();
                inc_adress -= vec_inc_adress.back();
                vec_inc_adress.pop_back();
                break;
            case call:
                vec_inc_adress.push_back(vid.back());
                inc_adress += vid.back();
                vid.push_back(0);
                break;
            default:
                break;
        }
    }

    inline uint8_t genom::generate_text::generate_command_code() {
        uint8_t command_code = exit;
        while (true) {
            do
                command_code = random_hpp::random(alloc_int, next_permutation);
            while (is_ios_command(command_code) || command_code == alloc_arr || command_code == alloc_arr_const || command_code == set_const);

            if (!is_alloc_command(command_code)) {
                if (std::find_if(commands[command_code].type.cbegin(), commands[command_code].type.cend(),
                                 [this](auto &i) {
                                     return find_in_stack_types(i) == stack_types.cend();
                                 }) != commands[command_code].type.cend()) {
                    continue;
                }

                if (command_code == _else && (com_stack.empty() || com_stack.back() != _if))
                    continue;

                if (is_end_module(command_code))
                    if (!com_stack.empty() && get_pair_command(com_stack.back()) == command_code)
                        com_stack.pop_back();
                    else continue;
            }

            if ((command_code == _continue || command_code == _break || command_code == _return) && vid.empty() ||
                (command_code == _continue || command_code == _break) && while_adresses.empty() ||
                command_code == _return && func_adresses.empty())
                continue;
            break;
        }

        if (is_begin_module(command_code)) {
            if (command_code == _else && (com_stack.empty() || com_stack.back() != _if))
                command_code = _if;
            if (command_code != _else)
                com_stack.push_back(command_code);
        }
        return command_code;
    }

    bool genom::generate_text::set_command_args(const uint8_t command_code, byte_code_command &back) {
        size_t tmp = 0;

        if (is_arr_change_command(command_code) && (back[tmp++] = std::distance(stack_types.cbegin(), find_in_stack_types(INTARR))) == 0) {
            switch (command_code) {
                case move_arr:
                    if (count_of_vars_this_types(INTARR) == 1)
                        return true;

                    while (back[tmp] = std::distance(stack_types.cbegin(), find_in_stack_types(INTARR)) == 0)
                        ;
                    break;
                case set_p:
                    if (!count_of_vars_this_types(INTP))
                        return true;
                    back[tmp++] = std::distance(stack_types.cbegin(), find_in_stack_types(INTP));
                    back[tmp++] = std::distance(stack_types.cbegin(), find_in_stack_types(INTP));
                    break;
                case set_p_const:
                    if (!count_of_vars_this_types(INTP))
                        return true;
                    back[tmp++] = std::distance(stack_types.cbegin(), find_in_stack_types(INTP));
                    back[tmp++] = random_hpp::random();
                    break;
            }
        } else
            for (auto &i : byte_code_basic::commands[command_code].type) {
                switch (i) {
                    case ENUM_ARGUMENTS: {
                        const auto count = random_hpp::random(0, byte_code_basic::MAX_COUNT_ARG);
                        back[tmp++]      = count;
                        back.reserve(count);
                        for (; tmp < back.get_length(); tmp++)
                            back[tmp] = random_hpp::random();
                    } break;
                    case SIZE_T:
                        back[tmp] = random_hpp::random(0, byte_code_basic::MAX_COUNT_ARG);
                        break;
                    case INT:
                        back[tmp] = random_hpp::random();
                        break;
                    case FUNCTION:
                    case INTP:
                    case INTARR:
                        back[tmp] = std::distance(stack_types.cbegin(), find_in_stack_types(i));
                        break;
                    case SWITCH_ARGUMENT:
                        back[tmp] = random_hpp::random(0, stack_types.size());
                        break;
                    case STRING:
                        COUT_GLOBAL_CONTROL_MESSAGE("\n\tWHAT?");
                        break;
                }
                ++tmp;
            }
        return false;
    }

    genom::generate_text genom::generate_text::go_to_index(size_t index, const byte_code_t &text) {
        generate_text syn;
        for (size_t i = 0; i < index; i++)
            syn.update_stack_types(text[i].get_command());
        return syn;
    }

    genom::genom(std::nullptr_t) {
        count_commands = create_count_commands();
        text           = byte_code_t(count_commands, byte_code_command(exit));
    }

    genom::genom() {
        size_t n = 0;
        text.clear();
        count_commands = create_count_commands();
        generate_text syn;

        for (size_t i = 0; i < count_commands; i++) {
            uint8_t command_code = syn.generate_command_code();
            byte_code_command bc_command(command_code);

            if (command_code == _while) {
                syn.while_adresses.push_back(text.cbegin() + text.size());
            } else if (command_code == _end_while) {
                syn.while_adresses.pop_back();
            }

            if (command_code == _function) {
                syn.func_adresses.push_back(text.cbegin() + text.size());
            } else if (command_code == _end_function) {
                syn.func_adresses.pop_back();
            }

            syn.update_stack_types(command_code);
            if (syn.set_command_args(command_code, bc_command)) {
                --i;
                continue;
            }

            text.push_back(bc_command);
        }

        while (!syn.com_stack.empty()) {
            auto command_code = get_pair_command(syn.com_stack.back());
            text.push_back(byte_code_command(command_code));
            syn.set_command_args(command_code, text.back());
            syn.com_stack.pop_back();
        }
    }

    void genom::calculation_fintess_function() {
        size_t index_graph = 0;
        for (auto &length : length_vec) {
            length = 0;

            bool inf_flag          = 0;
            bool result_is_changed = false;
            auto &matr             = global_control().graphs.graphs[index_graph++];

            const auto result = byte_code_interpreter(text, true).interpreter(matr, false, &result_is_changed);
            if (result_is_changed) {
                if (result != ARR_INT_T{}) {
                    for (size_t i = 0; i < result.size() - 1; i++) {
                        if (inf_flag = result[i] >= matr.size() || result[i + 1] >= matr[result[i]].size() || matr[result[i]][result[i + 1]] == INF)
                            break;
                        length += matr[result[i]][result[i + 1]];
                    }

                    if (inf_flag)
                        length = 0;
                    else
                        length += matr[result[result.size() - 1]][result[0]];
                }
            }
        }
    }

    std::string genom::to_esc_string() const {
        return byte_code::to_esc_string(text);
    }

    std::string genom::to_string() const {
        return byte_code::to_string(text);
    }

    void calculation_fitness_functions(genom_vector &vec) {
        for (auto &i : vec) {
            i.calculation_fintess_function();
        }
    }

    bool end_function(const genom_vector &vec, size_t step) {

        if (std::chrono::steady_clock::now() >= global_control().clock) {
            print("TIME\n");
            global_control().result_code = NO_NORMAL_RESULT;
        } else if (vec.size() >= 500000) {
            print("THE BIG POPULATION\n");
            global_control().result_code = NO_NORMAL_RESULT;
        } else {
            global_control().result_code = NO_RESULT;
            if (!vec.empty()) {
                for (const auto &it : vec) {
                    const auto &fitness_function = it.fitness_function();

                    if (sum_fitness(fitness_function)) {
                        global_control().result_code = NORMAL_RESULT;

                        std::stringstream ss;
                        ss << "GENOM " << step << " [";
                        for (const auto &i : fitness_function) {
                            ss << i << " ";
                        }
                        ss << "]\n";
                        ss << it.to_string() << std::endl;

                        print(ss.str());
                    }
                }
            } else {
                global_control().result_code = NO_NORMAL_RESULT;
                std::string str     = "No result";

                *global_control().fout_result << str << std::flush;
                output(str);
            }
        }
        return global_control().result_code != NO_RESULT;
    }

    void crossingover(genalg::pairs_t pairs, genom_vector &out_vec, const genom_vector &vec) {
        COUT_MESSAGE_FUNC();

        for (auto &i : pairs) {
            const auto count = random_hpp::random(0, global_control().COUNT_POINTS);
            for (size_t tmp_i = 0; tmp_i < count; tmp_i++) {
                out_vec.push_back(genom(nullptr));
                const auto points = genalg::crossingover_get_points(global_control().COUNT_POINTS, vec.back().text.size());
                auto it           = points.begin();
                bool flag         = true;
                size_t index      = i.first;

                for (size_t j = 0; j < out_vec.back().text.size() && j < vec[index].text.size(); j++) {

                    if (it != points.end() && *(it + 1) == j)
                        flag = !flag;

                    if (flag) {
                        if (j < vec[i.first].text.size())
                            index = i.first;
                        else if (j < vec[i.second].text.size())
                            index = i.second;
                    } else {
                        if (j < vec[i.second].text.size())
                            index = i.second;
                        else if (j < vec[i.first].text.size())
                            index = i.first;
                    }

                    out_vec.back().text[j] = vec[index].text[j];
                }
            }
        }
    }

    void mutation(genom_vector &vec) {
        COUT_MESSAGE_FUNC();

        for (auto &i : vec) {
            if (random_hpp::probability(global_control().PROBABILITY_MUTATION)) {
                const auto index_in_text = random_hpp::random(0, i.text.size());

                switch (random_hpp::random(0, 3)) {
                    case 0: {  // change
                        auto syn = genom::generate_text::go_to_index(index_in_text, i.text);
                        genom::byte_code_command command_code(syn.generate_command_code());
                        while (true) {
                            if (!syn.set_command_args(command_code.get_command(), command_code))
                                break;
                            command_code = genom::byte_code_command(syn.generate_command_code());
                        }
                        i.text[index_in_text] = command_code;
                    } break;
                    case 1: {  // delete
                        i.text.erase(i.text.begin() + index_in_text);
                    } break;
                    case 2: {  // insert
                        auto syn = genom::generate_text::go_to_index(index_in_text, i.text);
                        genom::byte_code_command command_code(syn.generate_command_code());
                        while (true) {
                            if (!syn.set_command_args(command_code.get_command(), command_code))
                                break;
                            command_code = genom::byte_code_command(syn.generate_command_code());
                        }
                        auto it = i.text.insert(i.text.begin() + index_in_text, command_code);

                    } break;
                }
            }
        }
    }

    void log_print(size_t step, const genom_vector &vec) {
        COUT_MESSAGE_FUNC();
        QString s;
        QTextStream qts(&s);
        qts << step << ", " << vec.size() << ", ";
        if (vec.size()) {
            const auto tmp = std::max_element(vec.begin(), vec.end(), [](auto &a, auto &b) { return a.fitness_function() < b.fitness_function(); })->fitness_function();

            qts << "{";
            for (const auto &i : tmp) {
                qts << i << " ";
            }
            qts << "}";
        }

        qts << "\n";

        print(s.toStdString());
    }

    void genetic(const size_t MAX_POPULATION, const size_t index_selection) {

        static genalg::pairs_t (*sp[])(const genom_vector &vec) = {
            panmixia,
            inbreeding,
            outbreening,
            tournament,
            roulette};

        genalg::genetic<genom, genom_vector>(MAX_POPULATION,
                                             calculation_fitness_functions,
                                             end_function,
                                             sp[index_selection],
                                             crossingover,
                                             mutation,
                                             log_print);
    };

}  // namespace byte_code
