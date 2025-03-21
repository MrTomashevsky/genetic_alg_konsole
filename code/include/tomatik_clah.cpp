#include "tomatik_clah.hpp"

#define GENERATE_INTO_FUNCTION_IMPL(TYPE, type_constanta) \
    tomatik::cla::arg_t::                                 \
        TYPE &tomatik::cla::arg_t::to_##TYPE              \
                                                          \
        (bool *is_error) {                                \
        if (is_error)                                     \
            *is_error = this->type == type_constanta;     \
        return *((                                        \
            TYPE *)pointer);                              \
    }

GENERATE_INTO_FUNCTION_IMPL(int_type, INT);
GENERATE_INTO_FUNCTION_IMPL(float_type, FLOAT);
GENERATE_INTO_FUNCTION_IMPL(string_type, STRING);
GENERATE_INTO_FUNCTION_IMPL(key_type, KEY);
GENERATE_INTO_FUNCTION_IMPL(array_string_type, ARRAY_STRING);

#define assert_command_line_argument_handler(ysl, error) \
    if (ysl) {                                           \
        std::stringstream ss;                            \
        ss << "error : \"" << error << "\"";             \
        return ss.str();                                 \
    }

tomatik::cla::arg_t::array_string_type tomatik::cla::argv_to_array_string(int argc, char *argv[]) {
    arg_t::array_string_type result_value(argc - 1, "");
    for (size_t i = 1; i < argc; ++i) {
        result_value[i - 1] = argv[i];
    }
    return result_value;
}

std::string tomatik::cla::command_line_argument_handler(const arg_t::array_string_type &argv, map_cla &map) {
    bool is_name_key = true;
    auto it          = map.begin();

    for (size_t i = 0; i < argv.size(); ++i) {
        auto &arg = argv[i];

        if (is_name_key) {
            it = map.find(arg);
            assert_command_line_argument_handler(it == map.end(), "unknown name: " << arg);
            bool flag            = false;
            arg_t::key_type &key = it->second.to_key_type(&flag);
            if (flag) {
                key = true;
            } else {
                is_name_key = !is_name_key;
            }
            it->second.is_def = true;
        } else {

            switch (it->second.type) {
                case INT: {
                    std::stringstream ss;
                    ss << arg;
                    ss >> it->second.to_int_type();
                    assert_command_line_argument_handler(!is_int_number(arg), "arg \"" << it->first << "\" not a INT... Value: \"" << arg << "\"");
                } break;
                case STRING: {
                    it->second.to_string_type() = arg;

                } break;
                case ARRAY_STRING: {
                    for (auto &v = it->second.to_array_string_type(); i < argv.size(); i++) {
                        v.push_back(argv[i]);
                    }
                } break;
                default:
                    assert_command_line_argument_handler(true, "unknown type of arg: " << it->second.type);
            }

            is_name_key = !is_name_key;
        }
    }
    return NO_ERROR;
}

std::string tomatik::cla::command_line_argument_handler(int argc, char *argv[], map_cla &map) {
    auto tmp = argv_to_array_string(argc, argv);
    return command_line_argument_handler(tmp, map);
}
