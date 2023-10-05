//
// async_transfer_child_process.cpp (part of NuP)
// setup by Thomas Greve on 2023-10-04
//

#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <utility>
#include "async_transfer_shared_memory.h"
#include "funcs_for_async_exec.h"

namespace bip= boost::interprocess;

int main(int argc, char** argv)    //to call count_multiples_within_a_range() we need three args, therefor argc==4
    {
    if (argc == 4)
        {
        try
            {
            bip::managed_shared_memory segment(bip::open_only, argv[1]);
            //The find()-functions return a std::pair<T*, size_t>: Pointer onto the (1st) element and
            //the size of the object
            std::pair<bool*, size_t> validity= segment.find<bool>(id_validty.c_str());
            std::pair<size_t*, size_t> nbr_multiples= segment.find<size_t>(id_nbr_multiples.c_str());
            std::pair<size_t*, size_t> factor=  segment.find<size_t>(id_factor.c_str());
            std::pair<size_t*, size_t> result= segment.find<size_t>(id_multiples.c_str());
            multiples mult;
            try
                {
                mult= get_multiples_within_a_range(
                                                  std::stoi(argv[2]),
                                                  std::stoi(argv[3]),
                                                  *factor.first
                                                  );
                }
            catch(illegal_factor)
                {
                //An illegal factor was detected - this will be signalled to the parent through the
                //validity-flag in shared memory
                }
            size_t ix{};
            for (size_t v : mult.values_)
                {
                *(result.first+ ix)= v;
                ++ix;
                }
            *nbr_multiples.first= mult.values_.size();
            *validity.first= true;
            }
        catch(...)
            {
            }


        }
    }