#include <dai/alldai.h>
#include <iostream>
#include <fstream>
#include <string>
#include "dai/emrun.h"
#include <dai/util.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <iomanip>      // std::setprecision
using namespace std;
using namespace dai;
//
// Generate a random number between 0 and 1
// return a uniform number in [0,1].
double unifRand()
{
    return rand()/double(RAND_MAX);
}
// Reset the random number generator with the system clock.
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

std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
string alarmlearnedfgloc = "/Applications/Utilities/libDAI-0.3.1/alarm1_factor_graphs/";
int main(int argc, char *argv[]) {
string line1;

int t=100;
int p_c = 0; // counter to skip the first four config lines in the cpt of the fg file
for( size_t n = 0; n < t; n++ )
{
	string number2; 
	stringstream num2; 
	num2 << n; 
	number2 = num2.str();
	string parent2 = alarmlearnedfgloc +"alarm_"+number2+".fg";	
	const char* parentfgfile = &parent2[0];
	ofstream c1 (parentfgfile); 
	ifstream p1 (argv[1]);  // input file	
	getline(p1,line1);
	c1<<line1<<endl;
	int factor =0;
	int no_factors = 0;
	int* factor_names;
	int factor_pos=0;
	int* states_arr;
	int no_states = 1;
	int arr_size =1;
	int factor_states =0;
	int tot_arrays =0;
	int arr_set =0;
	int arr_set_states =0;
	int no_arr_arrset =0;
	int stat =0;
	while(getline(p1,line1))
	{
		line1 = trim(line1);
		stringstream ss(line1);
		if(line1.empty())
		{
		  p_c = 0;
		  stat =0;
		  cout<<"factor variable "<<factor<<endl;
		  c1<<endl;
		  no_states = 1;
		  arr_size =1;
		  factor++;
		}
		if(p_c == 1)
		{
		 ss>>no_factors;
		 cout<<"no. of factors "<<no_factors<<endl;
		c1<<no_factors<<endl;
		}
		if(p_c == 2)
		{
		 factor_names = new int[no_factors];
		 std::vector<std::string> fields;
	 	 fields = tokenizeString( line1, true,"  ");		 
		 for( size_t i = 0; i < fields.size(); ++i ) 
		 {
		  stringstream n;
		  n << fields[i]; 
		  string s = n.str();	   
		  if( s.find_first_not_of(" ")!= std::string::npos)	 	
		  {
			c1<<s<<" ";	
			n >> factor_names[i];
			if(factor_names[i] == (factor-1))
			 factor_pos =i;
		  }
	 	 }
		 c1<<endl;
		 //cout<<"factor position "<<factor_pos<<endl;
		}
		if(p_c == 3)
		{
		 states_arr = new int[no_factors];
		 std::vector<std::string> fields1;
	 	 fields1 = tokenizeString( line1, true,"  ");		 
		 for( size_t j = 0; j < fields1.size(); ++j ) 
		 {
		  stringstream n1;
		  n1 << fields1[j]; 
		  string state = n1.str();	   
		  if( state.find_first_not_of(" ")!= std::string::npos)	 	
		  {
			c1<<state<<" ";	
			n1 >> states_arr[j];
			no_states = no_states * states_arr[j];
			if(j < factor_pos)
			 arr_size = arr_size * states_arr[j];
			if(j == factor_pos)
			 {
			  factor_states = states_arr[j];
			  arr_set_states = arr_size*states_arr[j];			  
			 }
		  }
	 	 }
		 c1<<endl;
		 tot_arrays = no_states/arr_size;
		 arr_set = no_states/arr_set_states;
		 no_arr_arrset = factor_states;
		 //cout<<"array size "<<arr_size<<endl;
		 //cout<<no_states<<"\t"<<"no. of states "<<no_states<<"\t"<<"total arrays "<<tot_arrays<<"\t"<<"array set "<<arr_set<<"\t"<<"no. arrays "<<no_arr_arrset<<"\t"<<"factor states "<<factor_states<<endl;
		c1<<no_states<<endl;
		}				
		if(p_c == 5)
	        {
		 for(int k=0; k<arr_set; k++)
		  {
			std::vector< double * > Arrays;
		       	for(int l=0; l<no_arr_arrset; l++)
			{
			 Arrays.push_back( new double[arr_size]);
			}
			vector<double*>::iterator it;
			for(int m=0; m<arr_size; m++)
			{
				it=Arrays.begin();
                std::vector<double> vec;
			    for(int i=0; i<no_arr_arrset; i++)
			    {
                    vec.push_back(unifRand());
			    }
			    const double total = std::accumulate(vec.begin(), vec.end(), 0.0);
			    for (double& value: vec)
			    {
                    double* arr = *it;
                    value /= total;
                    arr[m] = value;
                    ++it;
                    //cout<<value <<endl;				
			    }
                // cout<<endl;
			    vec.clear();
				/*double sum=0;
				for(int l=0; l<no_arr_arrset; l++)
				{
				  seed();
				  double* arr = *it;
				  double val;
				  if(l == no_arr_arrset-1)
					val = 1-sum;
				  else
				        val = unifRand()/(arr_set+4);				  	
				  arr[m] = val;	
				  sum = sum +val;	
				  ++it;
				}*/
			}
			vector<double*>::iterator it1;
			it1=Arrays.begin();		
			for(int l=0; l<no_arr_arrset; l++)
			{
				double* arr = *it1;
				for(int m=0; m<arr_size; m++)
				{
				 stringstream v;
				 v << fixed << setprecision(4) << arr[m];
				 c1<<stat<<"   "<<v.str()<<endl;
				 stat++;
				}
				++it1;
			}

		  }
		}
		p_c++;
	} 
	c1.close();
}
return 0;
}
