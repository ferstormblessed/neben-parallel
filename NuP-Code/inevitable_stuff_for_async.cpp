//
// stuff_being_needed.cpp (part of NuP)
// setup by Thomas Greve on 2023-10-01
//



#include <iostream>


/* ============================================================================
Demo of std::bind and std::bind_front
===============================================================================*/

#include <functional>
#include <cmath>

using std::placeholders::_1;
using std::placeholders::_2;

double area_general_triangle(double len_a, double len_b, double len_c)
    {
    double temp{(len_a+ len_b+ len_c)/ 2.};
    return std::sqrt(temp* (temp- len_a)* (temp- len_b)* (temp- len_c));
    }

std::function<double(double)> area_equilateral_triangle{std::bind(
                                                                 area_general_triangle,
                                                                 _1, _1, _1
                                                                 )};
std::function<double(double,double)> area_isosceles_triangle{std::bind(
                                                                      area_general_triangle,
                                                                      _1, _2, _1
                                                                      )};

void demo_binders_triangle_areas()
    {
    std::cout <<
             "Areas of a\n" <<
             "- general triangle with a=1.2 b=2.3 and c=3.4: a=" << area_general_triangle(1.2, 2.3, 3.4) <<
             "\n- isosceles triangle with a=c=1.2 and b=2.3: a=" << area_isosceles_triangle(1.2, 2.3) <<
             "\n- equilateral triangle with a=b=c=1.2: a=" << area_equilateral_triangle(1.2) <<
             "\n";
    }




struct scaling
    {
    double factor_;
    double offset_;
    };

double scaled_value_with_offset(scaling scaling, double value)
    {
    return scaling.factor_* value+ scaling.offset_;
    }

scaling c_to_f{9./5, 32.};
scaling f_to_k{5./9, 255.37};
scaling k_to_c{1., -273.15};

std::function<double(double)> celsius_to_fahrenheit{std::bind_front(scaled_value_with_offset, c_to_f)};
std::function<double(double)> fahrenheit_to_kelvin{std::bind_front(scaled_value_with_offset, f_to_k)};
auto kelvin_to_celsius{std::bind_front(scaled_value_with_offset, k_to_c)};
//Alternative code with bind:
std::function<double(double)> celsius_to_fahrenheit_alt{std::bind(scaled_value_with_offset, c_to_f, _1)};
std::function<double(double)> fahrenheit_to_kelvin_alt{std::bind(scaled_value_with_offset, f_to_k, _1)};
auto kelvin_to_celsius_alt{std::bind_front(scaled_value_with_offset, k_to_c, _1)};

void demo_binders_conversions(double temp_C)
    {
    double curr_conversion{};
    std::cout << "Conversions of a temperature between units - Starting with " << temp_C << "C\n" <<
                 "C to F: " << (curr_conversion= celsius_to_fahrenheit(temp_C)) << "F\n" <<
                 "F to K: " << (curr_conversion= fahrenheit_to_kelvin(curr_conversion)) << "K\n" <<
                 "K to C: " << kelvin_to_celsius(curr_conversion) << "C\n"
                 ;
    }

void demo_binders()
    {
    demo_binders_triangle_areas();
    demo_binders_conversions(12.3);
    }



/* ============================================================================
Demo of function, sunctionobject and lambda
===============================================================================*/

#include <vector>
#include <cmath>

using prm_set= std::vector<double>;
std::vector<prm_set> parameter_sets{
                                   {1, 2, 3, 4},            //f(x)= x^3+ 2x^2+ 3x+ 4
                                   {0.5, 0, 0, 0, 0, -42}   //f(x)= 0.5x^5- 42
                                   };

double func(double val, const prm_set& prms)
    {
    double sum{};
    size_t curr_exponent{prms.size()- 1};
    for (double curr_prm : prms)
        {
        sum+= curr_prm* std::pow(val, curr_exponent);
        --curr_exponent;
        }
    return sum;
    }

class func_obj
    {
    private:
        prm_set prms_;
    public:
        func_obj(const prm_set& prms)
          :
          prms_{prms}
            {
            }

        double operator()(double val)
            {
            double sum{};
            size_t curr_exponent{prms_.size()- 1};
            for (double curr_prm : prms_)
                {
                sum+= curr_prm* std::pow(val, curr_exponent);
                --curr_exponent;
                }
            return sum;
            }
    };


