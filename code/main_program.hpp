#ifndef MAIN_PROGRAM_HPP
#define MAIN_PROGRAM_HPP

#include "include/byte_code_genetic.hpp"
#include "include/get_file.hpp"
#include "include/global.hpp"

class main_program {

  public:
    enum class error_t {
        no_error,
        error
    };

    void interpreter(const QString &ui_text, const graph_info_t &vec_gr);

    error_t gen_interpreter(const graph_info_t &graph, const QString &ui_gen_size,
                            const QString &ui_probability_mutation, const QString &save_result);

    void set_cb_selection(int index);
    void set_count_points_crossingover(int index);

    const QString &get_QString_cb_selection() const;
    const QString &get_QString_count_points_crossingover() const;

  private:
    struct combo_box {
        const std::vector<QString> vec;
        int index;

        const QString &currentText() const;

        const QString &itemText(int index) const;

        int currentIndex() const;

        void setCurrentIndex(int index);
    };

  public:
    static inline combo_box
        ui_count_points_crossingover = {{"5", "4", "3", "2", "1"}, 0},
        ui_cb_selection              = {{"Panmixia", "Inbreeding", "Outbreeding", "Tournament", "Roulette"}, 0};
};

#endif  // MAIN_PROGRAM_HPP
