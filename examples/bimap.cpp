#include <boost/config.hpp>

#include <iostream>
#include <string>

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_inserter.hpp>

using namespace boost::bimaps;
using namespace boost;
using namespace std;
int main()
{
    typedef bimap<
    
    unordered_set_of<int >,
    multiset_of< double, std::greater<double> >
    
    > population_bimap;
    
    typedef population_bimap::value_type population;
    
    population_bimap pop;
    pop.insert( population(1,0.000001321) );
    pop.insert( population(2,0.000000023) );
    pop.insert( population(3,0.000000023));
    pop.insert( population(4,0.00001865) );
    pop.insert( population(5,0.00016363) );
    pop.insert( population(6,0.00030195) );
    pop.insert( population(7,0.00030195) );

    
    std::cout << "Countries by their population:    ---   " << std::endl;
    
    // First requirement
    /*<< The right map view works like a
     `std::multimap< long, std::string, std::greater<long> >`,
     We can iterate over it to print the results in the required order. >>*/
    for( population_bimap::right_const_iterator
        i = pop.right.begin(), iend = pop.right.end();
        i != iend ; ++i )
    {
        std::cout << i->second << " with " << i->first << std::endl;
    }
    
    // Second requirement
    /*<< The left map view works like a `std::unordered_map< std::string, long >`,
     given the name of the country we can use it to search for the population
     in constant time >>*/
    std::cout << "Population of China: " << pop.left.at(4) << std::endl;
    //]
    
    return 0;}