void demo_eo()
    {
    const double start{-3.1415};
    const double end{+3.1415};
    const double delta{0.1};

    std::cout << "Testing function with 1st prmset\n";
    for (double val= start; val < end; val+= delta)
        {
        std::cout << func(val, parameter_sets[0]) << ", ";
        }
    std::cout << "\nTesting function with 2nd prmset\n";
    for (double val= start; val < end; val+= delta)
        {
        std::cout << func(val, parameter_sets[1]) << ", ";
        }

    func_obj fo1{parameter_sets[0]};
    func_obj fo2{parameter_sets[1]};
    std::cout << "\nTesting function with both prmsets\n";
    for (double val= start; val < end; val+= delta)
        {
        std::cout << fo1(val) << ", " << fo2(val) << "\n";
        }

    prm_set curr_prm_set{parameter_sets[0]};
    auto lambda{
               [curr_prm_set](double val) -> double
                   {
                   double sum{};
                   size_t curr_exponent{curr_prm_set.size()- 1};
                   for (double curr_prm : curr_prm_set)
                       {
                       sum+= curr_prm* std::pow(val, curr_exponent);
                       --curr_exponent;
                       }
                   return sum;
                   }
               };

    std::cout << "Testing lambda\n";
    for (double val= start; val < end; val+= delta)
        {
        std::cout << lambda(val) << ", ";
        }
    std::cout << "\n";
    }



/* ============================================================================
Demo of RAII
===============================================================================*/

#include <cstdio>
#include <string>

class filewrapper
    {
    private:
        std::FILE* pFile_;   //C-based filehandle
    public:
        filewrapper(const std::string& path, const std::string& mode);
        ~filewrapper();
        template<typename T>
        std::pair<bool, T> read();
        template<typename T>
        bool write(T value);
    };

filewrapper::filewrapper(const std::string& path, const std::string& mode)
    {
    pFile_= std::fopen(path.c_str(), mode.c_str());
    }

filewrapper::~filewrapper()
    {
    std::fclose(pFile_);
    }

template<typename T>
std::pair<bool, T> filewrapper::read()
    {
    std::pair<bool, T> item;
    item.first= (std::fread(&item.second, sizeof(T), 1, pFile_) == 1);
    return item;
    }

template<typename T>
bool filewrapper::write(T value)
    {
    return (std::fwrite(&value, sizeof(T), 1, pFile_) == 1);
    }


void write(const std::string& path)
    {
    filewrapper fw(path, "w");
    double d{3.1415};
    fw.write<double>(d);
    long long ll{-12345678987654321ll};
    fw.write<long long>(ll);
    unsigned int ui{42u};
    fw.write<unsigned int>(ui);
    std::cout << "Written " << d << ", " << ll << " and " << ui << " to " << path << "\n";
    }

void read(const std::string& path)
    {
    filewrapper fw(path, "r");
    double d{fw.read<double>().second};
    long long ll{fw.read<long long>().second};
    unsigned int ui{fw.read<unsigned int>().second};
    std::cout << "Read " << d << ", " << ll << " and " << ui << " from " << path << "\n";
    }

void demo_raii()
    {
    std::string path{"/home/tg/SWDev/Prj/FHKiel/NuP/Data/demoraii"};
    write(path);
    read(path);
    }


/* ============================================================================
Demo of variadic template
===============================================================================*/


#include <string>
#include <iostream>
#include "../support/randomizer.h"

//Example 1: Dumping values of various numerical types

template<typename current_type>
std::string isolate_next_value(current_type val)
    {
    return std::to_string(val);
    }

template<typename current_type, typename... remaining_types>
std::string isolate_next_value(current_type val, remaining_types... remaining_vals)
    {
    std::string result{std::to_string(val)};
    return result+ ",  "+ isolate_next_value(remaining_vals...);
    }

template<typename... some_types>           //some_types is a template parameter pack
void dumping_values_of_different_types(some_types... args)          //args is a function parameter pack
    {
    std::cout << "Example 1: Dumping numerical values of different types\n";
    std::cout << isolate_next_value(args...) << "\n";    //Pack Expansion -> calc(arg1, arg2, /*etc*/ argN)
    }



// Example 2: Dices of various types (where a type represents the number of planes)

template<unsigned short nbr_sides>
class dice
    {
    public:
        unsigned short roll() const;
    private:
        mutable randomizer<unsigned short> rnd{1, nbr_sides};
    };

 template<unsigned short nbr_sides> unsigned short dice<nbr_sides>::roll() const
    {
    return rnd();
    };

template<typename first_type, typename... remaining_types>
std::string roll(first_type current_dice, remaining_types... dices_to_be_unpacked)
    {
    std::string result{std::to_string(current_dice.roll())};
    result+= " ";
    return result+ " | "+ roll(dices_to_be_unpacked...);
    }

template<typename final_type>
std::string roll(final_type current_dice)
    {
    return std::to_string(current_dice.roll());
    }

template<typename... dice_types>
void rolling_dices_of_various_geometry(dice_types... dices)
    {
    std::cout << "Example 2: Rolling dices of various geometry\n";
    std::cout << roll(dices...) << "\n";
    }


void demo_variadic_template()
    {
    dumping_values_of_different_types(1u, -2ll, 3.f, 4ul, -5., 6);

    dice<4> d4;
    dice<6> d6_1, d6_2;
    dice<8> d8;
    dice<12> d12_1, d12_2;
    dice<20> d20;
    rolling_dices_of_various_geometry(d12_1, d4, d6_1, d6_2, d8, d12_2);
    rolling_dices_of_various_geometry(d20, d8);
    }




