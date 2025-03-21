#ifndef TOMATIK_DEGREE_OF_COMPLETION_H
#define TOMATIK_DEGREE_OF_COMPLETION_H

#include <cassert>
#include <cstdlib>
#include <string>

namespace tomatik {

#define TOMATIK_DEGREE_OF_COMPLETION_H_ASSERT(what) assert(what)

    template <typename int_type>
    size_t len_digit(int_type digit) {
        size_t len{};
        do {
            digit /= 10;
            ++len;
        } while (digit);
        return len;
    }

    static std::string degree_of_completion(int numerator, int denominator) {
        const int
            len_numerator   = len_digit(numerator),
            len_denominator = len_digit(denominator);

        TOMATIK_DEGREE_OF_COMPLETION_H_ASSERT(len_numerator > len_denominator);
        char buffer[40] = {'\0'};
        snprintf(buffer, std::size(buffer), "[%*d/%*d]", len_denominator, numerator, len_denominator, denominator);
        return buffer;
    }

}  // namespace tomatik
#endif  // TOMATIK_DEGREE_OF_COMPLETION_H
