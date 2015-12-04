/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2010  Joris Mooij  [joris dot mooij at libdai dot org]
 */

#include <dai/alldai.h>
#include <fstream>
#include "dai/emrun.h"
#include <dai/util.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <iomanip>      // std::setprecision
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include<math.h>
#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/config.hpp>
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
size_t max_gen = 5; // no. of generations
size_t population_size =4; // population size
Real pc;// = 0.8; // crossover probability - between 0.6 an 0.9
Real pm;// = 0.3; // mutation probability - between 1/pop.size and 1/chromosome_length
int population_no=0; // current population id

// 1. Creating initial population of EM runs 
string alarmfgloc;
char* gibbsfile;// = "/home/priya/libDAI-0.3.1/alarm/500/5.tab";
char* networkname ; //= "alarm";
size_t maxIters =1000; // maximum number of iterations
char* emfile;
ofstream out;
EMRun* runname;
vector <EMRun*> layer; 
vector <EMRun*> layer_curr;
vector <EMRun*> layer_prev;
vector <EMRun*> layer_parent;
dai::hash_map<int, FactorGraph> hashmap;
int *arrval;
//
// Generate a random number between 0 and 1
// return a uniform number in [0,1].
double unifRand()
{
    return rand()/double(RAND_MAX);
}
void initializeEM()
{
    size_t i =0,j=0;
	cout<<"Initial start EM"<<endl;
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
        string fgfileloc =  alarmfgloc+"/"+networkname+"_"+number+".fg";
        const char* fgfile = &fgfileloc[0];
        FactorGraph fg;
        fg.ReadFromFile(fgfile);
        runname = new EMRun(networkname, i, fg, gibbsfile,emfile);
        // 2. Run EM
        boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters));
        layer.push_back(runname);
        sleep(1);
        // 4. Next individual
        cout<<" run "<<i<<" started"<<endl;
        i=i+1;
    }
}
void printFactors()
{
    vector<EMRun*>::iterator it1;
    for (int i=0;i<population_size;i++)
    {
       FactorGraph fg = hashmap[i];
    cout<<"------ Printing ------"<<i<<endl;
    for( size_t I = 0; I < fg.nrFactors(); I++ )
    {
        Factor P = fg.factor( I );
        dai::TProb<double> myvector = P.p();
        for (dai::TProb<double>::iterator it = myvector.begin();it != myvector.end(); ++it)
            std::cout << ' ' << *it;
        std::cout << '\n';
    }
    }
}
void startEM()
{
   // printFactors();
    for (int i=0;i<population_size;i++)
    {
        FactorGraph fg = hashmap[i];
        double runid = i;
        cout<<"Starting EM "<<runid<<endl;
        runname = new EMRun(networkname, i, fg, gibbsfile,emfile);
        // 2. Run EM
        boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters));
        layer.push_back(runname);
        sleep(1);
	}

}
std::vector<EMRun*>::iterator compareRuns(std::vector<EMRun*>::iterator *it)
{
    if(layer_prev.size() !=0 )
      {
        cout<<" iters = "<<(**it)->getIters()<<endl;
        double iters = (**it)->getIters();
        double val = (**it)->getRun();
        cout<<" em run stopped "<<val<<endl;
        double ll = (**it)->getLL();
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
       // cout<<" LL for prev run = "<<llprev<<endl;
        if(ll>=llprev)
        {
            cout<<" current LL "<<ll<<" is good"<<endl;
            boost::thread thread7(boost::bind(&EMRun::resumeEM, boost::ref(**it)));
            cout<<" em run resumed"<<val<<endl;
        }
        else
        {
            cout<<" current LL "<<ll<<" is not good"<<endl;
            **it = *itprev;
            (*itprev)->forceStopped();
            cout<<" em run not resumed"<<val<<endl;
        }
        
    }
    return *it;
    ////////////
}
void checkEMforTermination()
{
    layer_prev.assign(layer_curr.begin(), layer_curr.end());
	layer_curr.clear();
    int i=0;
	while (true){
	    //cout<<"in while loop "<<layer.size()<<endl;
        i++;
	    vector<EMRun*>::iterator it;
	    for (it=layer.begin(); it != layer.end(); ++it){
//            if((*it)->getIters() > 2*i)
//            it = compareRuns(&it);
//            cout<<" iters = "<<(*it)->getIters()<<endl;
//
	      //cout<<"layer size "<<layer.size()<<endl;
	      if((*it)->hasTerminated()) { //if it has terminated, set the run aside
      
              double ll = (*it)->getLL();
              double iters = (*it)->getIters();
              double runid = (*it)->getRun();
              //cout<<"em run "<<runid<<" terminated "<<iters<<endl;
              std::vector<Real> llTrace = (*it)->lastlog();
              cout<<"Run number "<<runid<<" ll = "<<ll<<" iters = "<<iters<<endl;
              InfAlg* inf1 = (*it)->getInf();
              FactorGraph fg1 = inf1->fg();
              //cout<<fg1<<endl;
              sleep(1);
              out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl;
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
            sleep(1);		
            //cout<<"Run number "<<number<<" has reached maximum iterations so stopped"<<endl;
            //out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl;
		    layer_curr.push_back(*it);
            if(!(*it)->isForceStop())
            {
                boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(*it)));
                
            }
            layer.erase(it);
            //	if (layer.size() == 0){break;}
            //	else { it = layer.begin(); }
	      }
            
	 }
	 if (layer.size() == 0){ //cout<<"breaking while"<<endl;
		break; }
	 else { //cout<<"iterating"<<endl;
		}
	  sleep(1); 
	}
    layer_parent.assign(layer_curr.begin(), layer_curr.end());
}

