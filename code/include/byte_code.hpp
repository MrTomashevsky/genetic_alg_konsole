#ifndef BYTE_CODE_HPP_INCLUDED
#define BYTE_CODE_HPP_INCLUDED

#include <algorithm>
#include <csetjmp>
#include <cstdint>
#include <iostream>

#include <fstream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace byte_code {

#define error_return(ysl, index_str)                                         \
    if ((ysl)) {                                                             \
        _error_return(#ysl, __LINE__, index_str, vec_of_args, line, output); \
        return {};                                                           \
    }
#define error_interpreter(ysl, index_str) \
    if (allow_error_output && (ysl)) _error_interpreter(#ysl, __LINE__, index_str, it, count_commands, buf, is_main_interpreter, error_line, error_str_index, output)
#define inc_iterator(it, end) \
    if (it < end) ++it
#define NULL_STRING ""

    class byte_code_basic {
      public:
        constexpr static size_t
            MAX_NAME       = 20,
            COUNT_COMMANDS = 64,
            MAX_COUNT_ARG  = UINT8_MAX;

        using INT_T           = intptr_t;
        using ARR_INT_T       = std::vector<INT_T>;
        using graph_t         = std::vector<ARR_INT_T>;
        using array_of_args_t = INT_T[MAX_COUNT_ARG];

        constexpr static INT_T
            INT_T_MAX = INTPTR_MAX,
            INT_T_MIN = INTPTR_MIN;

        enum type_t : intmax_t {
            INT,
            INTARR,
            FUNCTION,
            LINK_INT,
            LINK_INTARR,
            LINK_FUNCTION,
            INTP,
            STRING,
            ENUM_ARGUMENTS,
            SWITCH_ARGUMENT,
            SIZE_T
        };

        enum commands_t : uint8_t {
            alloc_int,
            alloc_int_const,
            alloc_arr,
            alloc_arr_const,
            alloc_arr_p_const,
            alloc_arr_copy,

            graph,
            graph_p,
            size,

            move,
            move_arr,
            set,
            get,
            set_p,
            get_p,
            move_const,
            set_const,
            set_p_const,
            arr_size,

            add,
            sub,
            mul,
            div,
            remainder,
            add_const,
            sub_const,
            mul_const,
            div_const,
            remainder_const,
            inc,
            dec,

            _not,
            _or,
            _and,
            equal,
            more,
            less,
            _or_const,
            _and_const,
            equal_const,
            more_const,
            less_const,

            _if,
            _else,
            _end_if,
            _while,
            _end_while,
            _module,
            _end_module,
            exit,
            _break,
            _continue,
            _return,

            puts,
            print,
            scan,

            _function,
            _end_function,
            call,

            sort,
            reverse,
            min_element,
            max_element,
            next_permutation
        };

        class byte_code_command {
            uint8_t command;
            std::vector<INT_T> arr;

            inline size_t default_size() const {
                return commands[command].type.size();
            }

          public:
            byte_code_command(uint8_t command) : command(command), arr(default_size(), 0) {}
            byte_code_command(uint8_t command, std::initializer_list<INT_T> lst) : command(command), arr(lst.size() ? lst.size() : default_size(), 0) {
                if (arr.size()) std::copy(lst.begin(), lst.begin() + (lst.size() ? lst.size() : default_size()), arr.begin());
            }
            byte_code_command(const byte_code_command &obj) noexcept : command(obj.command), arr(obj.arr.size(), 0) {
                if (arr.size()) std::copy(obj.arr.begin(), obj.arr.begin() + obj.arr.size(), arr.begin());
            }
            void reserve(size_t count);
            INT_T &operator[](size_t i);
            const INT_T &operator[](size_t i) const;
            uint16_t get_length() const;
            uint8_t get_command() const;
            bool operator==(commands_t c) const;
            bool operator==(const byte_code_command &c) const;
            std::string to_esc_string() const;
            std::string to_string() const;
        };

        using byte_code_t = std::vector<byte_code_command>;

        static inline const struct command_data {
            char name[MAX_NAME] = {0};
            std::vector<type_t> type;
        } commands[COUNT_COMMANDS] = {
            {{"alloc_int"}, {INTP}},
            {{"alloc_int_const"}, {INT}},

            {{"alloc_arr"}, {ENUM_ARGUMENTS}},
            {{"alloc_arr_const"}, {SIZE_T}},
            {{"alloc_arr_p_const"}, {INTP}},
            {{"alloc_arr_copy"}, {INTARR}},

            {{"graph"}, {INT, INT, INTP}},
            {{"graph_p"}, {INTP, INTP, INTP}},
            {{"size"}, {INTP}},

            {{"move"}, {INTP, INTP}},
            {{"move_arr"}, {INTARR, INTARR}},
            {{"set"}, {INTARR, INT, INTP}},
            {{"get"}, {INTARR, INT, INTP}},
            {{"set_p"}, {INTARR, INTP, INTP}},
            {{"get_p"}, {INTARR, INTP, INTP}},
            {{"move_const"}, {INTP, INT}},
            {{"set_const"}, {INTARR, INT, INT}},
            {{"set_p_const"}, {INTARR, INTP, INT}},
            {{"arr_size"}, {INTARR, INTP}},

            {{"add"}, {INTP, INTP, INTP}},
            {{"sub"}, {INTP, INTP, INTP}},
            {{"mul"}, {INTP, INTP, INTP}},
            {{"div"}, {INTP, INTP, INTP}},
            {{"remainder"}, {INTP, INT, INTP}},
            {{"add_const"}, {INTP, INT, INTP}},
            {{"sub_const"}, {INTP, INT, INTP}},
            {{"mul_const"}, {INTP, INT, INTP}},
            {{"div_const"}, {INTP, INT, INTP}},
            {{"remainder_const"}, {INTP, INT, INTP}},
            {{"inc"}, {INTP}},
            {{"dec"}, {INTP}},

            {{"not"}, {INTP, INTP}},
            {{"or"}, {INTP, INTP, INTP}},
            {{"and"}, {INTP, INTP, INTP}},
            {{"equal"}, {INTP, INTP, INTP}},
            {{"more"}, {INTP, INTP, INTP}},
            {{"less"}, {INTP, INTP, INTP}},
            {{"or_const"}, {INTP, INT, INTP}},
            {{"and_const"}, {INTP, INT, INTP}},
            {{"equal_const"}, {INTP, INT, INTP}},
            {{"more_const"}, {INTP, INT, INTP}},
            {{"less_const"}, {INTP, INT, INTP}},

            {{"if"}, {INTP}},
            {{"else"}, {}},
            {{"end_if"}, {}},
            {{"while"}, {INTP}},
            {{"end_while"}, {}},
            {{"module"}, {}},
            {{"end_module"}, {}},

            {{"exit"}, {}},
            {{"break"}, {}},
            {{"continue"}, {}},
            {{"return"}, {}},

            {{"puts"}, {STRING}},
            {{"print"}, {SWITCH_ARGUMENT}},
            {{"scan"}, {INTP}},

            {{"function"}, {ENUM_ARGUMENTS}},
            {{"end_function"}, {}},
            {{"call"}, {FUNCTION, ENUM_ARGUMENTS}},

            {{"sort"}, {INTARR, FUNCTION}},
            {{"reverse"}, {INTARR}},
            {{"min_element"}, {INTARR, FUNCTION, INTP}},
            {{"max_element"}, {INTARR, FUNCTION, INTP}},
            {{"next_permutation"}, {INTARR, INTP}},
        };

        static ARR_INT_T *default_array(size_t size);
        static bool is_ios_command(uint8_t it);
        static bool is_begin_module(uint8_t it);
        static bool is_end_module(uint8_t it);
        static bool is_alloc_command(uint8_t it);
        static bool is_arr_change_command(uint8_t it);
        static uint8_t get_pair_command(uint8_t first_command_code);
        static byte_code_t::const_iterator if_goto_else_or_end_if(byte_code_t::const_iterator it, byte_code_t::const_iterator end, bool &error_flag, std::vector<bool> &if_stack);
        static byte_code_t::const_iterator investigation(byte_code_t::const_iterator it, byte_code_t::const_iterator end, bool &error_flag, const std::initializer_list<uint8_t> inc_commands, const std::initializer_list<uint8_t> dec_commands, bool (*func)(intmax_t));
    };

    class byte_code_interpreter final : public byte_code_basic {
        std::stringstream output;

        void result_change_commands_push_back(const byte_code_t::const_iterator &it, array_of_args_t args, bool &its_result_change_command, const size_t &tmp);

        static void _error_interpreter(const char *ysl, size_t LINE, size_t index_str, const byte_code_t::const_iterator &it, size_t count_commands, jmp_buf &buf, bool is_main_interpreter, size_t &error_line, size_t &error_str_index, std::stringstream &output);
        void pop_back();
        void push_back(INT_T value, type_t type);
        ARR_INT_T end_interpreting(bool *result_is_changed);
        static size_t find(const std::vector<INT_T> &stack_pointers, const std::vector<type_t> &stack_types, INT_T value, type_t type);
        void get_command_args(const size_t &count_commands, byte_code_t::const_iterator &it, array_of_args_t args, std::string &strarg);
        void clear_visibility_area();
        byte_code_t search_function(const byte_code_t::const_iterator it, const size_t count_commands, const INT_T function_pointer);

        auto predicat_function(const byte_code_t::const_iterator it, const size_t count_commands, const INT_T function_pointer, graph_t graph) {
            auto function = search_function(it, count_commands, function_pointer);
            return [&graph, this, function, it, count_commands](INT_T &i1, INT_T &i2) {
                byte_code_t tmp_function = function;
                auto tmp_function_it     = std::back_inserter(tmp_function);

                *++tmp_function_it = byte_code_command(alloc_int_const, {INT_T(0)});
                *++tmp_function_it = byte_code_command(alloc_int_const, {i1});
                *++tmp_function_it = byte_code_command(alloc_int_const, {i2});
                *++tmp_function_it = byte_code_command(call, {INT_T(1), INT_T(3), INT_T(3), INT_T(4), INT_T(2)});

                byte_code_interpreter obj(tmp_function, allow_error_output);
                obj.interpreter(graph, false, nullptr);

                error_interpreter(obj.error_line, obj.error_str_index);

                return obj.stack_pointers[2];
            };
        }

      private:
        const byte_code_t &text;

        std::vector<INT_T> stack_pointers;
        std::vector<type_t> stack_types;
        std::vector<size_t> vid;

        std::vector<bool> if_stack;
        std::vector<byte_code_t::const_iterator> while_adresses;
        std::vector<byte_code_t::const_iterator> func_adresses;

        size_t inc_adress = 0;
        std::vector<size_t> vec_inc_adress;

        bool is_main_interpreter = true;
        bool allow_error_output  = true;
        size_t error_line        = 0;
        size_t error_str_index   = 0;

        jmp_buf buf;
        std::vector<uint8_t> result_change_commands;

      public:
        byte_code_interpreter(const byte_code_t &text, bool allow_error_output) : text(text), allow_error_output(allow_error_output) {}
        ARR_INT_T interpreter(const graph_t &arg_graph, bool is_main_interpreter, bool *result_is_changed);
        ~byte_code_interpreter();
        std::string get_output();
    };

    class byte_code_text_interpreter final : public byte_code_basic {
        std::stringstream output;

        static void _error_return(const char *ysl, size_t LINE, size_t index_str, const std::vector<std::string> &vec_of_args, size_t line, std::stringstream &output);
        static bool is_digit(const std::string &text);
        static bool is_pointer(const std::string &text);
        static bool is_string(const std::string &text);
        static std::vector<std::string> get_line(const std::string &text, size_t &index);
        static bool is_type_argument(const intmax_t &digit);

      public:
        byte_code_t text_interpreter(const std::string &text);
        ARR_INT_T interpreter(const std::string &text, const graph_t &graph, bool is_main_interpreter, bool allow_error_output, bool *result_is_changed);
        std::string get_output();
    };

    constexpr byte_code_basic::INT_T INF = byte_code_basic::INT_T_MAX;

    std::string to_esc_string(const byte_code_basic::byte_code_t &text);
    std::string to_string(const byte_code_basic::byte_code_t &text);
}  // namespace byte_code

#endif  // !BYTE_CODE_HPP_INCLUDED
