//
// async_execution_function.cpp (part of NuP)
// setup by Thomas Greve on 2023-09-26
//

#include <iostream>
#include <syncstream>
#

#include "funcs_for_async_exec.h"

void count_multiples_within_a_range(
                                   size_t start,
                                   size_t one_beyond_end,
                                   size_t factor
                                   )
    {
    size_t nbr_multiples{};
    for (auto val{start}; val != one_beyond_end; ++val)
        {
        if (val% factor == 0)
            {
            ++nbr_multiples;
            }
        }
    std::osyncstream(std::cout) <<
                                nbr_multiples << " multiples of " << factor <<
                                " between " << start << " and " << one_beyond_end- 1 <<
                                "\n";
    }


multiples get_multiples_within_a_range(
                                      size_t start,
                                      size_t one_beyond_end,
                                      size_t factor
                                      )
    {
    if (factor != 0)
        {
        multiples result;
        result.factor_= factor;
        for (auto val{start}; val != one_beyond_end; ++val)
            {
            if (val% factor == 0)
                {
                result.values_.push_back(val);
                }
            }
        return result;
        }
    else
        {
        throw illegal_factor{"There are no multiples of 0!"};
        }
    }