void doMutation() // open each child file and apply mutation if required
{
    //cout<<"Mutation Begins"<<endl;
    double pmut=0;
    vector<EMRun*>::iterator it_C;
    //printFactors();
    for (it_C=layer_parent.begin(); it_C != layer_parent.end(); ++it_C){
        double runid = (*it_C)->getRun();
        InfAlg* inf = (*it_C)->getInf();
        FactorGraph fg = hashmap[runid];//inf->fg();
        std::vector<int> mutateFactors;
        mutateFactors.clear();
        mutateFactors = (*it_C)->getmutateFactors();
        int noMutations =0;
        //cout<<" Run id "<<runid;
        //cout<<"______"<<mutateFactors.size()<<endl;
        cout<<"Mutation at ";
        for( size_t I = 0; I < fg.nrFactors(); I++ ) 
            {
//                std::vector<int>::iterator it;
//                it = find (mutateFactors.begin(), mutateFactors.end(), I);
//                if (it != mutateFactors.end())
//                {
//                    //std::cout << "Element found in myvector: " << *it << '\n';
//                    pmut=1;
//                }
//                else
//                {
//                    //  std::cout << "Element not found in myvector\n";
//                    pmut=0;
//                }
                pmut = uni(); // generates random a value between 0 and 1 for child1
                //cout<<pmut<<" "<<endl;
                int no_states = fg.factor( I ).nrStates();// total no. of states
                int no_arr_arrset =0; // factor_states
                int arr_size =1; //states of parents before factor_pos
                int arr_set =0; //states of parents after factor_pos
                int arr_set_states =0; // no. of states till factor_pos

                //cout<<fg.factor( I ).nrStates()<<endl;
                Factor P = fg.factor( I );
                VarSet vs = P.vars();
//                cout<<"vars = "<<vs<<endl;
//                cout << "{";
                for( VarSet::const_iterator v = vs.begin(); v != vs.end(); v++ )
                {
                   // cout << (v != vs.begin() ? ", " : "") << *v;
                    if(v->label()<I)
                        arr_size = arr_size * v->states();
                    if(v->label() == I)
                    {
                       // cout<<" states = "<<v->states()<<endl;
                        no_arr_arrset = v->states();
                        arr_set_states = arr_size * v->states();
                        break;
                    }
                }
                //cout << "}"<<endl;
                arr_set = no_states/arr_set_states;
                //cout<<"no. of states "<<no_states<<"\t"<<"array set "<<arr_set<<"\t"<<"arr_set_states "<<arr_set_states<<"\t"<<"no_arr_arrset "<<no_arr_arrset<<"\t"<<"array size "<<arr_size<<endl;

                if(pmut>pm) //do mutation handling the constraint that the states of the random variable sum to 1.
                {
                noMutations++;
                cout<<" "<<I;
                int k=0;
                while(k<arr_set)
                {
                    //Random values are generated---~----~----
                    std::vector< double * > Arrays1;
                    for(int l=0; l<no_arr_arrset; l++)
                    {
                        Arrays1.push_back( new double[arr_size]);
                    }
                    vector<double*>::iterator it;
                    for(int m=0; m<arr_size; m++)
                    {
                        it=Arrays1.begin();
                        std::vector<double> vec;
                        for(int i=0; i<no_arr_arrset; i++)
                        {
                            vec.push_back(uni());
                        }
                        const double total = std::accumulate(vec.begin(), vec.end(), 0.0);
                        for (double& value: vec)
                        {
                            double* arr = *it;
                            value /= total;
                            arr[m] = value;
                            ++it;
                           // cout<<value <<endl;
                        }
                        // cout<<endl;
                        vec.clear();
                    }
                    //Random values are generated---x----x----
                    //Random or actual values will be updated -----~------~
                    vector<double*>::iterator it11;
                    it11=Arrays1.begin();
                    int stat=0;
                    for(int l=0; l<no_arr_arrset; l++)
                    {
                        double* arr = *it11;
                        for(int m=0; m<arr_size; m++)
                        {
                            stringstream v;
                            v << fixed << setprecision(12) << arr[m];
                            //c1<<stat<<"   "<<v.str()<<endl;
                            //cout<< "replaced"<<v.str()<<endl;
                            P.set(stat,arr[m]);
                            stat++;
                        }
                        ++it11;
                    }
                    //Random values are updated ------x------x--------
                    k++;
                } // end of while
                } // end of if
                fg.setFactor(I,P);
            } // end of factors
        cout<<" Total mutations "<<noMutations<<" runid "<<runid<<endl;
        hashmap[runid]=fg;
    } // end of factorgraphs
     //printFactors();
    //cout<<"Mutation Done"<<endl;
}

