#include "byte_code.hpp"
#include "global.hpp"

namespace byte_code {

    void byte_code_basic::byte_code_command::reserve(size_t count) {
        arr.reserve(count);
    }

    byte_code_basic::INT_T &byte_code_basic::byte_code_command::operator[](size_t i) {
        return arr[i];
    }

    const byte_code_basic::INT_T &byte_code_basic::byte_code_command::operator[](size_t i) const {
        return arr[i];
    }

    uint16_t byte_code_basic::byte_code_command::get_length() const {
        return arr.size();
    }

    uint8_t byte_code_basic::byte_code_command::get_command() const {
        return command;
    }

    bool byte_code_basic::byte_code_command::operator==(commands_t c) const {
        return c == command;
    }

    bool byte_code_basic::byte_code_command::operator==(const byte_code_command &c) const {
        return c.command == command;
    }

    std::string byte_code_basic::byte_code_command::to_esc_string() const {
        std::stringstream out;

        out << "\033[34;1m" << commands[(*this).command].name << "\033[0m ";
#define print_digit(digit) "\033[32;1m" << digit << "\033[0m"

        for (size_t i = 0; i < commands[(*this).command].type.size(); i++) {
            switch (byte_code_basic::commands[(*this).command].type[i]) {
                case byte_code_basic::INT:
                case byte_code_basic::SIZE_T:
                    out << print_digit((*this)[i]);
                    break;
                case byte_code_basic::INTARR:
                    out << "\033[30;1m&\033[0m" << print_digit((*this)[i]);
                    break;
                case byte_code_basic::FUNCTION:
                case byte_code_basic::SWITCH_ARGUMENT:
                case byte_code_basic::INTP:
                    out << "\033[30;1m&\033[0m" << print_digit((*this)[i]);
                    break;
                case byte_code_basic::STRING:
                    out << "\033[33m\"" << *((std::string *)(*this)[i]) << "\"\033[0m";
                    break;
                case byte_code_basic::ENUM_ARGUMENTS:
                    out << print_digit((*this)[i]);
                    const auto index_in_arr = i++;
                    for (size_t j = 0; j < (*this)[index_in_arr]; j++) {
                        out << " " << print_digit((*this)[i]);
                    }
                    break;
            }
            out << " ";
        }

#undef print_digit
        return out.str();
    }

    std::string byte_code_basic::byte_code_command::to_string() const {
        std::stringstream out;
        out << commands[(*this).command].name << " ";

        for (size_t i = 0; i < commands[(*this).command].type.size(); i++) {
            switch (byte_code_basic::commands[(*this).command].type[i]) {
                case byte_code_basic::INT:
                case byte_code_basic::SIZE_T:
                    out << (*this)[i];
                    break;
                case byte_code_basic::INTARR:
                    out << "&" << (*this)[i];
                    break;
                case byte_code_basic::FUNCTION:
                case byte_code_basic::SWITCH_ARGUMENT:
                case byte_code_basic::INTP:
                    out << "&" << (*this)[i];
                    break;
                case byte_code_basic::STRING:
                    out << "\"" << *((std::string *)(*this)[i]) << "\"";
                    break;
                case byte_code_basic::ENUM_ARGUMENTS:
                    out << (*this)[i];
                    const auto index_in_arr = i++;
                    for (size_t j = 0; j < (*this)[index_in_arr]; j++) {
                        out << " " << (*this)[i];
                    }
                    break;
            }
            out << " ";
        }
        return out.str();
    }

    byte_code_basic::ARR_INT_T *byte_code_basic::default_array(size_t size) {
        size_t i = 0;

        auto result = new ARR_INT_T(size, 0);
        COUT_MESSAGE_IS_ERROR(result);
        std::generate(result->begin(), result->end(), [&i]() { return i++; });
        return result;
    }

    bool byte_code_basic::is_ios_command(uint8_t it) {
        return it == commands_t::scan ||
               it == commands_t::puts ||
               it == commands_t::print;
    }

    bool byte_code_basic::is_begin_module(uint8_t it) {
        return it == commands_t::_module ||
               it == commands_t::_if ||
               it == commands_t::_else ||
               it == commands_t::_while ||
               it == commands_t::_function;
    }

    bool byte_code_basic::is_end_module(uint8_t it) {
        return it == commands_t::_end_module ||
               it == commands_t::_end_if ||
               it == commands_t::_end_while ||
               it == commands_t::_end_function;
    }

    bool byte_code_basic::is_alloc_command(uint8_t it) {
        return it == commands_t::alloc_int_const ||
               it == commands_t::alloc_arr ||
               it == commands_t::alloc_arr_const;
    }

    bool byte_code_basic::is_arr_change_command(uint8_t it) {
        return it == move_arr ||
               it == set_p ||
               it == set_p_const;
    }

