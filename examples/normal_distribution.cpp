#include <dai/alldai.h>
#include <iostream>
#include <fstream>
#include <string>
#include "dai/emrun.h"
#include <dai/util.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/random/normal_distribution.hpp>
#include <iomanip>      // std::setprecision
#include <sstream>
#include <string>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <algorithm>
#include<math.h>
using namespace std;
using namespace dai;
using namespace boost;
void seed()
{
	/*Declare variable to hold seconds on clock.*/
    //	time_t seconds;
	/*Get value from system clock and place in seconds variable.*/
    //	time(&seconds);
	/*Convert seconds to a unsigned integer.*/
    struct timeval time;
    gettimeofday(&time,NULL);
    
    // microsecond has 1 000 000
    srand((time.tv_sec * 1000) + (time.tv_usec));
    //  rnd_seed((time.tv_sec * 1000) + (time.tv_usec / 10) );
}
double unifRand()
{
    return rand()/double(RAND_MAX);
}


// Define a random number generator and initialize it with a reproducible
// seed.
mt19937 generator(42);

// Define a uniform random number distribution which produces "double"
// values between 0 and 1 (0 inclusive, 1 exclusive).
boost::uniform_real<> uni_dist(0,1);
boost::variate_generator<mt19937&, boost::uniform_real<> > uni(generator, uni_dist);

boost::normal_distribution<> nd(0.0, 1.0);
boost::variate_generator<mt19937&, boost::normal_distribution<> > normal(generator, nd);
int main()
{
    generator.seed(static_cast<unsigned int>(std::time(0)));
    seed();
    
    for (int i=0; i<10; i++)
    {
        
    cout<<normal()<<" "<<uni()<<" "<<rnd_stdnormal()<<" "<<rnd_uniform()<<" "<<unifRand()<<endl;
    }
    return 0;
}
//
//int main()
//{
//
//std::vector<double> vec;
//seed();
//  for (int i=0; i<10; i++) {
//	vec.push_back(unifRand());
//	//cout<<vec[i]<<endl;
//	//++p[int(number)];
//  }
//
//const double total = std::accumulate(vec.begin(), vec.end(), 0.0);
//for (double& value: vec) 
//	{
// 	value /= total;
//	cout<<value <<endl;
//	}
//
//  return 0;
//}
