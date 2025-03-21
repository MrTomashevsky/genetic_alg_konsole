#include "global.hpp"

#include <iomanip>

#define SLEEP()                                         \
    if (prog.get_sleep_milliseconds_after_stdout_msg()) \
        std::this_thread::sleep_for(std::chrono::milliseconds(prog.get_sleep_milliseconds_after_stdout_msg()));

std::string hash_function(size_t input, size_t size) {
    std::string result(size, 'a');
    for (size_t i = 0; i < size; ++i) {
        result[i] = 'a' + (input % 26);
        input /= 26;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::string __crop(const char *file) {
    std::string ret(file);
    ret.erase(ret.begin(), ret.begin() + ret.find(FATHER_DIR) + sizeof FATHER_DIR);
    return ret;
}

std::string time_to_string(time_t tm) {
    std::string buffer(80, '\0');
    strftime(buffer.data(), buffer.size(), "[%H:%M:%S]", localtime(&tm));
    return buffer;
}

void __cout_message(const char *file, const char *func, int line, const char *color_func) {

    static const auto bt = time(nullptr);
    SLEEP();
    const auto
        &str                 = global_control().str,
        degree_of_completion = get_degree_of_completion(),
        stime                = time_to_string((time(nullptr) - bt + 75600)),
        sclock               = time_to_string(time(nullptr)),
        crop                 = std::string(color_func) + func + "\033[0m";

    std::stringstream ss;
    ss << std::left << degree_of_completion << "-" << sclock << "-" << stime << ": " << str << " ";
    ss << std::setw(40) << crop << " ";
    std::cout << ss.str();
}

std::string to_string(const byte_code::byte_code_basic::graph_t &gr) {
    std::stringstream ss;
    for (size_t i = 0; i < gr.size(); ++i) {
        ss << std::setw(8) << "{";
        for (size_t j = 0; j < gr[i].size(); ++j) {
            std::stringstream temp;
            temp << gr[i][j] << (j != gr[i].size() - 1 ? "," : "");
            ss << std::setw((j != gr[i].size() - 1 ? 4 : 3)) << temp.str();
        }
        ss << std::left << std::setw(1) << " }" << (i != gr.size() - 1 ? "," : "") << "\n"
           << std::right;
    }
    return ss.str();
}

#define PERMUTATION_HPP_ASSERT(ysl)                                                                          \
    if (!(ysl)) {                                                                                            \
        std::cout << "PERMUTATION_HPP " << __LINE__ << " " << __func__ << " !(" << #ysl << ")" << std::endl; \
        exit(-1);                                                                                            \
    }

static bool removeDir(const QString &path) {
    QDir dir(path);

    const QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    bool result = true;
    QFile file;

    for (QFileInfoList::const_iterator it  = fileList.constBegin(),
                                       end = fileList.constEnd();
         result && it != end; ++it) {
        const QString fileName = it->absoluteFilePath();

        qDebug() << QString("Remove ") + (it->isDir() ? "dir" : "file")
                 << fileName;

        file.setFileName(fileName);

        result = it->isDir()
                     ? removeDir(fileName)
                     : file.remove();

        if (!result) {
            qDebug() << file.errorString();
        }
    }

    return result && dir.rmdir(path);
}

save_and_log::save_and_log() {
    QDir dir(QDir::currentPath());
    ret = __init_ret{dir.path(), ""};
    settings_bc(get_file(QDir(QDir::currentPath() + "/settings_bc.json").path().toStdString()));

    for (size_t i = digits.get(4).min; i < digits.get(4).max + 1; ++i) {
        __graphs.push_back(graph_info_t{{vec_graphs.graphs[i]}, {vec_graphs.names[i]}, {vec_graphs.paths[i]}});
    }

    if (!dir.cd("logs")) {
        dir.mkdir("logs");
        dir.cd("logs");
    }
    ret.path_log = dir.path();

    count_variants = 1;
    for (auto &i : digits.arr) {
        count_variants *= (i.max + 1) - i.min;
    }
}

void save_and_log::clear() {
    gen_size.clear();
    cb_selection.clear();
    count_points_crossingover.clear();
    probability_mutation.clear();
    vec_graphs.clear();
    digits.arr.clear();
    count_steps    = 0;
    count_variants = count_steps + 1;
}

void save_and_log::settings_bc(std::string str) {
    clear();

    QJsonParseError parse_error;
    auto tmp          = QString::fromStdString(str).toUtf8();
    QJsonDocument doc = QJsonDocument::fromJson(tmp, &parse_error);
    PERMUTATION_HPP_ASSERT(parse_error.error == QJsonParseError::ParseError::NoError)
    PERMUTATION_HPP_ASSERT(doc.isObject());

    const auto json                           = doc.object();
    this->genetic_function_print              = json["genetic_function_print"].toBool();
    this->sleep_milliseconds_after_stdout_msg = json["sleep_milliseconds_after_stdout_msg"].toInt();
    const auto gen_size                       = json["gen_size"].toArray();
    const auto cb_selection                   = json["cb_selection"].toArray();
    const auto count_points_crossingover      = json["count_points_crossingover"].toArray();
    const auto probability_mutation           = json["probability_mutation"].toArray();

    for (auto &value : gen_size) {
        PERMUTATION_HPP_ASSERT(value.isString());
        this->gen_size.push_back(value.toString());
    }
    for (auto &value : cb_selection) {
        PERMUTATION_HPP_ASSERT(value.isDouble());
        this->cb_selection.push_back(value.toDouble());
    }
    for (auto &value : count_points_crossingover) {
        PERMUTATION_HPP_ASSERT(value.isDouble());
        this->count_points_crossingover.push_back(value.toDouble());
    }
    for (auto &value : probability_mutation) {
        PERMUTATION_HPP_ASSERT(value.isString());
        this->probability_mutation.push_back(value.toString());
    }

    QJsonArray json_array = json["graphs"].toArray();
    foreach (const QJsonValue &value, json_array) {
        PERMUTATION_HPP_ASSERT(value.isObject());
        this->vec_graphs.graphs.push_back({});
        QJsonObject obj         = value.toObject();
        QJsonArray graphs_array = obj["graph"].toArray();
        this->vec_graphs.paths.push_back(obj["good_path"].toInt());
        this->vec_graphs.names.push_back(obj["name"].toString());

        foreach (const QJsonValue &var, graphs_array) {
            std::string str = var.toString().toStdString();
            this->vec_graphs.graphs.back().push_back({});

            for (auto it = str.cbegin(); it < str.cend();) {
                this->vec_graphs.graphs.back().back().push_back(0);
                std::string tmp;
                auto tmp_it = std::find(it, str.cend() - 1, ',');
                std::istringstream(std::string(it, tmp_it)) >> tmp;
                if (tmp == "INF")
                    this->vec_graphs.graphs.back().back().back() = byte_code::INF;
                else
                    std::istringstream(tmp) >> this->vec_graphs.graphs.back().back().back();
                it = ++tmp_it;
            }
        }
    }

    for (const auto &i : this->vec_graphs.graphs)
        for (const auto &j : i)
            PERMUTATION_HPP_ASSERT(!(j.size() != i.size()));
    const size_t arr_sizes[] = {this->gen_size.size(), this->cb_selection.size(), this->count_points_crossingover.size(),
                                this->probability_mutation.size(), this->vec_graphs.size()};
    for (auto var : arr_sizes) {
        PERMUTATION_HPP_ASSERT(var);
        digits.arr.push_back(digit{0, digit_t(var - 1), 0});
    }
}

const save_and_log::__init_ret &save_and_log::init() const {
    return ret;
}

void save_and_log::uninit() {
    QDir dir(QDir::currentPath() + "/" + "logs");
    removeDir(dir.absolutePath());
}

bool save_and_log::next() {
    digits.inc();
    ++count_steps;
    return !digits.is_max();
}

void graph_info_t::clear() {
    graphs.clear();
    names.clear();
    paths.clear();
}

size_t graph_info_t::size() const {
    assert(graphs.size() == names.size() && names.size() == paths.size());
    return graphs.size();
}

std::string to_string(const graph_info_t &gr) {
    std::stringstream ss;

    for (size_t i = 0; i < gr.size(); ++i) {
        ss << gr.names[i].toStdString() << " " << gr.paths[i] << "\n"
           << to_string(gr.graphs[i]) << "\n";
    }
    return ss.str();
}
