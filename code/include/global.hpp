#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include "array_digits.hpp"
#include "byte_code.hpp"
#include "get_file.hpp"
#include "random.hpp"
#include "tomatik_degree_of_completion.hpp"

using graph_t      = byte_code::byte_code_basic::graph_t;
constexpr auto INF = byte_code::INF;

struct graph_info_t {
    std::vector<graph_t> graphs;
    std::vector<QString> names;
    std::vector<size_t> paths;

    void clear();

    size_t size() const;
};

std::string to_string(const graph_info_t &gr);

std::string to_string(const byte_code::byte_code_basic::graph_t &gr);

enum result_code_t : uint8_t {
    NO_RESULT,
    NORMAL_RESULT,
    NO_NORMAL_RESULT
};

struct global_info {
    std::string str;
    std::ofstream *fout_result;
    result_code_t result_code = NO_RESULT;
    graph_info_t graphs;
    size_t step                 = 0;
    size_t COUNT_POINTS         = 0;
    double PROBABILITY_MUTATION = 0;

    std::chrono::steady_clock::time_point clock;

    global_info &operator()() {
        return *this;
    }
};

#define __TEMPLATE_GETTER(name, type, attribute, what) \
    type get_##name() attribute { return what; }
#define GETTER(name) __TEMPLATE_GETTER(name, auto &, , name)
#define CONST_GETTER(name) __TEMPLATE_GETTER(name, const auto &, const, name)
#define SAVE_AND_LOG_GETTER(name, what) __TEMPLATE_GETTER(name, auto &, , name what)
#define SAVE_AND_LOG_CONST_GETTER(name, what) __TEMPLATE_GETTER(name, const auto &, const, name what)

struct save_and_log {

    struct __init_ret {
        QString path, path_log;
    };

  private:
    std::vector<QString> gen_size;
    std::vector<int> cb_selection;
    std::vector<int> count_points_crossingover;
    std::vector<QString> probability_mutation;
    graph_info_t vec_graphs;

    bool genetic_function_print;

    __init_ret ret;
    size_t sleep_milliseconds_after_stdout_msg = 0;
    size_t count_steps                         = 0;
    size_t count_variants                      = count_steps + 1;

    std::vector<graph_info_t> __graphs;

  public:
    SAVE_AND_LOG_CONST_GETTER(gen_size, [digits[0]]);
    SAVE_AND_LOG_CONST_GETTER(cb_selection, [digits[1]]);
    SAVE_AND_LOG_CONST_GETTER(count_points_crossingover, [digits[2]]);
    SAVE_AND_LOG_CONST_GETTER(probability_mutation, [digits[3]]);
    const auto get_graph_on_index() const {
        return __graphs[digits[4]];
    }
    CONST_GETTER(sleep_milliseconds_after_stdout_msg);
    CONST_GETTER(vec_graphs);
    CONST_GETTER(genetic_function_print);
    CONST_GETTER(count_steps);
    CONST_GETTER(count_variants);

    CONST_GETTER(__graphs);

    using digit_t = uint8_t;

  private:
    array_digits<digit_t> digits;

    void clear();

    void settings_bc(std::string str);

  public:
    using digit = array_digits<digit_t>::digit;

    save_and_log();

    const __init_ret &init() const;

    void uninit();

    bool next();
};

extern global_info global_control;
extern save_and_log prog;
extern std::function<std::string()> get_degree_of_completion;

#define COUT_FLUSH() std::cout.flush()

void __cout_message(const char *file, const char *func, int line, const char *color_func);

#define COUT_MESSAGE(what, color_func)                            \
    {                                                             \
        __cout_message(__FILE__, __func__, __LINE__, color_func); \
        std::cout << what << std::flush;                          \
    }

#define COUT_GLOBAL_CONTROL_MESSAGE_DEF(what) COUT_MESSAGE(what, "\033[32m")
#define COUT_GLOBAL_CONTROL_MESSAGE(what) COUT_GLOBAL_CONTROL_MESSAGE_DEF(what << std::endl)
#define COUT_GLOBAL_CONTROL_MESSAGE_DIF_COLOR(what) COUT_MESSAGE(what << std::endl, "\033[35;1m")
#define COUT_MESSAGE_FUNC() \
    if (prog.get_genetic_function_print()) COUT_GLOBAL_CONTROL_MESSAGE("")

#define COUT_MESSAGE_IS_ERROR(ysl) \
    if (!(ysl)) COUT_GLOBAL_CONTROL_MESSAGE_DIF_COLOR(std::string("\033[34;4m") + #ysl + " == 0 !!!\033[0m")

#define output(w) COUT_GLOBAL_CONTROL_MESSAGE(w)

#endif  // GLOBAL_HPP
