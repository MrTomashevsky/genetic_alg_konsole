#include <QCoreApplication>
#include <QDir>
#include <QString>

#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>

#include "include/global.hpp"
#include "include/tomatik_clah.hpp"
#include "main_program.hpp"

#define WORK_ONES_HELP_TEXT " work_ones:\n"                                                                               \
                            "  format-values  performs work according to the following list of modes\n"                   \
                            "\n"                                                                                          \
                            "                    \"<let> <size> <cros> <pm>\"\n"                                          \
                            "\n"                                                                                          \
                            "                 where\n"                                                                    \
                            "                    let - first small letter of cb_selection\n"                              \
                            "                    size - digit of gen_size\n"                                              \
                            "                    cros - digit of count_points_crossingover\n"                             \
                            "                    pm - digit of probability_mutation\n"                                    \
                            "\n"                                                                                          \
                            "                interpretation takes place through spaces\n"                                 \
                            "                all args - strings in this format.\n"                                        \
                            "                You must specify all args.\n"                                                \
                            "                The remaining default values mean passing through all modes in the cycle.\n" \
                            "\n"                                                                                          \
                            "                examples:\n"                                                                 \
                            "                    \" t 100 3 10\" - tournament\n"                                          \
                            "                    \" i  200  3  50 \" - inbreeding\n"                                      \
                            "                    \"o 50  1  8 \" - outbreeding\n"                                         \
                            "                etc.\n"                                                                      \
                            "  --help        this information"

#define command_line_argument_handler(...)                       \
    {                                                            \
        auto error = command_line_argument_handler(__VA_ARGS__); \
        if (error != NO_ERROR) {                                 \
            std::cout << error << std::endl;                     \
            exit(-1);                                            \
        }                                                        \
    }

using namespace tomatik::cla;

static size_t length_int(int arg) {
    return std::to_string(arg).size();
}

struct subcommand_info {
    std::string info;
    std::function<void(void)> function;
    map_cla map_command{};
    arg_t::array_string_type arr{};
};

struct work_ones_value {
    QString gen_size{};
    int cb_selection{};
    int count_points_crossingover{};
    QString probability_mutation{};

    CONST_GETTER(gen_size);
    CONST_GETTER(cb_selection);
    CONST_GETTER(count_points_crossingover);
    CONST_GETTER(probability_mutation);
};

global_info global_control;
save_and_log prog;
std::function<std::string(void)> get_degree_of_completion;

void init_genetic_alg_konsole(const graph_info_t graph, const QString &path_log, const QString &path, const QString gen_size, int cb_selection,
                              int count_points_crossingover, const QString probability_mutation) {
    global_control().clock = std::chrono::steady_clock::now() + std::chrono::hours(1);
    main_program w;
    w.set_cb_selection(cb_selection);
    w.set_count_points_crossingover(count_points_crossingover);

    const QString std_name         = gen_size + "_" + w.get_QString_cb_selection() + "_" + w.get_QString_count_points_crossingover() + "_" + probability_mutation + "_" + graph.names.back() + "_";
    const QString file_save_result = QDir(path_log + "/" + std_name).path() + "r.log";
    COUT_GLOBAL_CONTROL_MESSAGE("save to " + file_save_result.toStdString());
    COUT_GLOBAL_CONTROL_MESSAGE("begin");
    w.gen_interpreter(graph, gen_size, probability_mutation, file_save_result);
    COUT_GLOBAL_CONTROL_MESSAGE("end");
}

void work() {

    ::get_degree_of_completion = []() { return tomatik::degree_of_completion(prog.get_count_steps() * 100, prog.get_count_variants()); };

    COUT_GLOBAL_CONTROL_MESSAGE("START PROGRAM!!!");
    const auto paths = prog.init();
    QString tmp;

    do {
        const auto &graph = prog.get_graph_on_index();
        tmp.clear();
        QTextStream(&tmp) << "selected " << prog.get_gen_size() << " " << prog.get_cb_selection() << " " << prog.get_count_points_crossingover() << " " << prog.get_probability_mutation() << " " << graph.names.back();

        COUT_GLOBAL_CONTROL_MESSAGE(tmp.toStdString());
        init_genetic_alg_konsole(graph,
                                 (paths.path_log),
                                 (paths.path),
                                 (prog.get_gen_size()),
                                 (prog.get_cb_selection()),
                                 (prog.get_count_points_crossingover()),
                                 (prog.get_probability_mutation()));

    } while (prog.next());

    COUT_GLOBAL_CONTROL_MESSAGE("END PROGRAM!!!");
}

arg_t::array_string_type work_ones_values_to_string(const arg_t::string_type &s) {
    arg_t::array_string_type ret;
    std::stringstream ss(s);

    while (ss.tellg() != std::stringstream::pos_type(-1)) {
        ret.push_back("");
        ss >> ret.back();
    }

    return ret;
}

void work_ones(std::vector<work_ones_value> args) {
    size_t i      = 0;
    ::get_degree_of_completion = [&i, &args]() { return tomatik::degree_of_completion(i * 100, args.size()); };

    COUT_GLOBAL_CONTROL_MESSAGE("START WORK_ONES!!!");
    const auto paths = prog.init();
    QString tmp;

    for (; i < args.size(); ++i) {

        const auto &graph = prog.get___graphs()[0];
        tmp.clear();
        QTextStream(&tmp) << "selected " << args[i].get_gen_size() << " " << args[i].get_cb_selection() << " " << args[i].get_count_points_crossingover() << " " << args[i].get_probability_mutation() << " " << graph.names.back();

        COUT_GLOBAL_CONTROL_MESSAGE(tmp.toStdString());
        init_genetic_alg_konsole(graph,
                                 (paths.path_log),
                                 (paths.path),
                                 (args[i].get_gen_size()),
                                 (args[i].get_cb_selection()),
                                 (args[i].get_count_points_crossingover()),
                                 (args[i].get_probability_mutation())

        );
    }
    i = 0;
    COUT_GLOBAL_CONTROL_MESSAGE("END PROGRAM!!!");
}

