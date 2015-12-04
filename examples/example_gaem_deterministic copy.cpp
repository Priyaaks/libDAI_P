/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2010  Joris Mooij  [joris dot mooij at libdai dot org]
 */

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
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/config.hpp>

#include <string>
#include <iostream>

#include <boost/bimap.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_inserter.hpp>
using namespace std;
using namespace dai;
using namespace boost;
// Define a random number generator and initialize it with a reproducible
// seed.
mt19937 generator(42);

// Define a uniform random number distribution which produces "double"
// values between 0 and 1 (0 inclusive, 1 exclusive).
boost::uniform_real<> uni_dist(0,1);
boost::variate_generator<mt19937&, boost::uniform_real<> > uni(generator, uni_dist);

boost::normal_distribution<> nd(0.0, 1.0);
boost::variate_generator<mt19937&, boost::normal_distribution<> > normal(generator, nd);

// GA - initialization
size_t max_gen = 10; // no. of generations
size_t population_size =4; // population size
Real pc;// = 0.8; // crossover probability - between 0.6 an 0.9
Real pm;// = 0.3; // mutation probability - between 1/pop.size and 1/chromosome_length
int population_no=0; // current population id

// 1. Creating initial population of EM runs 
	
char* gibbsfile;// = "/home/priya/libDAI-0.3.1/alarm/500/5.tab";
//const char* gibbsfile = argv[1];
string alarmfgloc;// = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/"; 
string alarmlearnedfgloc;// = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/";
char* networkname ; //= "alarm";
size_t maxIters =1000; // maximum number of iterations
char* emfile;
ofstream out,out1;
EMRun* runname;
vector <EMRun*> layer; 
vector <EMRun*> layer_curr;
vector <EMRun*> layer_prev;
int noVars;
int *arrval;
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
	//time_t seconds;
	/*Get value from system clock and place in seconds variable.*/
//	time(&seconds);
	/*Convert seconds to a unsigned integer.*/