    uint8_t byte_code_basic::get_pair_command(uint8_t first_command_code) {
        static const std::map<uint8_t, uint8_t> m = {
            {_module, _end_module},
            {_if, _end_if},
            {_else, _end_if},
            {_while, _end_while},
            {_function, _end_function}};
        auto result = m.find(first_command_code);
        return result->second;
    }

    byte_code_basic::byte_code_t::const_iterator byte_code_basic::if_goto_else_or_end_if(byte_code_t::const_iterator it, byte_code_t::const_iterator end, bool &error_flag, std::vector<bool> &if_stack) {
        intmax_t investigation = 1, if_investigation = 1;
        while (it < end) {
            if (*it == _if) {
                ++if_investigation;
                ++investigation;
            } else if (*it == _else) {
                --if_investigation;
                if (!investigation or !if_investigation) {
                    ++it;
                    if_stack.pop_back();
                    break;
                }
                if (error_flag = investigation <= 0)
                    return it;
            } else if (*it == _end_if) {
                --investigation;
                if (!investigation or !if_investigation) {
                    ++it;
                    break;
                }
                if (error_flag = investigation <= 0)
                    return it;
            }
            if (it < end) ++it;
        }
        --it;
        return it;
    }

    byte_code_basic::byte_code_t::const_iterator byte_code_basic::investigation(byte_code_t::const_iterator it, byte_code_t::const_iterator end, bool &error_flag, const std::initializer_list<uint8_t> inc_commands, const std::initializer_list<uint8_t> dec_commands, bool (*func)(intmax_t)) {
        if (it < end) ++it;
        intmax_t investigation = 1;
        while (it < end) {
            if (std::find(inc_commands.begin(), inc_commands.end(), it->get_command()) != inc_commands.end())
                ++investigation;
            else if (std::find(dec_commands.begin(), dec_commands.end(), it->get_command()) != dec_commands.end()) {
                --investigation;
                if (!investigation) {
                    ++it;
                    break;
                }
                if (error_flag = func(investigation))
                    return it;
            }
            ++it;
        }
        --it;
        return it;
    }

    void byte_code_interpreter::result_change_commands_push_back(const byte_code_t::const_iterator &it, array_of_args_t args, bool &its_result_change_command, const size_t &tmp) {

        if (its_result_change_command = !(inc_adress + args[tmp]) &&
                                        !its_result_change_command &&
                                        !is_alloc_command(it->get_command()) &&
                                        !tmp &&
                                        is_arr_change_command(it->get_command())) {

            if (it->get_command() == move_arr) {
                if (inc_adress + args[tmp + 1]) {
                    const auto
                        &result = *((ARR_INT_T *)stack_pointers[0]),
                        &arr    = *((ARR_INT_T *)stack_pointers[inc_adress + args[tmp + 1]]);

                    auto tmp_arr = arr;
                    std::sort(tmp_arr.begin(), tmp_arr.end());
                    const auto default_arr = default_array(result.size());
                    if (*default_arr == tmp_arr) {
                        result_change_commands.push_back(it->get_command());
                    }

                    delete default_arr;
                }
            } else
                result_change_commands.push_back(it->get_command());
        }
    }

    void byte_code_interpreter::_error_interpreter(const char *ysl, size_t LINE, size_t index_str, const byte_code_t::const_iterator &it, size_t count_commands, jmp_buf &buf, bool is_main_interpreter, size_t &error_line, size_t &error_str_index, std::stringstream &output) {
        if (is_main_interpreter) {
            constexpr const char *const str[] = {
                "",
                "????????? ? ???????????? ??????",
                "?????????????? ????, ?????????? ????????, ? ???? ?????????",
                "error_line: array length equal 0",
                "???????? ?????? else, end_if ??? end_while",
                "???????? else ??? if",
                "???????? ?????? end_if, end_while ??? break",
                "calling \"end_while\" out of while circle",
                "calling \"break\" out of while circle",
                "error_line in switch_argument",
                "error_line: this function is defined",
                "calling \"return\" out of function",
                "calling \"end_function\" out of function",
                "error_line in calling function",
                "unknown commands: error_line in interpreter",
                "set_const: ????? ??????? ?? ??????? ???????",
                "??????? ??????????? ? ?????? ??????",
                "graph_p: index more than graph size"};

            output << "\nERROR IN BYTE_CODE INTERPRETER:"
                   << "\n \"" << ysl << "\""
                   << "\n message: \"" << str[index_str - 1] << "\""
                                                                "\n source file name: \""
                   << __FILE__ << "\""
                                  "\n source code line: "
                   << LINE << "\n\n command:  " << commands[it->get_command()].name << "\n executable code line: " << count_commands << "\n\n";
        }
        error_line = LINE;
        longjmp(buf, error_str_index = index_str);
    }

    void byte_code_interpreter::pop_back() {
        if (stack_types.back() == type_t::INTARR)
            delete (ARR_INT_T *)stack_pointers.back();
        stack_pointers.pop_back();
        stack_types.pop_back();
    }