#define ERROR(ysl, msg)                              \
    if (ysl) {                                       \
        std::cout << "error : " << msg << std::endl; \
        exit(-1);                                    \
    }
#define WARNING(ysl, msg) \
    if (ysl) std::cout << "warning : " << msg << std::endl;

std::vector<work_ones_value> work_ones_values_processing(const arg_t::array_string_type &work_ones_values) {
    std::vector<work_ones_value> args(work_ones_values.size(), work_ones_value{});

    for (size_t i = 0; i < work_ones_values.size(); ++i) {
        auto arr = work_ones_values_to_string(work_ones_values[i]);
        if (arr.back() == "")
            arr.erase(arr.end() - 1);

        WARNING(arr.size() > 4, "args more 4 \"" << work_ones_values[i] << "\". the args wat skipped");

        for (size_t j = 0; j < arr.size(); j++) {
            switch (j) {
                case 0: {
                    ERROR(arr[j].size() != 1, "unknown letter of cb_selection \"" << arr[j] << "\"");
                    const auto &v = main_program::ui_cb_selection.vec;
                    auto it       = std::find_if(v.begin(), v.end(), [arr, j](const QString &name) { return name.toStdString().front() == toupper(arr[j].front()); });
                    ERROR(it == v.end(), "unknown symb of cb_selection \"" << arr[j] << "\"");
                    args[i].cb_selection = std::distance(v.begin(), it);
                } break;
                case 1: {
                    ERROR(!tomatik::cla::is_int_number(arr[j]), "not a digit gen_size \"" << arr[j] << "\"");
                    args[i].gen_size = QString::fromStdString(arr[j]);
                } break;
                case 2: {
                    ERROR(!tomatik::cla::is_int_number(arr[j]), "not a digit count_points_crossingover \"" << arr[j] << "\"");
                    const auto &v = main_program::ui_count_points_crossingover.vec;

                    args[i].count_points_crossingover = std::distance(v.begin(), std::find(v.begin(), v.end(), QString::fromStdString(arr[j])));
                    ERROR(args[i].count_points_crossingover >= v.size(), "unknown count_points_crossingover \"" << arr[j] << "\"");
                } break;
                case 3: {
                    ERROR(!tomatik::cla::is_int_number(arr[j]), "not a digit probability_mutation \"" << arr[j] << "\"");
                    args[i].probability_mutation = QString::fromStdString(arr[j]);
                } break;
                default:
                    WARNING(1, "unknown regim format \"" << j << "\". the mode was skipped");
            }
        }
    }

    return args;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    using map_t = std::map<std::string, subcommand_info>;

    map_t map_subcommand;
    arg_t::array_string_type
        work_ones_values,
        __help;
    map_cla map_command;

    map_subcommand["remove"]    = map_t::value_type::second_type{"remove dir \"logs\" if it be created and all files in dir", []() { prog.uninit(); }};
    map_subcommand["work"]      = map_t::value_type::second_type{"go work program", []() {
                                                                ::work();
                                                            }};
    map_subcommand["--help"]    = map_t::value_type::second_type{"this information", [&map_subcommand]() {
                                                                  const auto max_element = std::max_element(map_subcommand.begin(), map_subcommand.end(), [](const map_t::value_type &a, const map_t::value_type &b) { return a.first.size() < b.first.size(); });
                                                                  std::cout << QCoreApplication::applicationName().toStdString() << ":\n";
                                                                  for (auto &i : map_subcommand) {
                                                                      std::cout << std::left << "  " << std::setw(max_element->first.size() + 1) << i.first << " " << i.second.info << std::endl;
                                                                  }
                                                              }};
    map_subcommand["graphs"]    = map_t::value_type::second_type{"print all graphs", []() {
                                                                  const auto &v = prog.get_vec_graphs();
                                                                  for (size_t i = 0; i < v.size(); ++i) {
                                                                      std::cout << std::setw(8) << "Длина лучшего пути: " << v.paths[i] << std::endl;
                                                                      std::cout << to_string(v.graphs[i]) << std::endl;
                                                                  }
                                                              }};
    map_subcommand["work_ones"] = map_t::value_type::second_type{"work one, for more information, please call \"work_ones --help\"", [&map_subcommand, &work_ones_values, &__help]() {
                                                                     if (map_subcommand["work_ones"].map_command["format-values"].is_def) {
                                                                         work_ones(work_ones_values_processing(work_ones_values));
                                                                     } else if (map_subcommand["work_ones"].map_command["--help"].is_def) {
                                                                         map_cla m{};
                                                                         command_line_argument_handler(__help, m);
                                                                         std::cout << QCoreApplication::applicationName().toStdString()
                                                                                   << WORK_ONES_HELP_TEXT
                                                                                   << std::endl;
                                                                     } else {
                                                                         std::cout << "error work ones : not select additional agrument" << std::endl;
                                                                     }
                                                                 },
                                                                 map_cla{{"format-values", arg_t{ARRAY_STRING, &work_ones_values}}, {"--help", arg_t{ARRAY_STRING, &__help}}}};

    for (auto &var : map_subcommand) {
        map_command[var.first] = arg_t{ARRAY_STRING, &var.second.arr};
    }

    command_line_argument_handler(argc, argv, map_command);

    for (auto &var : map_subcommand) {
        command_line_argument_handler(var.second.arr, var.second.map_command);
    }

    for (auto &var : map_command) {
        if (var.second.is_def) {
            map_subcommand[var.first].function();
        }
    }
}
