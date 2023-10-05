//
// async_execution_child_process.cpp (part of NuP)
// setup by Thomas Greve on 2023-09-26
//

#include <string>
#include "funcs_for_async_exec.h"


int main(int argc, char** argv)    //to call count_multiples_within_a_range() we need three args, therefor argc==4
    {
    if (argc == 4)
        {
        count_multiples_within_a_range(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]));
        }
    }