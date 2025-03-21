#ifndef TOMATIK_CLAH_H
#define TOMATIK_CLAH_H

#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define GENERATE_INTO_FUNCTION_DECL(type, type_constanta) \
    type &to_##type(bool *is_error = nullptr);

#define NO_ERROR ""

namespace tomatik {

    namespace cla {

        enum type_t {
            INT,
            FLOAT,
            STRING,
            KEY,
            ARRAY_STRING
        };

        struct arg_t {
            type_t type;
            void *pointer;
            bool is_def = false;

            using int_type          = intmax_t;
            using float_type        = double;
            using string_type       = std::string;
            using key_type          = bool;
            using array_string_type = std::vector<std::string>;

            GENERATE_INTO_FUNCTION_DECL(int_type, INT);
            GENERATE_INTO_FUNCTION_DECL(float_type, FLOAT);
            GENERATE_INTO_FUNCTION_DECL(string_type, STRING);
            GENERATE_INTO_FUNCTION_DECL(key_type, KEY);
            GENERATE_INTO_FUNCTION_DECL(array_string_type, ARRAY_STRING);
        };

        using map_cla = std::map<std::string, arg_t>;

        static bool is_int_number(const std::string &s) {
            return !s.empty() && (s.find_first_not_of("0123456789") == s.npos);
        }

        arg_t::array_string_type argv_to_array_string(int argc, char *argv[]);

        // return NO_ERROR if no error else return error-string
        std::string command_line_argument_handler(int argc, char *argv[], tomatik::cla::map_cla &map);

        // return NO_ERROR if no error else return error-string
        std::string command_line_argument_handler(const arg_t::array_string_type &argv, tomatik::cla::map_cla &map);
    }  // namespace cla

}  // namespace tomatik

#endif  // TOMATIK_CLAH_H