//     struct timeval time;
//     gettimeofday(&time,NULL);

     // microsecond has 1 000 000
     //srand((time.tv_sec * 1000) + (time.tv_usec / 10));
    //srand((unsigned)time(0));
}
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
double euclideanDist(double v1[],double v2[],int length)
{
    double distance=0;
    for(int i=0;i<length;i++)
    {
        distance = distance+ pow((v1[i]-v2[i]),2); //Euclidean distance
    }
    distance = sqrt(distance);
    return distance;
}
double klDist(double v1[],double v2[],int length)
{
    double distance=0;
    for(int i=0;i<length;i++)
    {
        distance = distance + (v1[i]* log2( v1[i]/v2[i])); // KL distance
    }
    return distance;
}
double calculateDistance(string child, string child1)
{
    
    const char* childfgfile = &child[0];
    const char* childfgfile1 = &child1[0];
    //cout<<"child file "<< childfgfile<<" "<<childfgfile1<<endl;
    string line1,line2;
    ifstream p1 (childfgfile);
    ifstream c1 (childfgfile1);
    getline(p1,line1);
    getline(c1,line2);
    
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
    int p_c = 0;
    double pmut=0,pm=0,distval=0;
    while(getline(p1,line1)&&getline(c1,line2))
    {
        line1 = trim(line1);
        line2 = trim(line2);
        //cout<<p_c<<" "<<pmut<<" "<<pm<<" "<<line1<<endl;
        stringstream ss(line1);
        stringstream ss1(line2);
        if(line1.empty())
        {
            p_c = 0;
            stat =0;
            //cout<<"factor variable "<<factor<<endl;
            //seed();
            //pmut = unifRand(); // generates random a value between 0 and 1 for child1
            //cout<<"Mutation probability "<<pmut<<endl;
            no_states = 1;
            arr_size =1;
            factor++;
        }
        if(pmut>pm)
        {
            //c1<<line1<<endl;
            //cout<<"No mutation"<<endl;
            //p_c++;
            //continue;
        }
        if(pmut<=pm) //do mutation handling the constraint that the states of the random variable sum to 1.
        {
            //cout<<"Mutation happening "<<endl;
            double sumo=0,sumo1=0;
            if(p_c == 0)
            {
                //c1<<line1<<endl;
            }
            if(p_c == 1)
            {
                ss>>no_factors;
                //cout<<"no. of factors "<<no_factors<<endl;
                //c1<<no_factors<<endl;
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
                        //c1<<s<<" ";
                        n >> factor_names[i];
                        if(factor_names[i] == (factor-1))
                            factor_pos =i;
                    }
                }
                //c1<<endl;
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
                        //c1<<state<<" ";
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
                //c1<<endl;
                tot_arrays = no_states/arr_size;
                arr_set = no_states/arr_set_states;
                no_arr_arrset = factor_states;
                //cout<<"array size "<<arr_size<<endl;
                //cout<<no_states<<"\t"<<"no. of states "<<no_states<<"\t"<<"total arrays "<<tot_arrays<<"\t"<<"array set "<<arr_set<<"\t"<<"no. arrays "<<no_arr_arrset<<"\t"<<"factor states "<<factor_states<<endl;
                //c1<<no_states<<endl;
            }
            if(p_c >= 5)
            {
                int k=0;
                while(k<arr_set)
                {
                    std::vector< double * > Arrays, Arrays1;
                    for(int l=0; l<no_arr_arrset; l++)
                    {
                        Arrays.push_back( new double[arr_size]);
                        Arrays1.push_back( new double[arr_size]);
                    }
                    vector<double*>::iterator it2,it21;
                    it2=Arrays.begin();
                    it21=Arrays1.begin();
                    //get the current cpt values and copy them in the array
                    int l=0;
                    while(l<no_arr_arrset)
                    {
                        double* arr = *it2;
                        double* arr1 = *it21;
                        // cout<<"new arr"<<endl;
                        int m=0;
                        while(m<arr_size)
                        {
                            line1 = trim(line1);
                            line2 = trim(line2);
                            //cout<<line1 <<" "<<line2<<endl;
                            std::vector<std::string> values,values1;
                            //                                values = tokenizeString( line1, true," +");
                            //                                values1 = tokenizeString( line2, true," +");
//                            split(values, line1, is_any_of(" +")); // here it is
//                            split(values1, line2, is_any_of(" +")); // here it is
                            char_separator<char> sep("   ");
                            tokenizer<char_separator<char>> tokens(line1, sep);
                            for (const auto& t : tokens) {
                                //cout<<t;
                                if(t!=" ")
                                {
                                    values.push_back(t);
                                   // cout<<"pushed "<<t<<endl;
                                }
                            }
                            //cout<<endl;
                            tokenizer<char_separator<char>> tokens1(line2, sep);
                            for (const auto& t : tokens1) {
                                //cout<<t;
                                if(t!=" ")
                                {
                                    values1.push_back(t);
                                    //cout<<"pushed "<<t<<endl;
                                }
                            }
                            //cout<<endl;
                            int ii=0;
                           // cout<<values.size()<<" "<<values1.size()<<endl;
                            for( size_t j = 0; j < values.size(); ++j )
                            {
                                stringstream n1,n2;
                                n1 << values[j];
                                n2 << values1[j];
                                string state = n1.str();
                                string state1 = n2.str();
                                //cout<<state<<" !! "<<state1<<" !! "<<endl;
                                if( state.find_first_not_of(" ")!= std::string::npos && state1.find_first_not_of(" ")!= std::string::npos)
                                {
                                    if(ii==1)
                                    {
                                        n1 >> arr[m];
                                        n2 >> arr1[m];
                                        //cout<<p_c<<" "<<line1<<" "<<"arrval = "<<values[0]<<"\tarr[m]"<<arr[m]<<" m = "<<m<<endl;
                                        m++;
                                        if(m<arr_size)
                                        {
                                            getline(p1,line1);
                                            getline(c1,line2);
                                        }
                                        ii=0;
                                    }
                                    else
                                        ii=1;
                                } //end of if
                            } // end of for
                        } // end of while -m
                        l++;
                        if(l<no_arr_arrset)
                        {
                            getline(p1,line1);
                            getline(c1,line2);
                        }
                        ++it2; ++it21;
                    } //end of while -l
                    // cout<<"new val"<<endl;
                    vector<double*>::iterator it,it1;
                    double sumarr[arr_size];
                    for(int m=0; m<arr_size; m++)
                    {
                        it=Arrays.begin();
                        it1=Arrays1.begin();
                        double valarr[no_arr_arrset],valarr1[no_arr_arrset];
                        for(int l=0; l<no_arr_arrset; l++) // stdnormal rand values are generated and sum is found
                        {
                            double* arr = *it;
                            double* arr1 = *it1;
                            double val = arr[m];
                            double val1 =  arr1[m];
                            valarr[l]=val;
                            valarr1[l]=val1;
                            //cout<<val<<" "<<val1<<endl;
                            ++it; ++it1;
                        }
                        
                        sumo+=euclideanDist(valarr,valarr1,no_arr_arrset);
                        sumo1+=klDist(valarr,valarr1,no_arr_arrset);
                        // cout<<endl;
                    }
                    k++;
                    if(k<arr_set)
                    {
                        getline(p1,line1);
                        getline(c1,line2);
                    }
                }// end of while -k
                //cout<<" "<<sumo<<" "<<sumo1<<endl;
                distval = distval+sumo;
            }// end of if p_c>=5
            
        } // end of if pmut<p_m
        p_c++;
    } // end of while loop
    c1.close();
    p1.close();
    //cout<<distval<<endl;
    return distval;
}

void startEM()
{
	size_t i =0,j=0;
	//cout<<"In start EM"<<endl;
	while(i<population_size)
	{
	 string number;
	 stringstream num;
	 num << i;
	 number = num.str();
	 string dirname;
	 stringstream p_id;
	 p_id << population_no;
	 dirname = p_id.str();
	 string fgfileloc =  alarmfgloc + dirname+"/"+networkname+"_"+number+".fg";
	 const char* fgfile = &fgfileloc[0];
	 cout<<"Starting EM "<<fgfileloc<<endl;
	 runname = new EMRun(networkname, i, fgfile, gibbsfile,emfile); // args are -1.network name 2.factor-graph number 3. evidencefile

	 // 2. Run EM
	 boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters)); // args - max iters
	 layer.push_back(runname);
	 sleep(1);
	 // 4. Next individual3a
	 i=i+1;
	}

}