    void byte_code_interpreter::push_back(INT_T value, type_t type) {
        if (type == INTARR)
            COUT_MESSAGE_IS_ERROR(value);
        stack_types.push_back(type);
        stack_pointers.push_back(value);
        ++vid.back();
    }

    byte_code_basic::ARR_INT_T byte_code_interpreter::end_interpreting(bool *result_is_changed) {
        if (result_is_changed) {
            if (result_change_commands.empty()) {
                *result_is_changed = false;
            } else {
                for (size_t i = 0; i < result_change_commands.size(); i++) {
                    if (result_change_commands[i] == reverse) {
                        result_change_commands.erase(result_change_commands.begin() + i);
                        --i;
                    } else break;
                }
                *result_is_changed = !result_change_commands.empty();
            }
        }

        return *((ARR_INT_T *)stack_pointers.front());
    }

    size_t byte_code_interpreter::find(const std::vector<INT_T> &stack_pointers, const std::vector<type_t> &stack_types, INT_T value, type_t type) {
        for (auto it_types = stack_types.begin();; it_types++) {
            it_types         = std::find(it_types, stack_types.end(), type);
            const auto index = std::distance(stack_types.begin(), it_types);
            if (it_types == stack_types.end())
                break;
            if (value == stack_pointers[index])
                return index;
        }
        return EOF;
    }

    void byte_code_interpreter::get_command_args(const size_t &count_commands, byte_code_t::const_iterator &it, array_of_args_t args, std::string &strarg) {
        bool its_result_change_command = false;

        size_t tmp = 0;
        for (auto &i : commands[it->get_command()].type) {
            switch (i) {
                case ENUM_ARGUMENTS: {
                    args[tmp]               = (*it)[tmp];
                    const auto index_in_arr = tmp++;

                    error_interpreter(args[index_in_arr] + tmp - 1 >= MAX_COUNT_ARG, 1);

                    for (size_t j = 0; j < args[index_in_arr]; j++) {
                        args[tmp] = (*it)[tmp];
                        ++tmp;
                    }

                } break;
                case INT:
                case SIZE_T:
                    args[tmp] = (*it)[tmp];
                    break;
                case FUNCTION:
                    args[tmp] = (*it)[tmp];
                    error_interpreter(inc_adress + args[tmp] >= stack_pointers.size(), 2);
                    error_interpreter(stack_types[inc_adress + args[tmp]] != FUNCTION && stack_types[inc_adress + args[tmp]] != LINK_FUNCTION, 3);
                    if (allow_error_output && stack_types[inc_adress + args[tmp]] == LINK_FUNCTION) {
                        args[tmp] = stack_pointers[inc_adress + args[tmp]];
                    }
                    break;
                case INTP:
                    args[tmp] = (*it)[tmp];
                    error_interpreter(inc_adress + args[tmp] >= stack_pointers.size(), 2);
                    error_interpreter(stack_types[inc_adress + args[tmp]] != INT && stack_types[inc_adress + args[tmp]] != LINK_INT, 3);
                    if (allow_error_output && stack_types[inc_adress + args[tmp]] == LINK_INT) {
                        args[tmp] = stack_pointers[inc_adress + args[tmp]];
                    }
                    break;
                case INTARR:
                    args[tmp] = (*it)[tmp];
                    error_interpreter(inc_adress + args[tmp] >= stack_pointers.size(), 2);
                    error_interpreter(stack_types[inc_adress + args[tmp]] != INTARR && stack_types[inc_adress + args[tmp]] != LINK_INTARR, 3);
                    if (allow_error_output && stack_types[inc_adress + args[tmp]] == LINK_INTARR) {
                        args[tmp] = stack_pointers[inc_adress + args[tmp]];
                    }
                    break;
                case STRING:
                    strarg = *((std::string *)(*it)[tmp]);
                    break;
                case SWITCH_ARGUMENT:
                    args[tmp] = (*it)[tmp];
                    break;
            }
            ++tmp;
        }

        result_change_commands_push_back(it, args, its_result_change_command, tmp);
    }

    void byte_code_interpreter::clear_visibility_area() {
        for (size_t i = 0; i < vid.back(); i++)
            pop_back();
        vid.pop_back();
    }

    byte_code_basic::byte_code_t byte_code_interpreter::search_function(const byte_code_t::const_iterator it, const size_t count_commands, const INT_T function_pointer) {
        auto text_it           = text.cbegin() + function_pointer;
        intmax_t investigation = 0;
        while (text_it < text.end()) {
            if (*text_it == _function)
                ++investigation;
            else if (*text_it == _end_function) {
                --investigation;
                if (!investigation) {
                    inc_iterator(text_it, text.end());
                    break;
                }
                error_interpreter(investigation < 0, 7);
            }
            inc_iterator(text_it, text.end());
        }
        return byte_code_t(text.cbegin() + function_pointer, text_it);
    }