void doCrossover()
{
    //cout<<"Crossover Begins"<<endl;
	// compute crossover probability
    double pcross = uni(); // generates random a value between 0 and 1
	//cout<<"Crossover probability "<<pcross <<endl;
	// 4. Select 2 parents randomly
	const int LOW = 0;
	const int HIGH = population_size-1;
    set<int> myset;
	set<int>::iterator it;
	size_t p=0;
	arrval = new int[population_size];
    /*Declare variable to hold seconds on clock.*/
	time_t seconds;
	/*Get value from system clock and place in seconds variable.*/
	time(&seconds);
	/*Convert seconds to a unsigned integer.*/
	srand(time(0));
   
    for (int n=0;n<population_size;n++)
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
               // cout <<" Random value is"<<arrval[n]<<" and "<< arrval[n-1];
                EMRun* r1 = layer_parent.at(n);
                EMRun* r2 = layer_parent.at(n-1);
                double runid = (r1)->getRun();
                InfAlg* inf = (r1)->getInf();
                FactorGraph fg = inf->fg();
                double runid1 = (r2)->getRun();
                InfAlg* inf1 = (r2)->getInf();
                FactorGraph fg1 = inf1->fg();
                // randomly select a crossover point
                int cr_pt = (rand() % fg.nrVars());
               // cout<<" Crossover point is "<< cr_pt<<endl;
                p=0;
                if(pcross>pc)
                {
                for( size_t I = cr_pt; I < fg.nrFactors(); I++ )
                {
                    Factor P = fg.factor( I );
                    dai::TProb<double> myvector = P.p();
                    Factor P1 = fg1.factor( I );
                    dai::TProb<double> myvector1 = P1.p();
                    dai::TProb<double>::iterator it1 = myvector1.begin();
                    dai::TProb<double>::iterator it = myvector.begin();
                    for (int state=0;state<myvector.size();state++)
                    {
                        P.set(state,*it1);
                        P1.set(state,*it);
                        ++it1;++it;
                    }
                    fg.setFactor(I,P);
                    fg1.setFactor(I,P1);
                }
                }
                int val = arrval[n];int val1 = arrval[n-1];
                hashmap[val]=fg;
                hashmap[val1]=fg1;
            }
        }
        else
        {
            n=n-1;
            continue;
        }
    }
    //cout<<"Crossover Done"<<endl;
    //printFactors();

}

void replacementStrategy()
{
 //cout<<"In replacement strategy"<<endl;
 double ll, ll1;
 string ssiters,ssiters1,ssll1,ssll,ssrunid, ssrunid1;
 layer_parent.clear();
 for( size_t n = 0; n < population_size; n++ )
 {
	int val = arrval[n];
	//cout<<"val = "<<val<<endl;
	vector<EMRun*>::iterator it, it1; 
	int i=0;
	for (it=layer_prev.begin(); it != layer_prev.end(); ++it)
    {    	   
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
            //  cout<<val<<" "<<"runid = "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
              break;
	      }	
	}
    for (it1=layer_curr.begin(); it1 != layer_curr.end(); ++it1)
    {    	   
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
             // cout<<val<<" "<<"runid = "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
              break;
	      }	
	      i++;
	}
	if(ll<=ll1)
	{
		//cout<<"Child  has a better LL so leave the child as it is."<<endl;
        out<<population_no<<"  "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
	}	
	else
	{
       // cout<<"Replace child with parent."<<endl;
        out<<population_no<<"  "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
		*it1=*it; //replacing the object in the layer_curr vector
        
	}
  }
  layer_parent.assign(layer_curr.begin(), layer_curr.end());

}

int main(int argc, char *argv[]) {
    generator.seed(static_cast<unsigned int>(std::time(0)));
 
    gibbsfile = argv[1];
    stringstream ss2(argv[2]);
    alarmfgloc = ss2.str();
    istringstream ss( argv[3] );
    ss >> pm;
    stringstream ss1( argv[4] );
    ss1 >> pc;
    const char* fname =argv[5];
    networkname = argv[6];
    emfile = argv[7];
    out.open(fname);

    time_t timer1, timer2;
    time(&timer1);
    clock_t tStart = clock();
    
    initializeEM();
    
    cout<<"population no"<<population_no<<endl;
    while(population_no<max_gen)
    {
          checkEMforTermination();
         if(population_no>0)
            replacementStrategy();
          doCrossover();
          doMutation();
          startEM();
          population_no=population_no+1;
          cout<<"population no"<<population_no<<endl;
   }
 
    time(&timer2);
    double t = difftime(timer2,timer1);
    double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
    out<<"Time taken: "<<t<<"s"<<endl;
    out<<" Processor Time taken: "<<t_c<<"s"<<endl;
    out.close();
    return 0;
}