void checkEMforTermination()
{
         string dirname; 
	 stringstream p_id; 
	 p_id << population_no; 
	 dirname = p_id.str();
	string loc =  alarmlearnedfgloc + "learned/"+dirname+"/";
	const char* learnedloc = &loc[0];
	mkdir(learnedloc,0755);
	layer_prev.assign(layer_curr.begin(), layer_curr.end());
	layer_curr.clear();
	while (true){
	    //cout<<"in while loop "<<layer.size()<<endl;
	    vector<EMRun*>::iterator it; 
	    for (it=layer.begin(); it != layer.end(); ++it){    
	      //cout<<"layer size "<<layer.size()<<endl;	
	      if((*it)->hasTerminated()) { //if it has terminated, set the run aside
//        ////////////
//              if(layer_prev.size() !=0 )
//              {
//                  boost::thread thread6(boost::bind(&EMRun::stopEM, boost::ref(*it)));
//                  
//                  cout<<" iters = "<<(*it)->getIters()<<endl;
//                  double iters = (*it)->getIters();
//                  double val = (*it)->getRun();
//                  cout<<" em run stopped "<<val<<endl;
//                  double ll = (*it)->getLL();
//                  vector<EMRun*>::iterator itprev;
//                  std::vector<Real> llTraceprev;
//                  // get the pervious run
//                  
//                  for (itprev=layer_prev.begin(); itprev != layer_prev.end(); ++itprev)
//                  {
//                      double runid = (*itprev)->getRun();
//                      if(val==runid)
//                      {
//                          //out1<<runid<<" "<<iters<<" ";
//                          
//                          llTraceprev = (*itprev)->lastlog();
//                          cout<<val<<" "<<"runid = "<<(*itprev)->getRun()<<"  "<<" "<<(*itprev)->getIters()<<endl;
//                          break;
//                      }
//                  }
//                  //check if current EM run's LL is greater or equal to prev EM run's LL
//                  
//                  double llprev;
//                  cout<<" size = "<<llTraceprev.size()<<endl;
//                  if((llTraceprev.size()-1)>iters)
//                      llprev = llTraceprev.at(iters);
//                  else
//                      llprev = llTraceprev.at(llTraceprev.size()-1);
//                  cout<<" LL for prev run = "<<llprev<<endl;
//                  if(ll>=llprev)
//                  {
//                      cout<<" current LL "<<ll<<" is good"<<endl;
//                      boost::thread thread7(boost::bind(&EMRun::resumeEM, boost::ref(*it)));
//                      cout<<" em run resumed"<<val<<endl;
//                  }
//                  else
//                  {
//                      cout<<" current LL "<<ll<<" is not good"<<endl;
//                      *it = *itprev;
//                      (*itprev)->forceStopped();
//                      cout<<" em run not resumed"<<val<<endl;
//                  }
//                  
//              }
//      
//        ////////////
        double ll = (*it)->getLL();
		double iters = (*it)->getIters();
		double runid = (*it)->getRun();
        //cout<<"em run "<<runid<<" terminated "<<iters<<endl;
        std::vector<Real> llTrace = (*it)->lastlog();
		string number; 
		stringstream num; 
		num << runid; 
		number = num.str();
	 	string fglearnedfileloc =  alarmlearnedfgloc + "learned/"+dirname+"/"+networkname+"_"+number+".fg";
        const char* fglearnedfile = &fglearnedfileloc[0];
		cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		// 3. Store the learned factor graphs
		InfAlg* inf = (*it)->getInf();
		ofstream learnedstream;
		cout<<"learned file = "<<fglearnedfile<<endl;
		learnedstream.open(fglearnedfile);
		learnedstream.precision(2);
		learnedstream << inf->fg();  
		learnedstream.flush();
		learnedstream.close();
		//delete inf;
		sleep(1);
		cout<<"Run number "<<number<<" has terminated"<<endl;
		out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl;
//        out1<<runid<<" "<<iters<<" ";
//        for(std::vector<Real>::iterator logval = llTrace.begin(); logval != llTrace.end(); ++logval)
//        {
//                  out1 << *logval<<" ";
//        }
//        out1<<" "<<endl;
        std::vector<int> mutateFactors;
        mutateFactors = (*it)->getmutateFactors();
        cout<<" Run id "<<runid;
        cout<<" iiiii "<<mutateFactors.size()<<endl;
		layer_curr.push_back(*it); 
		layer.erase(it);	
		if (layer.size() == 0){break;}
		else { it = layer.begin(); }
        }
            //we stop the run
        else if(((*it)->getIters()) == maxIters){
		double ll = (*it)->getLL();
		double iters = (*it)->getIters();
		double runid = (*it)->getRun();
		string number; 
		stringstream num; 
		num << runid; 
		number = num.str();
	 	string fglearnedfileloc =  alarmlearnedfgloc + "learned/"+dirname+"/"+networkname+"_"+number+".fg";
	    	const char* fglearnedfile = &fglearnedfileloc[0];
		//cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		//cout<<"Run "<<(*it)->getRun()<<" has reached maximum iterations, so stopping the run"<<endl;
        
	 	// 3. Store the learned factor graphs
		InfAlg *inf = (*it)->getInf();
	    	ofstream learnedstream;
	    	//cout<<"learned file = "<<fglearnedfile<<endl;
	    	learnedstream.open(fglearnedfile);
	    	learnedstream.precision(12);
	    	learnedstream << inf->fg();  
	    	learnedstream.close();
		//delete inf;
		sleep(1);		
		//cout<<"Run number "<<number<<" has reached maximum iterations so stopped"<<endl;
		out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
		layer_curr.push_back(*it);
            if(!(*it)->isForceStop())
            {
                boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(*it)));
                
            }
            layer.erase(it);
	//	if (layer.size() == 0){break;}
	//	else { it = layer.begin(); }
	      }
        else 
        {
            //*it=compareRuns(it);
            if(layer_prev.size() !=0 )
            {
                boost::thread thread6(boost::bind(&EMRun::stopEM, boost::ref(*it)));
                
                cout<<" iters = "<<(*it)->getIters()<<endl;
                double iters = (*it)->getIters();
                double val = (*it)->getRun();
                cout<<" em run stopped "<<val<<endl;
                double ll = (*it)->getLL();
                vector<EMRun*>::iterator itprev;
                std::vector<Real> llTraceprev;
                // get the pervious run
                
                for (itprev=layer_prev.begin(); itprev != layer_prev.end(); ++itprev)
                {
                    double runid = (*itprev)->getRun();
                    if(val==runid)
                    {
                        //out1<<runid<<" "<<iters<<" ";
                        
                        llTraceprev = (*itprev)->lastlog();
                        cout<<val<<" "<<"runid = "<<(*itprev)->getRun()<<"  "<<" "<<(*itprev)->getIters()<<endl;
                        break;
                    }
                }
                //check if current EM run's LL is greater or equal to prev EM run's LL
                
                double llprev;
                cout<<" size = "<<llTraceprev.size()<<endl;
                if((llTraceprev.size()-1)>iters)
                    llprev = llTraceprev.at(iters);
                else
                    llprev = llTraceprev.at(llTraceprev.size()-1);
                cout<<" LL for prev run = "<<llprev<<endl;
                if(ll>=llprev)
                {
                    cout<<" current LL "<<ll<<" is good"<<endl;
                    boost::thread thread7(boost::bind(&EMRun::resumeEM, boost::ref(*it)));
                    cout<<" em run resumed"<<val<<endl;
                }
                else
                {
                    cout<<" current LL "<<ll<<" is not good"<<endl;
                    *it = *itprev;
                    (*itprev)->forceStopped();
                    cout<<" em run not resumed"<<val<<endl;
                }
                
            }

            
        }
	      //cout<<"Going .."<<endl;
	 }
	 if (layer.size() == 0){ //cout<<"breaking while"<<endl;
		break; }
	 else { //cout<<"iterating"<<endl;
		}
	  sleep(1); 
	}
}
void doMutation() // open each child file and apply mutation if required
{
    string dirname2;
    stringstream p_id2;
    p_id2 << population_no+1;
    dirname2 = p_id2.str();
    string childloc =  alarmlearnedfgloc + dirname2+"/";
    const char* childfgloc = &childloc[0];
    //int t=2;
    //cout<<"In Mutation"<<endl;
    //cout<<"declarations done "<<arrval[0]<<endl;
    vector<EMRun*>::iterator it = layer_curr.begin();
       
    for( size_t n = 0; n < population_size; n++ )
    {
        string number1;
        stringstream num1;
        num1 << n;//arrval[n];
        number1 = num1.str();
        string child =  childloc+ networkname+"C_"+number1+".fg";
        string child1 =  childloc+ networkname+"_"+number1+".fg";
        const char* childfgfile = &child[0];         
        const char* childfgfile1 = &child1[0];         
        //cout<<"child file "<< childfgfile<<endl;
        //cout<<"parent file "<<childfgfile1<<endl;
        string line1;
        ifstream p1 (childfgfile); 
        ofstream c1 (childfgfile1); 
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
        int p_c = 0;
        double pmut=0;
        
        dai::hash_map<int, double> hashmap = (*it)->getParams();
        double runid = (*it)->getRun();
        std::vector<int> mutateFactors;
        mutateFactors.clear();
        mutateFactors = (*it)->getmutateFactors();
        cout<<" Run id "<<runid;
        cout<<"______"<<mutateFactors.size()<<endl;
        while(getline(p1,line1))
        {
            line1 = trim(line1);
            //cout<<p_c<<" "<<pmut<<" "<<pm<<" "<<line1<<endl;
            stringstream ss(line1);
            //cout<<params.length()<<endl;
            if(line1.empty())
            {
              p_c = 0;
              stat =0;
             // cout<<"factor variable "<<factor;
                std::vector<int>::iterator it;
                
                it = find (mutateFactors.begin(), mutateFactors.end(), factor);
                if (it != mutateFactors.end())
                {
                    //std::cout << "Element found in myvector: " << *it << '\n';
                    pmut=1;
                }
                else
                {
                  //  std::cout << "Element not found in myvector\n";
                    pmut=0;
                }
             // cout<<params[factor]<<endl;
              //pmut = uni(); // generates random a value between 0 and 1 for child1
              //cout<<factor<<" "<<hashmap[factor]<<endl;
              //pmut = hashmap[factor];
              no_states = 1;
              arr_size =1;  
              factor++;
            }
            if(pmut<=pm)
            {
             c1<<line1<<endl;
             //cout<<"No mutation"<<endl;
                //p_c++;       
             //continue;
            }
            if(pmut>pm) //do mutation handling the constraint that the states of the random variable sum to 1.
            {
                
                if(p_c == 0)
                {
                 c1<<line1<<endl;
                }
                if(p_c == 1)
                {
                 ss>>no_factors;
                 //cout<<"no. of factors "<<no_factors<<endl;
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
                  //  cout<< factor_states<<endl;
                 //cout<<"array size "<<arr_size<<endl;
                 //cout<<no_states<<"\t"<<"no. of states "<<no_states<<"\t"<<"total arrays "<<tot_arrays<<"\t"<<"array set "<<arr_set<<"\t"<<"no. arrays "<<no_arr_arrset<<"\t"<<"factor states "<<factor_states<<endl;
                c1<<no_states<<endl;
                }               
                if(p_c == 5)
                {
                    cout<<childfgfile<<"  Mutation happening at "<<(factor-1)<<endl;
                    int k=0;
                   while(k<arr_set)
                    {
//                         //Random values are generated ----~------~-----
//                        std::vector< double * > Arrays1;
//                        for(int l=0; l<no_arr_arrset; l++)
//                        {
//                            Arrays1.push_back( new double[arr_size]);
//                        }
//                        vector<double*>::iterator it;
//                        for(int m=0; m<arr_size; m++)
//                        {
//                            it=Arrays1.begin();
//                            std::vector<double> vec;
//                            for(int i=0; i<no_arr_arrset; i++)
//                            {
//                                vec.push_back(uni());
//                            }
//                            const double total = std::accumulate(vec.begin(), vec.end(), 0.0);
//                            for (double& value: vec)
//                            {
//                                double* arr = *it;
//                                value /= total;
//                                arr[m] = value;
//                                ++it;
//                                //cout<<value <<endl;
//                            }
//                            // cout<<endl;
//                            vec.clear();
//                        }
//                        //Random values are generated---x----x----
                        //Actual values are retrieved ----~------~-----
                        std::vector< double * > Arrays2;
                        for(int lll=0; lll<no_arr_arrset; lll++)
                        {
                            Arrays2.push_back( new double[arr_size]);
                        }
                        vector<double*>::iterator it2;
                        it2=Arrays2.begin();
                        //get the current cpt values and copy them in the array
                        int ll=0;
                        while(ll<no_arr_arrset)
                        {
                            double* arrl = *it2;
                           // cout<<"new arr"<<endl;
                            int ml=0;
                            while(ml<arr_size)
                            {
                                line1 = trim(line1);
                                char_separator<char> sep("   ");
                                tokenizer<char_separator<char>> tokens(line1, sep);
                                std::vector<std::string> valuesl;
                                for (const auto& t : tokens) {
                                 //   cout<<t;
                                    if(t!=" ")
                                    {
                                        valuesl.push_back(t);
                                    //    cout<<"pushed "<<t<<endl;
                                    }
                                }
                                stringstream n1;
                                n1 << valuesl[1];
                                string state = n1.str();
                                n1 >> arrl[ml];
                                ml++;
                                if(ml<arr_size)
                                 getline(p1,line1);
                            } // end of while -m
                            ll++;
                            if(ll<no_arr_arrset)
                                getline(p1,line1);
                            ++it2;
                        } //end of while -l
                        //Actual values are retrieved --------x------x------
                        //Normal Random values are generated ----~------~-----
                        std::vector< double * > Arrays1;
                        vector<double*>::iterator a_it,nor_it;
                        for(int l=0; l<no_arr_arrset; l++)
                        {
                            Arrays1.push_back( new double[arr_size]);
                        }

                        for(int m=0; m<arr_size; m++)
                        {
                            a_it=Arrays2.begin();
                            nor_it=Arrays1.begin();
                            std::vector<double> vec;
                            for(int i=0; i<no_arr_arrset; i++)
                            {
                                double* arr = *a_it;
                                double val = arr[m];
                                double rval = normal();
                                while((val+rval)<0) //boundary condition, negative cpt is not allowed
                                {
                                    rval = normal();
                                }
                                //cout<<"val = "<<val<<" rval ="<<rval<<endl;
                                val = val+rval;
                                ++a_it;
                                vec.push_back(val);
                            }
                            const double total = std::accumulate(vec.begin(), vec.end(), 0.0);
                            for (double& value: vec)
                            {
                                double* nor_arr = *nor_it;
                                value /= total;
                                nor_arr[m] = value;
                                ++nor_it;
                                //cout<<value <<endl;
                            }
                            // cout<<endl;
                            vec.clear();
                        }
                        //Normal Random values are generated---x----x----

                        //Check if actual value or random value should be updated ------~------~-----
                        std::vector< double * > Arrays3;
                        for(int l=0; l<no_arr_arrset; l++)
                        {
                            Arrays3.push_back( new double[arr_size]);
                        }
                        vector<double*>::iterator it3;
                        for(int m=0; m<arr_size; m++)
                        {
                            it3=Arrays3.begin();
                            std::vector<double> vec2;
                            double pmut1 = uni();
                            for(int i=0; i<no_arr_arrset; i++)
                            {
                                if(true)//if(pmut1>pm)
                                    vec2.push_back(1);
                                else
                                    vec2.push_back(0);
                            }
                            for (double& value: vec2)
                            {
                                double* arr_rd = *it3;
                                arr_rd[m] = value;
                                ++it3;
                                //cout<<value <<endl;
                            }
                            // cout<<endl;
                            vec2.clear();
                        }
                        //Done checking-----x--------x--------
                        //Random or actual values will be updated -----~------~
                        vector<double*>::iterator it11,it22,it33;
                        it11=Arrays1.begin();
                        it22=Arrays2.begin();
                        it33=Arrays3.begin();
                        for(int l=0; l<no_arr_arrset; l++)
                        {
                            double* arr = *it11;double* arrl = *it22;double* arr_rd = *it33;
                            for(int m=0; m<arr_size; m++)
                            {
                                stringstream v;
                                if(true)//arr_rd[m]==1
                                {
                                    v << fixed << setprecision(12) << arr[m];
                                    c1<<stat<<"   "<<v.str()<<endl;
                                    stat++;
                                  //  cout<< "replaced"<<v.str()<<endl;
                                }
                                else
                                {
                                    v << fixed << setprecision(12) << arrl[m];
                                    c1<<stat<<"   "<<v.str()<<endl;
                                    stat++;
                                   // cout<<"not replaced "<<v.str()<<endl;
                                }
                            }
                            ++it11;++it22;++it33;
                        }
                        //Random values are updated ------x------x--------
                        Arrays1.clear();
                        Arrays2.clear();
                        Arrays3.clear();
                        k++;
                        if(k<arr_set)
                        {
                            getline(p1,line1);
                        }
                    }// end of while -k
                }// end of if p_c==5
            } // end of if pmut<p_m
            p_c++;
        } // end of while loop
        c1.close();
        p1.close();
        remove(childfgfile);
        *it++;
    } // end of for loop
    //cout<<"Mutation Done"<<endl;
}


