//
// async_transfer.cpp (part of NuP)
// setup by Thomas Greve on 2023-10-01
//

#include <iostream>
#include <syncstream>
#include <future>
#include <thread>
#include "funcs_for_async_exec.h"

void show_results(const multiples& m)
    {
    std::cout << m.values_.size() << " multiples of " << m.factor_ << " found:\n";
    for (auto v : m.values_)
        {
        std::cout << v << "   ";
        }
    std::cout << "\n";
    }

using results= std::pair<bool, multiples>;

results get_results_through_future(std::future<multiples>& f)
    {
    results r;
    r.first= false;
    try
        {
        r.second= f.get();    //In case of an exception being transferred through
                                   //the shared state it will be rethrown at this point
        r.first= true;
        }
    catch (illegal_factor ex)
        {
        std::cout <<
                    "=== An exception signalling that an illegal factor was caught, saying: " <<
                    ex.what() <<
                    " ===" <<
                    "\n";
        }
    return r;
    };


const std::vector<size_t> factors{{
                                  3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 0,
                                  53, 59, 61, 67, 71, 73, 79, 83, 89, 97
                                  }};

void demo_async_as_provider()
    {
    std::vector<std::future<multiples>> all_futures;
    for (auto f : factors)
        {
        all_futures.push_back(
                             std::async(
                                       std::launch::async,   //once again: never forget to specify std::launch::async
                                                             //if you want to run the function asynchronously
                                       get_multiples_within_a_range,
                                       0,
                                       10'000,
                                       f
                                       )
                             );
        }
    std::osyncstream(std::cout) << "All (" << factors.size() << ") functions launched aynchroniously.\n";
    for (auto& f : all_futures)
        {
        results r{get_results_through_future(f)};
        if (r.first)
            {
            show_results(r.second);
            }
        }
    std::cout << "All functions terminated.\n";
    }


void demo_packaged_task_as_provider()
    {
    std::vector<std::future<multiples>> all_futures;
    std::vector<std::jthread> all_threads;
    for (auto f : factors)
        {
        std::packaged_task<multiples(size_t, size_t, size_t)> pt{get_multiples_within_a_range};
        all_futures.push_back(pt.get_future());
        all_threads.emplace_back(
                                std::move(pt),
                                0,
                                10'000,
                                f
                                );
        }
    std::osyncstream(std::cout) << "All (" << factors.size() << ") functions launched aynchroniously.\n";
    for (auto& f : all_futures)
        {
        results r{get_results_through_future(f)};
        if (r.first)
            {
            show_results(r.second);
            }
        }
    std::cout << "All functions terminated.\n";
    }


void get_multiples_within_a_range_over_promise(
                                              size_t start,
                                              size_t one_beyond_end,
                                              size_t factor,
                                              std::promise<multiples>&& pr
                                              )
    {
    /*
    get_multiples_within_a_range() needs to be wrapped in this function as we have to distribute
    ordinary results (set_value()) and exceptions (set_exception()) 'by hand' when using a promise -
    std::async and std::packaged_task are doing this automatically (i.e. within their own
    functionality).
    */
    try
        {
        multiples mult{get_multiples_within_a_range(start, one_beyond_end, factor)};
        pr.set_value(mult);
        }
    catch (illegal_factor ex)
        {
        //An exception can't be transferred as value but as a std::exception_ptr through the shared state.
        //p.set_exception(std::make_exception_ptr(ex));   //this would work as well
        pr.set_exception(std::current_exception());
        }
    }

void demo_promise_as_provider()
    {
    std::vector<std::future<multiples>> all_futures;
    std::vector<std::jthread> all_threads;
    for (auto f : factors)
        {
        std::promise<multiples> p;
        all_futures.push_back(p.get_future());
        all_threads.emplace_back(
                                get_multiples_within_a_range_over_promise,
                                0,
                                10'000,
                                f,
                                std::move(p)   //mind the move: std::promises can't be copied
                                );
        }
    std::osyncstream(std::cout) << "All (" << factors.size() << ") functions launched aynchroniously.\n";
    for (auto& f : all_futures)
        {
        results r{get_results_through_future(f)};
        if (r.first)
            {
            show_results(r.second);
            }
        }
    std::cout << "All functions terminated.\n";
    }

