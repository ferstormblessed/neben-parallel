//
// Created by tg on 8/30/23.
//

#include <cstdlib>
#include <array>
#include <vector>
#include <thread>
#include <future>
#include <iostream>
#include <syncstream>

#include "funcs_for_async_exec.h"


template <typename thread_type>
void execute_function_as_threads(bool join_with_finished_threads)
    {
    constexpr size_t nbr_threads{4};
    std::array<thread_type, nbr_threads> threads;
    std::array<size_t, nbr_threads>factors{ 3, 5, 7, 11};

    for (size_t ix{}; ix != nbr_threads; ++ix)
        {
        threads[ix]= thread_type{count_multiples_within_a_range, 0, 10'000, factors[ix]};
        }

    if(join_with_finished_threads)
        {
        for (size_t ix{}; ix != nbr_threads; ++ix)
            {
            threads[ix].join();
            }
        }
    }


void demo_jthread_vs_thread()
    {
    std::cout << "Executing function as std::jthread-instances. Finished threads are explicitly joined.\n";
    execute_function_as_threads<std::jthread>(true);
    std::cout << "Executing function as std::jthread-instances. Finished threads are NOT explicitly joined.\n";
    execute_function_as_threads<std::jthread>(false);
    std::cout << "Executing function as std::thread-instances (mind the missing j). Finished threads are explicitly joined.\n";
    execute_function_as_threads<std::thread>(true);
    std::cout << "Executing function as std::thread-instances. Finished threads are NOT explicitly joined.\n";
    execute_function_as_threads<std::thread>(false);
    }


void count_multiples_until_termination(
                                     std::stop_token st,   //important: needs to be the 1st parameter
                                     size_t start,
                                     size_t factor
                                     )
    {
    size_t nbr_multiples{};
    auto val{start};
    std::chrono::time_point t_start{std::chrono::steady_clock::now()};
    while (!st.stop_requested())
        {
        if (val% factor == 0)
            {
            ++nbr_multiples;
            }
        ++val;
        }
    std::chrono::milliseconds lot{
                                 std::chrono::duration_cast<
                                                           std::chrono::milliseconds
                                                           >(std::chrono::steady_clock::now()- t_start)
                                 };
    std::osyncstream(std::cout) <<
                                nbr_multiples << " multiples of " << factor << " found within " <<
                                lot.count() << "ms" <<
                                "\n";
    }


void demo_stoppable_jthread()
    {
    constexpr size_t nbr_threads{4};
    std::array<std::jthread, nbr_threads> threads;
    std::array<size_t, nbr_threads>factors{ 3, 5, 7, 11};

    for (size_t ix{}; ix != nbr_threads; ++ix)
        {
        threads[ix]= std::jthread{
                                 count_multiples_until_termination,
                                 //note: as count_multiples_until_termination takes a std::stop_token as FIRST argument
                                 //the compiler implicitly adds a stop_token which is related to the stop_source of
                                 //the new instantiated jthread-instance
                                 0,
                                 factors[ix]
                                 };
        }

    std::chrono::seconds runtime{3};
    std::osyncstream(std::cout) << "This demo runs for " << runtime.count() << "s.\n";
    std::chrono::time_point start{std::chrono::steady_clock::now()};
    while (std::chrono::steady_clock::now()- start < runtime)
        std::this_thread::sleep_for(std::chrono::milliseconds{10});
    std::osyncstream(std::cout) << "Runtime gone - Stopping threads now.\n";
    for (size_t ix{}; ix != nbr_threads; ++ix)
        {
        threads[ix].request_stop();
        }
    }


//Necessary to add boost's home-dir in include_subdirectories() in the project's cmakelists.txt
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

void demo_threadpool()
    {
    const size_t nbr_threads{4};
    const std::vector<size_t> factors{{
                                      3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
                                      53, 59, 61, 67, 71, 73, 79, 83, 89, 97
                                      }};
    boost::asio::thread_pool tp(nbr_threads);

    for (auto f : factors)
        {
        /*
        This one does not compile
        boost::asio::post(
                         tp,
                         count_multiples_within_a_range,
                         0,
                         10'000,
                         f
                         );
        Function and arguments need to be bound (as shown below) or wrapped into a lambda of functor.
        */
        boost::asio::post(
                         tp,
                         std::bind(
                                  count_multiples_within_a_range,
                                  0,
                                  10'000,
                                  f
                                  )
                         );
        }
    std::osyncstream(std::cout) << "All tasks posted.\n";
    tp.wait();
    std::osyncstream(std::cout) << "All tasks done.\n";
    }


void demo_async()
    {
    const std::vector<size_t> factors{{
                                      3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
                                      53, 59, 61, 67, 71, 73, 79, 83, 89, 97
                                      }};
    std::vector<std::future<void>> all_futures;

    for (auto f : factors)
        {
        all_futures.push_back(
                             std::async(
                                       std::launch::async,   //don't forget this if you want
                                                             // to run the function asynchronously
                                       count_multiples_within_a_range,
                                       0,
                                       10'000,
                                       f
                                       )
                             );
        }
    std::osyncstream(std::cout) << "All (" << factors.size() << ") counting functions launched aynchroniously.\n";
    for (auto& f : all_futures)
        {
        f.get();
        }
    std::osyncstream(std::cout) << "All functions terminated.\n";
    }

#include <algorithm>
#include <execution>

void demo_parallel_algorithm()
    {
    const std::vector<size_t> factors{{
                                      3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
                                      53, 59, 61, 67, 71, 73, 79, 83, 89, 97
                                      }};
    std::for_each(
                 std::execution::par,
                 factors.begin(),
                 factors.end(),
                 std::bind_front(
                                count_multiples_within_a_range,
                                0,
                                10'000
                                )
                 );
    }


#include <boost/process.hpp>

void demo_process()
    {
    const std::vector<size_t> factors{{
                                      3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
                                      53, 59, 61, 67, 71, 73, 79, 83, 89, 97
                                      }};
    std::vector<boost::process::child> all_child_processes{};
    for (size_t f : factors)
        {

        all_child_processes.emplace_back(
                                        "async_execution_child_process",
                                        "0",
                                        "10000",
                                        std::to_string(f)
                                        );
        /*
        Alternative formulation: One string contains cmd and args - but you are responsible
        for proper placement of blanks, etc!

        all_child_processes.emplace_back(
                                        "async_execution_child_process"
                                        " 0"
                                        " 10000 "
                                        +
                                        std::to_string(f)
                                        );
        */
        }
    for (auto& p : all_child_processes)
        {
        p.wait();
        }
    }