void doCrossover()
{
	// compute crossover probability
	 double pcross = uni(); // generates random a value between 0 and 1	
	cout<<"Crossover probability "<<pcross;
	// 4. Select 2 parents randomly
	const int LOW = 0;
	const int HIGH = population_size-1;
	//int t =2;
	//int t=40;
	// Generate the random variables to select as parents where 't' is the number of variables
	/*Declare variable to hold seconds on clock.*/
	time_t seconds;
	/*Get value from system clock and place in seconds variable.*/
	time(&seconds);
	/*Convert seconds to a unsigned integer.*/
	srand(time(0));
	set<int> myset;
	set<int>::iterator it;
	size_t k =0,p=0;
	arrval = new int[population_size];
	string dirname; 
	stringstream p_id; 
	p_id << population_no; 
	dirname = p_id.str();
	string dirname2; 
	stringstream p_id2; 
	p_id2 << population_no+1; 
	dirname2 = p_id2.str();
	string childloc =  alarmlearnedfgloc + dirname2+"/";
	const char* childfgloc = &childloc[0];
	mkdir(childfgloc,0755);
	for( size_t n = 0; n < population_size; n++ )
	{ 
	   int val = rand() % (HIGH - LOW + 1) + LOW;
	   it=myset.find(val);
	   arrval[n] = val;
	   if(it==myset.end()) // if the value is not in the set
	   {
		myset.insert(val);
		p++;
		if(p == 2)
		{
		 cout <<" Random value is"<<arrval[n]<<" and "<< arrval[n-1];
		 string number1; 
		 stringstream num1; 
		 num1 << arrval[n];
		 number1 = num1.str();
		 string parent1 =  alarmlearnedfgloc +"learned/"+dirname+"/"+ networkname+"_"+number1+".fg";
		 string child1 =  childloc+ networkname+"C_"+number1+".fg";
		 const char* parentfgfile1 = &parent1[0];
		 const char* childfgfile1 = &child1[0];
		 string number2; 
		 stringstream num2; 
		 num2 << arrval[n-1]; 
		 number2 = num2.str();
		 string parent2 = alarmlearnedfgloc +"learned/"+dirname+"/"+ networkname+"_"+number2+".fg";
		 string child2 =  childloc+ networkname+"C_"+number2+".fg";
		 const char* parentfgfile2 = &parent2[0];
		 const char* childfgfile2 = &child2[0];	 
		 //cout<<"parent files are "<< parentfgfile1<< " and "<< parentfgfile2<<endl;
		 //cout<<"child files are "<< childfgfile1<<" and "<<childfgfile2<<endl;
	 
		 string line1,line2;
		 ifstream p1 (parentfgfile1);  ifstream p2 (parentfgfile2);
		 ofstream c1 (childfgfile1);  ofstream c2 (childfgfile2);
		// 5. Do crossover and create two children
		// randomly select a crossover point
		 int cr_pt = (rand() % 4) +1;
		 int c = 0; // count of parameters
	 	 int p_c1 = 0, p_c2 =0; // counter to skip the first four config lines in the cpt of the fg file
		 cout<<" Crossover point is "<< cr_pt<<endl;
		 double pmut1=0, pmut2 =0;
		 if (p1.is_open() && p2.is_open())
		 {
			 getline(p1,line1); getline(p2,line2);
			 c1<< line1<<endl;
			 c2<< line2<<endl;					

			 while(getline(p1,line1) && getline(p2,line2))
			 {
				//////cout<<line1 << " "<< line2 <<endl;
				line1 = trim(line1); line2 = trim(line2);
				if(line1.empty())
				{
					c++; 	
					seed();
				 	c1<< line1<<endl;
					c2<< line2<<endl;	
					//cout<<"empty line"<<endl;											 
				}
				else
				{
				 	//cout<<"Line number: "<<p_c1<<" "<<p_c2<<endl;
					//cout<<"Crossover point "<<cr_pt<<"our pt "<<c<<"pcross "<<pcross<<"pc "<<pc<<endl;
					if(c <= cr_pt || pcross <= pc) // checking crossover probability
					{				
				 		c1<< line1<<endl;
						c2<< line2<<endl;					
					}
					else
					{
						//cout<<"crossover happening"<<endl;
						c2<< line1<<endl;
						c1<< line2<<endl;											
					}
				}
			 }
		 }
		 p1.close(); p2.close();
	 	 c1.close(); c2.close(); 
		 p=0;	
		//break;
		}

	   }
	   else
	   {
		n=n-1;
		continue;
	   }   
	}
}

