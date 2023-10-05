//
// async_execution_function.h (part of NuP)
// setup by Thomas Greve on 2023-09-26
//

#ifndef NUP_FUNCS_FOR_ASYNC_EXEC_H
#define NUP_FUNCS_FOR_ASYNC_EXEC_H


#include <vector>
#include <stdexcept>

void count_multiples_within_a_range(
                                   size_t start,
                                   size_t one_beyond_end,
                                   size_t factor
                                   );


struct multiples
    {
    size_t factor_{};
    std::vector<size_t> values_{};
    };

class illegal_factor : public std::logic_error
    {
    using std::logic_error::logic_error;
    };

multiples get_multiples_within_a_range(
                                      size_t start,
                                      size_t one_beyond_end,
                                      size_t factor
                                      );

#endif //NUP_FUNCS_FOR_ASYNC_EXEC_H
