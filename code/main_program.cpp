#include "main_program.hpp"

#define COUT_MAIN_PROGRAM_MESSAGE(what) COUT_GLOBAL_CONTROL_MESSAGE(std::string("\033[31;1;4m") + what + "\033[0m")

static bool is_normal_file(std::ofstream **file, const std::string &fn, const char *message) {
    if ((*file)) {
        (*file)->close();
        delete (*file);
    }
    (*file) = new std::ofstream(fn);
    COUT_MESSAGE_IS_ERROR((*file));

    if (!(*file)->is_open()) {
        delete (*file);
        COUT_GLOBAL_CONTROL_MESSAGE(message);
        return false;
    }
    return true;
};

void main_program::interpreter(const QString &ui_text, const graph_info_t &vec_gr) {
    const auto byte_text = byte_code::byte_code_text_interpreter().text_interpreter(ui_text.toStdString());
    for (auto &i : vec_gr.graphs) {
        byte_code::byte_code_interpreter bc(byte_text, true);
        const auto result = bc.interpreter(i, true, nullptr);
    }
}

main_program::error_t main_program::gen_interpreter(const graph_info_t &graph, const QString &ui_gen_size,
                                                    const QString &ui_probability_mutation, const QString &save_result) {
    bool error      = true;
    const auto size = ui_gen_size.toInt(&error);

    if (!error) {
        COUT_GLOBAL_CONTROL_MESSAGE("population not digit!\n");
        return main_program::error_t::error;
    }

    global_control().graphs               = graph;
    global_control().COUNT_POINTS         = ui_count_points_crossingover.currentText().toInt();
    global_control().PROBABILITY_MUTATION = ui_probability_mutation.toInt(&error) / 100.;

    if (!error) {
        COUT_GLOBAL_CONTROL_MESSAGE("probability_mutation not digit!\n");
        return main_program::error_t::error;
    }

    if (!is_normal_file(&global_control().fout_result, save_result.toStdString(), "fout_result file not open"))
        return main_program::error_t::error;

    byte_code::genetic(size, ui_cb_selection.currentIndex());

    return main_program::error_t::no_error;
}

void main_program::set_cb_selection(int index) {
    ui_cb_selection.setCurrentIndex(index);
}

void main_program::set_count_points_crossingover(int index) {
    ui_count_points_crossingover.setCurrentIndex(index);
}

const QString &main_program::get_QString_cb_selection() const {
    return ui_cb_selection.itemText(ui_cb_selection.currentIndex());
}

const QString &main_program::get_QString_count_points_crossingover() const {
    return ui_count_points_crossingover.itemText(ui_count_points_crossingover.currentIndex());
}

const QString &main_program::combo_box::currentText() const {
    return vec[index];
}

const QString &main_program::combo_box::itemText(int index) const {
    return vec[index];
}

int main_program::combo_box::currentIndex() const {
    return index;
}

void main_program::combo_box::setCurrentIndex(int index) {
    this->index = index;
}