#include <boost/process.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <utility>
#include "async_transfer_shared_memory.h"

namespace bp= boost::process;
namespace bip= boost::interprocess;

struct casm   //short for child with attached shared memory
    {
    bp::child child_;
    bip::managed_shared_memory segment_;
    std::string id_of_segment_;
    };

void demo_process_with_result_transfer()
    {
    const std::string prefix_for_ids{"SharedMemoryForFactor"};
    const std::pair<size_t, size_t> range_to_be_scanned_for_multiples{0, 10000};
    const size_t sz_of_shared_memory{
                                    sizeof(bool)   //for validity
                                    +
                                    2* sizeof(size_t)   //for factor and nbr of found multiples
                                    +
                                    (range_to_be_scanned_for_multiples.second
                                    -
                                    range_to_be_scanned_for_multiples.first)
                                    *
                                    sizeof(size_t)
                                    +
                                    10000
                                    };
    std::vector<casm> all_child_processes{};
    for (size_t f : factors)
        {
        casm curr_casm;
        curr_casm.id_of_segment_= prefix_for_ids+ std::to_string(f);   //id for the results of current factor
        //Step1: Allocating shared memory
        bip::shared_memory_object::remove(curr_casm.id_of_segment_.c_str());    //to make sure that it doesn't exist already
        curr_casm.segment_= bip::managed_shared_memory(
                                                      bip::create_only,
                                                      curr_casm.id_of_segment_.c_str(),   //std::string doesn't work;
                                                                                          // C-style string needed
                                                      //following: the size in bytes: You are responsible that
                                                      //it's large enough.
                                                      sz_of_shared_memory
                                                      );
        //Step2: Structuring memory for the values to be transferred
        curr_casm.segment_.construct<bool>(id_validty.c_str())(false);   //a boolean - to indicate validity
        curr_casm.segment_.construct<size_t>(id_nbr_multiples.c_str())(0);   //a size_t - for the actual number
                                                                             //of multiples
        curr_casm.segment_.construct<size_t>(id_factor.c_str())(f);   //a size_t - for the factor
        curr_casm.segment_.construct<size_t>(id_multiples.c_str())[   //an array (mind the []) of size_t for the
                                                                      //multiples themselves
                                                                  (range_to_be_scanned_for_multiples.second
                                                                  -
                                                                  range_to_be_scanned_for_multiples.first)
                                                                  ](0);
        //Step3: Creating and starting the child process and store the related casm
        curr_casm.child_= bp::child(
                                   "async_transfer_child_process",
                                   curr_casm.id_of_segment_.c_str(),
                                   std::to_string(range_to_be_scanned_for_multiples.first).c_str(),
                                   std::to_string(range_to_be_scanned_for_multiples.second).c_str()
                                   );
        all_child_processes.push_back(std::move(curr_casm));
        }
    for (auto& p : all_child_processes)
        {
        //Waiting for the current child-process to terminate
        p.child_.wait();
        //Obtaining results from shared memory
        multiples mult{};
        bip::managed_shared_memory segment(bip::open_only, p.id_of_segment_.c_str());
        if (*segment.find<bool>(id_validty.c_str()).first)
            {
            std::pair<size_t*, size_t> nbr_multiples= segment.find<size_t>(id_nbr_multiples.c_str());
            mult.factor_= *segment.find<size_t>(id_factor.c_str()).first;
            std::pair<size_t*, size_t> multiples= segment.find<size_t>(id_multiples.c_str());
            for (size_t ix{}; ix != *(nbr_multiples.first); ++ix)
                {
                mult.values_.push_back(*(multiples.first+ ix));
                }
            show_results(mult);
            }
        else
            {
            std::cout << "=== No valid results for factor " <<
                         *segment.find<size_t>(id_factor.c_str()).first <<
                         " ===" <<
                         "\n";
            }
        //Clean up
        p.segment_.destroy<bool>(id_validty.c_str());
        p.segment_.destroy<size_t>(id_nbr_multiples.c_str());
        p.segment_.destroy<size_t>(id_factor.c_str());
        p.segment_.destroy<size_t>(id_multiples.c_str());
        bip::shared_memory_object::remove(p.id_of_segment_.c_str());
        }
    }