void replacementStrategy()
{
 //int t=2;
 //cout<<"In replacement strategy"<<endl;

 double ll, ll1; 


 string ssiters,ssiters1,ssll1,ssll,ssrunid, ssrunid1;
 //cout<<"declarations done "<<arrval[0]<<endl;
 for( size_t n = 0; n < population_size; n++ )
 {
	int val = arrval[n];
	//cout<<"val = "<<val<<endl;
	vector<EMRun*>::iterator it, it1; 
	int i=0;
	for (it=layer_prev.begin(); it != layer_prev.end(); ++it)
        {    
	     // cout<<"layer size "<<layer.size()<<endl;		   
	      ll = (*it)->getLL();
	      double iters = (*it)->getIters();
	      double runid = (*it)->getRun();
	      stringstream sll;
	      stringstream siters;	 
              stringstream srunid;	      
	      sll<<ll; ssll = sll.str();
	      siters <<iters; ssiters = siters.str();
	      srunid <<runid; ssrunid = srunid.str();
	     // cout<<"runid = "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
	      if(val==runid)
	      {
              //out1<<runid<<" "<<iters<<" ";
 		//cout<<val<<" "<<"runid = "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
		break;
	      }	
	}
        for (it1=layer_curr.begin(); it1 != layer_curr.end(); ++it1)
        {    
	     // cout<<"layer size "<<layer.size()<<endl;		   
	      ll1 = (*it1)->getLL();
	      double iters1 = (*it1)->getIters();
	      double runid1 = (*it1)->getRun();
              stringstream sll1;
              stringstream siters1;	 
              stringstream srunid1;
	      sll1<<ll1; ssll1 = sll1.str();	      
	      siters1 <<iters1; ssiters1 = siters1.str();
	      srunid1 <<runid1; ssrunid1 = srunid1.str();
	     // cout<<"runid = "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
	      if(val==runid1)
	      {
             // out1<<runid1<<" "<<iters1<<" ";
		//cout<<val<<" "<<"runid = "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
		break;
	      }	
	      i++;
	}
	if(ll<=ll1)
	{
		cout<<"Child  has a better LL so leave the child as it is."<<endl;
        //Writing the selected file to 0.log
//        std::vector<Real> llTrace = (*it)->lastlog();
//        
//        for(std::vector<Real>::iterator logval = llTrace.begin(); logval != llTrace.end(); ++logval)
//        {
//            out1 << *logval<<" ";
//        }
     //   out1<<" "<<endl;
       out1<<population_no<<"  "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
	}	
	else
	{
		string dirname; 
		stringstream p_id; 
		p_id << population_no; 
		dirname = p_id.str();
		string dirname1; 
		stringstream p_id1; 
		p_id1 << population_no-1; 
		dirname1 = p_id1.str();
		
		string parentloc =  alarmlearnedfgloc + "learned/"+dirname1+"/"+networkname+"_"+ssrunid+".fg";
		string childloc =  alarmlearnedfgloc + "learned/"+dirname+"/"+networkname+"_"+ssrunid1+".fg";
		//cout<<"Copying files from "<<parentloc<<" to "<<childloc<<endl;
		char *ploc = &parentloc[0];
		char *cloc = &childloc[0];
		cout<<"Replace the child with the parent."<<childloc<<endl;
		remove(cloc);// remove the child file from the child folder inside the learned folder
		ifstream parent(ploc);
		ofstream child(cloc);
		child << parent.rdbuf();// copy the parent file to the child folder location inside the learned folder
		child.close(); parent.close();
		out1<<population_no<<"  "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
		*it1=*it; //replacing the object in the layer_curr vector
        //Writing the selected file to 0.log
//        std::vector<Real> llTrace = (*it1)->lastlog();
//        for(std::vector<Real>::iterator logval = llTrace.begin(); logval != llTrace.end(); ++logval)
//        {
//            out1 << *logval<<" ";
//        }
//        out1<<" "<<endl;
        
	}
     
 }
 
}
bool myfn(int i, int j) { return i<j; }