    byte_code_basic::ARR_INT_T byte_code_interpreter::interpreter(const graph_t &arg_graph, bool is_main_interpreter, bool *result_is_changed) {
        if (setjmp(buf))
            return {};

        this->is_main_interpreter = is_main_interpreter;

        while (!vid.empty())
            clear_visibility_area();
        stack_pointers.clear();
        stack_types.clear();
        if_stack.clear();
        while_adresses.clear();

        size_t count_commands = 0;

        stack_pointers.push_back((INT_T)default_array(arg_graph.size()));

        stack_types.push_back(INTARR);
        vid            = std::vector<size_t>(1, 1);
        inc_adress     = 0;
        vec_inc_adress = std::vector<size_t>(1, 0);

        result_change_commands.clear();

        array_of_args_t args = {0};
        std::string strarg;

        for (auto it = text.cbegin(); it < text.cend(); it++) {
            ++count_commands;

            get_command_args(count_commands, it, args, strarg);

            switch (it->get_command()) {
                case alloc_int:
                    push_back(stack_pointers[inc_adress + args[0]], INT);
                    break;
                case alloc_int_const:
                    push_back(args[0], INT);
                    break;
                case alloc_arr:
                    error_interpreter(!args[0], 4);
                    push_back((INT_T) new ARR_INT_T(args[0], 0), INTARR);
                    break;
                case alloc_arr_const:
                    error_interpreter(!args[0], 4);
                    push_back((INT_T) new ARR_INT_T(args[0], 0), INTARR);
                    break;
                case alloc_arr_p_const:
                    error_interpreter(!stack_pointers[inc_adress + args[0]], 4);
                    push_back((INT_T) new ARR_INT_T(stack_pointers[inc_adress + args[0]], 0), INTARR);
                    break;
                case alloc_arr_copy:
                    push_back((INT_T) new ARR_INT_T((*(ARR_INT_T *)stack_pointers[inc_adress + args[0]])), INTARR);
                    break;
                case graph:
                    stack_pointers[inc_adress + args[2]] = arg_graph[args[0]][args[1]];
                    break;
                case graph_p:
                    error_interpreter(stack_pointers[inc_adress + args[0]] >= arg_graph.size() || stack_pointers[inc_adress + args[1]] >= arg_graph.size(), 17);
                    stack_pointers[inc_adress + args[2]] = arg_graph[stack_pointers[inc_adress + args[0]]][stack_pointers[inc_adress + args[1]]];
                    break;
                case size:
                    stack_pointers[inc_adress + args[0]] = arg_graph.size();
                    break;
                case move:
                    stack_pointers[inc_adress + args[0]] = stack_pointers[inc_adress + args[1]];
                    break;
                case move_arr:
                    error_interpreter(args[0] == args[1], 17);
                    delete ((ARR_INT_T *)stack_pointers[inc_adress + args[0]]);
                    stack_pointers[inc_adress + args[0]] = (INT_T) new ARR_INT_T(*((ARR_INT_T *)stack_pointers[inc_adress + args[1]]));
                    break;
                case set:
                    (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]]))[args[1]] = stack_pointers[inc_adress + args[2]];
                    break;
                case get:
                    stack_pointers[inc_adress + args[2]] = (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]]))[args[1]];
                    break;
                case set_p:
                    (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]]))[stack_pointers[inc_adress + args[1]]] = stack_pointers[inc_adress + args[2]];
                    break;
                case get_p:
                    stack_pointers[inc_adress + args[2]] = (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]]))[stack_pointers[inc_adress + args[1]]];
                    break;
                case move_const:
                    stack_pointers[inc_adress + args[0]] = args[1];
                    break;
                case set_const:
                    error_interpreter(args[1] >= ((ARR_INT_T *)stack_pointers[inc_adress + args[0]])->size(), 16);
                    (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]]))[args[1]] = args[2];
                    break;
                case set_p_const:  // =
                    (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]]))[stack_pointers[inc_adress + args[1]]] = args[2];
                    break;
                case arr_size:
                    stack_pointers[inc_adress + args[1]] = (*((ARR_INT_T *)stack_pointers[inc_adress + args[0]])).size();
                    break;
                case add:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] + stack_pointers[inc_adress + args[1]];
                    break;
                case sub:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] - stack_pointers[inc_adress + args[1]];
                    break;
                case mul:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] * stack_pointers[inc_adress + args[1]];
                    break;
                case div:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] / stack_pointers[inc_adress + args[1]];
                    break;
                case remainder:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] % stack_pointers[inc_adress + args[1]];
                    break;
                case add_const:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] + args[1];
                    break;
                case sub_const:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] - args[1];
                    break;
                case mul_const:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] * args[1];
                    break;
                case div_const:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] / args[1];
                    break;
                case remainder_const:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] % args[1];
                    break;
                case inc:
                    ++stack_pointers[inc_adress + args[0]];
                    break;
                case dec:
                    --stack_pointers[inc_adress + args[0]];
                    break;
                case _not:
                    stack_pointers[inc_adress + args[1]] = !stack_pointers[inc_adress + args[0]];
                    break;
                case _or:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] || stack_pointers[inc_adress + args[1]];
                    break;
                case _and:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] && stack_pointers[inc_adress + args[1]];
                    break;
                case equal:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] == stack_pointers[inc_adress + args[1]];
                    break;
                case more:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] > stack_pointers[inc_adress + args[1]];
                    break;
                case less:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] < stack_pointers[inc_adress + args[1]];
                    break;
                case _or_const:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] || args[1];
                    break;
                case _and_const:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] && args[1];
                    break;
                case equal_const:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] == args[1];
                    break;
                case more_const:
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] > args[1];
                    break;
                case less_const:  // =
                    stack_pointers[inc_adress + args[2]] = stack_pointers[inc_adress + args[0]] < args[1];
                    break;
                case _if:
                    if_stack.push_back(!stack_pointers[inc_adress + args[0]]);
                    if (!stack_pointers[inc_adress + args[0]]) {
                        ++it;
                        bool investigation_less_null = false;
                        it                           = if_goto_else_or_end_if(it, text.end(), investigation_less_null, if_stack);
                        error_interpreter(investigation_less_null, 5);
                    } else vid.push_back(0);
                    break;
                case _else:
                    error_interpreter(if_stack.empty(), 6);
                    if (!if_stack.back()) {
                        ++it;
                        bool investigation_less_null = false;
                        it                           = investigation(it, text.end(), investigation_less_null, {_if}, {_end_if}, [](intmax_t investigation) { return investigation <= 0; });
                        error_interpreter(investigation_less_null, 5);
                    }
                    if_stack.pop_back();
                    break;
                case _end_if:
                    error_interpreter(vid.size() <= 1, 7);
                    clear_visibility_area();
                    break;
                case _while:
                    if (!stack_pointers[inc_adress + args[0]]) {
                        ++it;
                        bool investigation_less_null = false;
                        it                           = investigation(it, text.end(), investigation_less_null, {_while}, {_end_while}, [](intmax_t investigation) { return investigation < 0; });
                        error_interpreter(investigation_less_null, 5);
                    } else {
                        while_adresses.push_back(it - 1);
                        vid.push_back(0);
                    }
                    break;
                case _continue:
                case _end_while:
                    error_interpreter(while_adresses.empty(), 8);
                    error_interpreter(vid.size() <= 1, 7);
                    clear_visibility_area();
                    it = while_adresses.back();
                    while_adresses.pop_back();
                    break;
                case _module:
                    break;
                case _end_module:
                    break;
                case exit:
                    return end_interpreting(result_is_changed);
                    break;
                case _break: {
                    error_interpreter(while_adresses.empty(), 9);
                    error_interpreter(vid.size() <= 1, 7);
                    ++it;
                    bool investigation_less_null = false;
                    it                           = investigation(it, text.end(), investigation_less_null, {_while}, {_end_while}, [](intmax_t investigation) { return investigation < 0; });
                    error_interpreter(investigation_less_null, 5);
                    clear_visibility_area();
                    while_adresses.pop_back();
                } break;
                case puts:
                    output << strarg << std::flush;
                    break;
                case print:
                    switch (stack_types[inc_adress + args[0]]) {
                        case INT:
                        case FUNCTION:
                            output << stack_pointers[inc_adress + args[0]];
                            break;
                        case LINK_INT:
                        case LINK_FUNCTION:
                            output << stack_pointers[inc_adress + stack_pointers[inc_adress + args[0]]];
                            break;
                        case LINK_INTARR:
                        case INTARR: {
                            auto &arr = *((ARR_INT_T *)stack_pointers[inc_adress + args[0]]);
                            output << "{ ";
                            for (auto it = arr.begin(); it < arr.end(); it++) {
                                output << *it << (it < arr.end() - 1 ? ", " : "");
                            }
                            output << " }";
                        } break;
                        default:
                            error_interpreter(1, 10);
                            break;
                    }
                    output << std::flush;
                    break;
                case scan: {
                    // clock_t begin1 = clock();
                    /*std::cin >> stack_pointers[inc_adress + args[0]];*/
                    // clock_t end2 = clock();
                    // begin += end2 - begin1;
                } break;
                case _function: {
                    const auto adress_function = std::distance(text.cbegin(), it);
                    if (find(stack_pointers, stack_types, adress_function, FUNCTION) == EOF) {
                        push_back(adress_function, FUNCTION);

                        inc_iterator(it, text.end());
                        intmax_t investigation = 1;
                        while (it < text.end()) {
                            if (*it == _function)
                                ++investigation;
                            else if (*it == _end_function) {
                                --investigation;
                                if (!investigation) {
                                    inc_iterator(it, text.end());
                                    break;
                                }
                                error_interpreter(investigation < 0, 5);
                            }
                            inc_iterator(it, text.end());
                        }
                        --it;

                    } else {
                        error_interpreter(1, 11);
                    }
                } break;
                case _return:
                    error_interpreter(func_adresses.empty(), 12);
                case _end_function:
                    error_interpreter(func_adresses.empty(), 13);
                    error_interpreter(vid.size() <= 1, 7);
                    clear_visibility_area();
                    it = func_adresses.back();
                    func_adresses.pop_back();
                    inc_adress -= vec_inc_adress.back();
                    vec_inc_adress.pop_back();
                    break;
                case call: {
                    INT_T call_args[MAX_COUNT_ARG] = {0};

                    func_adresses.push_back(it);
                    it = stack_pointers[inc_adress + args[0]] + text.cbegin();

                    get_command_args(count_commands, it, call_args, strarg);

                    error_interpreter(args[1] != call_args[0], 14);
                    for (size_t i = 1; i < 1 + args[1]; i++) {
                        switch (call_args[i]) {
                            case LINK_INTARR:
                            case INTARR:
                                error_interpreter(args[i + 1] >= stack_pointers.size(), 2);
                                error_interpreter(stack_types[inc_adress + args[i + 1]] != INTARR && stack_types[inc_adress + args[i + 1]] != LINK_INTARR, 3);
                                if (stack_types[inc_adress + args[i + 1]] == LINK_INTARR) {
                                    args[i + 1] = stack_pointers[inc_adress + args[i + 1]];
                                }
                                break;
                            case LINK_FUNCTION:
                            case FUNCTION:
                                error_interpreter(args[i + 1] >= stack_pointers.size(), 2);
                                error_interpreter(stack_types[inc_adress + args[i + 1]] != FUNCTION && stack_types[inc_adress + args[i + 1]] != LINK_FUNCTION, 3);
                                if (stack_types[inc_adress + args[i + 1]] == LINK_FUNCTION) {
                                    args[i + 1] = stack_pointers[inc_adress + args[i + 1]];
                                }
                                break;
                            case LINK_INT:
                                error_interpreter(args[i + 1] >= stack_pointers.size(), 2);
                                error_interpreter(stack_types[inc_adress + args[i + 1]] != INT && stack_types[inc_adress + args[i + 1]] != LINK_INT, 3);
                                if (stack_types[inc_adress + args[i + 1]] == LINK_INT) {
                                    args[i + 1] = stack_pointers[inc_adress + args[i + 1]];
                                }
                                break;
                        }
                    }

                    vec_inc_adress.push_back(vid.back());
                    inc_adress += vid.back();
                    vid.push_back(0);

                    for (size_t i = 1; i < 1 + args[1]; i++) {
                        switch (call_args[i]) {
                            case INT:
                                push_back(args[i + 1], INT);
                                break;
                            case INTARR:
                                push_back(args[i + 1], INTARR);
                                break;
                            case FUNCTION:
                                push_back(args[i + 1], FUNCTION);
                                break;
                            case LINK_INT:
                                push_back(args[i + 1] - vec_inc_adress.back(), LINK_INT);
                                break;
                            case LINK_INTARR:
                                push_back(args[i + 1] - vec_inc_adress.back(), LINK_INTARR);
                                break;
                            case LINK_FUNCTION:
                                push_back(args[i + 1] - vec_inc_adress.back(), LINK_FUNCTION);
                                break;
                        }
                    }
                } break;
                case sort: {
                    auto &arr = *(ARR_INT_T *)stack_pointers[inc_adress + args[0]];
                    std::sort(arr.begin(), arr.end(), predicat_function(it, count_commands, stack_pointers[inc_adress + args[1]], arg_graph));
                } break;
                case reverse: {
                    auto &arr = *(ARR_INT_T *)stack_pointers[inc_adress + args[0]];
                    std::reverse(arr.begin(), arr.end());
                } break;
                case min_element: {
                    auto &arr                            = *(ARR_INT_T *)stack_pointers[inc_adress + args[0]];
                    stack_pointers[inc_adress + args[2]] = *std::min_element(arr.begin(), arr.end(), predicat_function(it, count_commands, stack_pointers[inc_adress + args[1]], arg_graph));
                } break;
                case max_element: {
                    auto &arr                            = *(ARR_INT_T *)stack_pointers[inc_adress + args[0]];
                    stack_pointers[inc_adress + args[2]] = *std::max_element(arr.begin(), arr.end(), predicat_function(it, count_commands, stack_pointers[inc_adress + args[1]], arg_graph));
                } break;
                case next_permutation: {
                    auto &arr                            = *(ARR_INT_T *)stack_pointers[inc_adress + args[0]];
                    stack_pointers[inc_adress + args[1]] = std::next_permutation(arr.begin(), arr.end());
                } break;
                default:
                    error_interpreter(1, 15);
                    ++it;
                    break;
            }
        }

        return end_interpreting(result_is_changed);
    }

    byte_code_interpreter::~byte_code_interpreter() {
        while (!vid.empty())
            clear_visibility_area();
    }

    std::string byte_code_interpreter::get_output() {
        return output.str();
    }

    void byte_code_text_interpreter::_error_return(const char *ysl, size_t LINE, size_t index_str, const std::vector<std::string> &vec_of_args, size_t line, std::stringstream &output) {
        constexpr const char *const str[] = {
            "",
            "??????????? ????????",
            "?? ??????? ??????????",
            "??????????? ???????"};
        output << "\nERROR IN TEXT INTERPRETER:"
                  "\n \""
               << ysl << "\""
                         "\n message: \""
               << str[index_str] << "\""
                                    "\n source file name: \""
               << __FILE__ << "\""
                              "\n source code line: "
               << LINE << "\n\n text line: \"";
        for (auto &i : vec_of_args) {
            output << i << " ";
        }

        output << "\""
                  "\n executable code line: "
               << line << "\n\n";
    }

    bool byte_code_text_interpreter::is_digit(const std::string &text) {
        for (auto i = text.begin(); i < text.end(); i++)
            if (!isdigit(*i))
                return false;
        return true;
    }

    bool byte_code_text_interpreter::is_pointer(const std::string &text) {
        if (text.front() != '&' || text.size() == 1)
            return false;
        for (auto i = text.begin() + 1; i < text.end(); i++)
            if (!isdigit(*i))
                return false;
        return true;
    }

    bool byte_code_text_interpreter::is_string(const std::string &text) {
        return text.front() == text.back() && text.front() == '\"';
    }

    std::vector<std::string> byte_code_text_interpreter::get_line(const std::string &text, size_t &index) {
        const size_t new_index = std::distance(text.begin() + index, std::find(text.begin() + index, text.end(), '\n'));
        std::vector<std::string> vec_of_args(1, NULL_STRING);

        bool flag_string_symb = false;

        for (size_t i = index; i < index + new_index; i++) {

            if (text[i] == '\"')
                flag_string_symb = !flag_string_symb;

            if (text[i] == ';')
                vec_of_args.push_back({';'});
            else if (!flag_string_symb && isspace(text[i]) && vec_of_args.back() != NULL_STRING)
                vec_of_args.push_back(NULL_STRING);
            else if (flag_string_symb or !isspace(text[i]))
                vec_of_args.back().push_back(text[i]);
        }
        index += new_index;
        return vec_of_args;
    }

    bool byte_code_text_interpreter::is_type_argument(const intmax_t &digit) {
        return digit >= INT && digit <= LINK_FUNCTION;
    }

    byte_code_basic::byte_code_t byte_code_text_interpreter::text_interpreter(const std::string &text) {
        byte_code_t byte_text;
        size_t line = 0;
        for (size_t i = 0; i < text.size(); i++, line++) {
            std::vector<std::string> vec_of_args = get_line(text, i);
            if (vec_of_args.front() != NULL_STRING && vec_of_args.front() != ";") {
                bool is_command = false;
                for (size_t j = 0; j < COUNT_COMMANDS; j++) {
                    if (is_command = vec_of_args.front() == commands[j].name) {
                        byte_text.push_back(byte_code_command(j));
                        size_t k = 1;
                        intmax_t digit;
                        for (auto &it_type : commands[j].type) {
                            error_return(k >= vec_of_args.size(), 2);
                            switch (it_type) {
                                case INT:
                                case SIZE_T:
                                    error_return(!is_digit(vec_of_args[k]), 1);
                                    digit                   = std::stoull(vec_of_args[k]);
                                    byte_text.back()[k - 1] = digit;
                                    break;
                                case SWITCH_ARGUMENT:
                                case FUNCTION:
                                case INTP:
                                case INTARR:
                                    error_return(!is_pointer(vec_of_args[k]), 1);
                                    vec_of_args[k].erase(vec_of_args[k].begin());
                                    digit                   = std::stoull(vec_of_args[k]);
                                    byte_text.back()[k - 1] = digit;
                                    break;
                                case STRING:
                                    error_return(!is_string(vec_of_args[k]), 1);
                                    vec_of_args[k].erase(vec_of_args[k].end() - 1);
                                    vec_of_args[k].erase(vec_of_args[k].begin());
                                    {
                                        std::string strarg;
                                        for (auto it = vec_of_args[k].begin(); it < vec_of_args[k].end(); it++) {
                                            if (*it == '\\') {
                                                switch (*++it) {
                                                    case 'n':
                                                        strarg.push_back('\n');
                                                        break;
                                                    case 't':
                                                        strarg.push_back('\t');
                                                        break;
                                                    case 'a':
                                                        strarg.push_back('\a');
                                                        break;
                                                    case 'f':
                                                        strarg.push_back('\f');
                                                        break;
                                                    case 'r':
                                                        strarg.push_back('\r');
                                                        break;
                                                    case 'v':
                                                        strarg.push_back('\v');
                                                        break;
                                                    case '\\':
                                                        strarg.push_back('\\');
                                                        break;
                                                }
                                            } else strarg.push_back(*it);
                                        }
                                        strarg.push_back(NULL);
                                        byte_text.back()[k - 1] = INT_T(new std::string(strarg));
                                    }
                                    break;
                                case ENUM_ARGUMENTS:
                                    error_return(!is_digit(vec_of_args[k]), 1);
                                    digit = std::stoull(vec_of_args[k]);
                                    error_return(k + digit > vec_of_args.size(), 1);
                                    error_return(digit > MAX_COUNT_ARG, 2);
                                    byte_text.back()[k - 1] = digit;
                                    byte_text.back().reserve(digit);
                                    {
                                        const auto count_args = digit;
                                        for (const size_t i = ++k; k < i + count_args; k++) {
                                            error_return(!is_digit(vec_of_args[k]), 1);
                                            digit = std::stoull(vec_of_args[k]);
                                            if (j == _function)
                                                error_return(!is_type_argument(digit), 2);

                                            byte_text.back()[k - 1] = digit;
                                        }
                                    }
                                    --k;

                                    break;
                            }
                            ++k;
                        }
                        error_return(k < vec_of_args.size() && vec_of_args[k] != NULL_STRING && vec_of_args[k].front() != ';', 1);
                        break;
                    }
                }
                error_return(!is_command, 3);
            }
        }

        return byte_text;
    }

    byte_code_basic::ARR_INT_T byte_code_text_interpreter::interpreter(const std::string &text, const graph_t &graph, bool is_main_interpreter, bool allow_error_output, bool *result_is_changed) {
        output.clear();
        auto byte_text = text_interpreter(text);
        byte_code_interpreter bc(byte_text, allow_error_output);
        auto result = bc.interpreter(graph, is_main_interpreter, result_is_changed);
        output << bc.get_output();
        return result;
    }

    std::string byte_code_text_interpreter::get_output() {
        return output.str();
    }

    std::string to_esc_string(const byte_code_basic::byte_code_t &text) {
        std::stringstream out;
        byte_code_basic::array_of_args_t args = {0};
        std::string strarg;
        std::string tabs_count;
        size_t count_commands = 0;

        out << "\n";

        for (auto it = text.cbegin(); it < text.cend(); count_commands++, it++) {
            if (byte_code_basic::commands + it->get_command() < std::end(byte_code_basic::commands)) {
                if (it->get_command() != byte_code_basic::_else && byte_code_basic::is_begin_module(it->get_command())) {
                    out << "\n";
                }
                if (it->get_command() == byte_code_basic::_else || byte_code_basic::is_end_module(it->get_command())) {
                    tabs_count.resize(tabs_count.size() - 4);
                }

                out << "    " << tabs_count << "\033[34;1m";
                out << it->to_esc_string();
                out << "\033[0m ";

                if (byte_code_basic::is_begin_module(it->get_command())) {
                    tabs_count += "    ";
                }
                if (byte_code_basic::is_end_module(it->get_command())) {
                    out << "\n";
                }
            } else {
                out << "\033[31mERROR COMMAND : " << it->get_command() << "\033[0m";
            }

            out << "\n";
        }

        return out.str();
    }

    std::string to_string(const byte_code_basic::byte_code_t &text) {
        std::stringstream out;
        byte_code_basic::array_of_args_t args = {0};
        std::string strarg;
        std::string tabs_count;
        size_t count_commands = 0;

        out << "\n";

        for (auto it = text.cbegin(); it < text.cend(); count_commands++, it++) {
            if (byte_code_basic::commands + it->get_command() < std::end(byte_code_basic::commands)) {
                if (it->get_command() != byte_code_basic::_else && byte_code_basic::is_begin_module(it->get_command())) {
                    out << "\n";
                }
                if (it->get_command() == byte_code_basic::_else || byte_code_basic::is_end_module(it->get_command())) {
                    tabs_count.resize(tabs_count.size() - 4);
                }

                out << "    " << tabs_count;
                out << it->to_string();

                if (byte_code_basic::is_begin_module(it->get_command())) {
                    tabs_count += "    ";
                }
                if (byte_code_basic::is_end_module(it->get_command())) {
                    out << "\n";
                }
            } else {
                out << "ERROR COMMAND : " << it->get_command() << "";
            }

            out << "\n";
        }

        return out.str();
    }

}  // namespace byte_code