int indexofSmallestElement(double array[], int size)
{
    int index = 0 ;
    double n = array[0] ;
    for (int i = 1; i < size; ++i)
    {
        if (array[i] < n)
        {
            n = array[i] ;
            index = i ;
        }
    }
    return index;
}

int main(int argc, char *argv[]) {
generator.seed(static_cast<unsigned int>(std::time(0)));
const char* fname =argv[2];
const char* fname1 =argv[6];
gibbsfile = argv[1];
emfile = argv[8];
stringstream ss9( argv[9] );
ss9 << noVars;
istringstream ss( argv[3] );
ss >> pm;
stringstream ss1( argv[4] );
ss1 >> pc;
stringstream ss2(argv[5]);
alarmfgloc = ss2.str();
alarmlearnedfgloc = ss2.str();
networkname = argv[7];
out.open(fname);
out1.open(fname1);
time_t timer1, timer2;
time(&timer1);
clock_t tStart = clock();
while(population_no<max_gen)
{
	startEM();
	checkEMforTermination(); 
	cout<<"population no"<<population_no<<endl;
	if(population_no > 0)
	{
	  replacementStrategy();
	}
	doCrossover();
	doMutation();
	//cout<<"Next population"<<endl;
	population_no=population_no+1;
    int nocompare_gens = 4;
//    if(population_no>=nocompare_gens)
//    {
//      
//            for (int j=0; j<population_size; j++) {
//                string number1;
//                stringstream n_id1;
//                n_id1<<j;
//                number1 = n_id1.str();
//                string dirname1;
//                stringstream p_id1;
//                p_id1<<population_no;
//                dirname1 = p_id1.str();
//                string fglearnedfileloc =  alarmlearnedfgloc + dirname1+"/"+networkname+"_"+number1+".fg";
//                int n =nocompare_gens*population_size;
//                double* distance = new double[n];
//                int p=0;
//                for (int i=0; i<nocompare_gens; i++)
//                {
//                    string dirname;
//                    stringstream p_id;
//                    p_id << i;
//                    p_id1<<population_no;
//                    dirname = p_id.str();
//                    
//                for (int k=0; k<population_size; k++) {
//                    string number;
//                    stringstream n_id;
//                    n_id<<k;
//                    number = n_id.str();
//                    string fgfileloc =  alarmfgloc +dirname+"/"+networkname+"_"+number+".fg";
//                    distance[p] = calculateDistance(fgfileloc,fglearnedfileloc);
//                     out1<<distance[p]<<" ";
//                    cout<< dirname1+"/"+networkname+"_"+number1+".fg and "<<dirname+"/"+networkname+"_"+number+".fg "<<distance[p]<<endl;
//                     p++;
//                }
//            }
//                int index = indexofSmallestElement(distance,n);
//                int dir = index/population_size;
//                int fg = index % population_size;
//                //cout<<"index "<<index;
//                string sdir,sfg;
//                stringstream ssdir,ssfg;
//                ssdir<<dir; ssfg<<fg;
//                sdir = ssdir.str();sfg = ssfg.str();
//                string neighbor =  sdir+"/"+networkname+"_"+sfg+".fg";
//                out1<<"nearest neighbor of "<<dirname1+"/"+networkname+"_"+number1+".fg"<<" is "<<neighbor << " "<<distance[index]<<endl;
//                cout<<" nearest neighbor of "<<dirname1+"/"+networkname+"_"+number1+".fg"<<" is "<<neighbor << " "<<distance[index]<<endl;
//        }
//    }
//    out1<<endl;
    
}
time(&timer2);
double t = difftime(timer2,timer1);
double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
out<<"Time taken: "<<t<<"s"<<endl;
out<<" Processor Time taken: "<<t_c<<"s"<<endl;
out.close();
out1.close();
return 0;
}

