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

#include <iomanip>      // std::setprecision
#include <sstream>
#include <string>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace dai;

// GA - initialization
size_t max_gen = 25; // no. of generations
size_t population_size =20; // population size
Real pc;// = 0.8; // crossover probability - between 0.6 an 0.9
Real pm;// = 0.3; // mutation probability - between 1/pop.size and 1/chromosome_length
int population_no=0; // current population id

// 1. Creating initial population of EM runs 
	
char* gibbsfile;// = "/home/priya/libDAI-0.3.1/alarm/500/5.tab";
//const char* gibbsfile = argv[1];
string alarmfgloc;// = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/"; 
string alarmlearnedfgloc;// = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/";
const char* networkname = "alarm";
size_t maxIters =1000; // maximum number of iterations

ofstream out,out1;
EMRun* runname;
vector <EMRun*> layer; 
vector <EMRun*> layer_curr;
vector <EMRun*> layer_prev;
int *arrval;
//
// Generate a random number between 0 and 1
// return a uniform number in [0,1].
double unifRand()
{
    return rand() / double(RAND_MAX);
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
     srand((time.tv_sec * 1000) + (time.tv_usec / 10));
}

std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	return s;
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
	 string fgfileloc =  alarmfgloc + dirname+"/"+"alarm_"+number+".fg";
	 const char* fgfile = &fgfileloc[0];
	 //cout<<"Starting EM "<<fgfileloc<<endl;
	 runname = new EMRun(networkname, i, fgfile, gibbsfile); // args are -1.network name 2.factor-graph number 3. evidencefile

	 // 2. Run EM
	 boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters)); // args - max iters 
	 layer.push_back(runname); 
	 sleep(1);
	 // 4. Next individual
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
	   // cout<<"in while loop "<<layer.size()<<endl;
	    vector<EMRun*>::iterator it; 
	    for (it=layer.begin(); it != layer.end(); ++it){    
	     // cout<<"layer size "<<layer.size()<<endl;	
	      if((*it)->hasTerminated()) { //if it has terminated, set the run aside	   
		double ll = (*it)->getLL();
		double iters = (*it)->getIters();
		double runid = (*it)->getRun();
		string number; 
		stringstream num; 
		num << runid; 
		number = num.str();
	 	string fglearnedfileloc =  alarmlearnedfgloc + "learned/"+dirname+"/alarm_"+number+".fg";
	    	const char* fglearnedfile = &fglearnedfileloc[0];
		//cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		// 3. Store the learned factor graphs
		InfAlg* inf = (*it)->getInf();
		ofstream learnedstream;
		//cout<<"learned file = "<<fglearnedfile<<endl;
		learnedstream.open(fglearnedfile);
		learnedstream.precision(12);
		learnedstream << inf->fg();  
		learnedstream.flush();
		learnedstream.close();
		delete inf;
		sleep(1);
		//cout<<"Run number "<<number<<" has terminated"<<endl;
		out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
		layer_curr.push_back(*it); 
		layer.erase(it);	
		if (layer.size() == 0){break;}
		else { it = layer.begin(); }
	      }
	      else if(((*it)->getIters()) == maxIters){ //we stop the run
		double ll = (*it)->getLL();
		double iters = (*it)->getIters();
		double runid = (*it)->getRun();
		string number; 
		stringstream num; 
		num << runid; 
		number = num.str();
	 	string fglearnedfileloc =  alarmlearnedfgloc + "learned/"+dirname+"/alarm_"+number+".fg";
	    	const char* fglearnedfile = &fglearnedfileloc[0];
		//cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		//cout<<"Run "<<(*it)->getRun()<<" has reached maximum iterations, so stopping the run"<<endl;
		boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(*it))); 
	 	// 3. Store the learned factor graphs
		InfAlg *inf = (*it)->getInf();
	    	ofstream learnedstream;
	    	//cout<<"learned file = "<<fglearnedfile<<endl;
	    	learnedstream.open(fglearnedfile);
	    	learnedstream.precision(12);
	    	learnedstream << inf->fg();  
	    	learnedstream.close();
		delete inf;
		sleep(1);		
		//cout<<"Run number "<<number<<" has reached maximum iterations so stopped"<<endl;
		out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
		layer_curr.push_back(*it); 
		layer.erase(it);
	//	if (layer.size() == 0){break;}
	//	else { it = layer.begin(); }
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

void doRandomSearch() // open each child file and apply mutation if required
{   
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
    int t=20;
    arrval = new int[t];
    //cout<<"In Mutation"<<endl;
    //cout<<"declarations done "<<arrval[0]<<endl;
    for( size_t n = 0; n < t; n++ )
    {
        string number1;
        stringstream num1;
        num1 << n;//arrval[n];
	arrval[n] = n;
        number1 = num1.str();
        //string child =  childloc+ "alarmC_"+number1+".fg";
        string child1 =  childloc+ "alarm_"+number1+".fg";
	string child =  alarmlearnedfgloc +"learned/"+dirname+"/"+ "alarm_"+number1+".fg";
        const char* childfgfile = &child[0];         
        const char* childfgfile1 = &child1[0];         
       // cout<<"child file "<< childfgfile1<<endl;  
	mkdir(childfgloc,0755);   
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
        while(getline(p1,line1))
        {
            line1 = trim(line1);
            //cout<<p_c<<" "<<pmut<<" "<<pm<<" "<<line1<<endl;
            stringstream ss(line1);           
            if(line1.empty())
            {
              p_c = 0;
              stat =0;
              //cout<<"factor variable "<<factor<<endl;
              seed();
              pmut = unifRand(); // generates random a value between 0 and 1 for child1
              //cout<<"Mutation probability "<<pmut<<endl;         
              no_states = 1;
              arr_size =1;
              factor++;
            }
            //if(pmut>pm)
            //{
             //c1<<line1<<endl;
             //cout<<"No mutation"<<endl;
                //p_c++;       
             //continue;
            //}
            //if(pmut<=pm) //do mutation handling the constraint that the states of the random variable sum to 1.
            {
                //cout<<"Mutation happening "<<endl;
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
                 //cout<<"array size "<<arr_size<<endl;
                 //cout<<no_states<<"\t"<<"no. of states "<<no_states<<"\t"<<"total arrays "<<tot_arrays<<"\t"<<"array set "<<arr_set<<"\t"<<"no. arrays "<<no_arr_arrset<<"\t"<<"factor states "<<factor_states<<endl;
                c1<<no_states<<endl;
                }               
                if(p_c >= 5)
                {
                int k=0;                 
                 while(k<arr_set)
                  {
                    std::vector< double * > Arrays;
                           for(int l=0; l<no_arr_arrset; l++)
                    {
                     Arrays.push_back( new double[arr_size]);
                    }
                    vector<double*>::iterator it2;
                    it2=Arrays.begin();
                    //get the current cpt values and copy them in the array
                    int l=0;
                    while(l<no_arr_arrset)
                    {
                        double* arr = *it2;
                        //cout<<"new arr"<<endl;
                        int m=0;
                        while(m<arr_size)
                        {                         
                         line1 = trim(line1);
                         std::vector<std::string> values;
                          values = tokenizeString( line1, true,"  ");
                         int ii=0; ;
                         for( size_t j = 0; j < values.size(); ++j )
                         {
                          stringstream n1;
                          n1 << values[j];
                          string state = n1.str();      
                          if( state.find_first_not_of(" ")!= std::string::npos)        
                          {                          
                           if(ii==1)
                           {
                           n1 >> arr[m];
                           //cout<<p_c<<" "<<line1<<" "<<"arrval = "<<values[0]<<"\tarr[m]"<<arr[m]<<" m = "<<m<<endl;
                           m++;                          
                           if(m<arr_size)
                            getline(p1,line1);
                           ii=0;
                           }
                           else
                            ii=1;   
                          } //end of if
                         } // end of for
                        } // end of while -m
                        l++;
                        if(l<no_arr_arrset)
                         getline(p1,line1);
                        ++it2;
                    } //end of while -l
                    vector<double*>::iterator it;
                    double sumarr[arr_size];                                       
                    for(int m=0; m<arr_size; m++)
                    {
                        it=Arrays.begin();
                        double sum =0;                                       
                        for(int l=0; l<no_arr_arrset; l++) // stdnormal rand values are generated and sum is found
                        {
                          seed();
                          double* arr = *it;
                          double val = arr[m];
                          double rval = rnd_stdnormal();
                          while((val+rval)<0) //boundary condition, negative cpt is not allowed
                          {
                           rval = rnd_stdnormal();
                          }
                          //cout<<"val = "<<val<<" rval ="<<rval<<endl;
                          val = val+rval;
                          //cout<<"mutation = "<<val<<endl;
                          /*if(l == no_arr_arrset-1)
                            val = 1-sum;
                          else
                            val = unifRand()/(arr_set+4);*/                     
                          arr[m] = val;   
                          sum = sum +val;   
                          ++it;
                        }
                        sumarr[m] =sum;
                        //cout<<"sum = "<<sum<<endl;
                    }
                    vector<double*>::iterator it3;                                   
                    for(int m=0; m<arr_size; m++)
                    {
                        it3=Arrays.begin();
                        double sum =0;                                       
                        for(int l=0; l<no_arr_arrset; l++) // the rand values are normalized so that the sum is 1
                        {
                          seed();
                          double* arr = *it3;
                          double val;
                          val = arr[m];
                          /*if(l == no_arr_arrset-1)
                            val = 1-sum;
                          else
                            val = unifRand()/(arr_set+4);*/                                               
                          arr[m] = val/sumarr[m];
                          //cout<<"mutation val = "<<arr[m]<<endl;       
                          ++it3;
                        }
                        //cout<<endl;
                    }
                    vector<double*>::iterator it1;
                    it1=Arrays.begin();       
                    for(int l=0; l<no_arr_arrset; l++)
                    {
                        double* arr = *it1;
                        for(int m=0; m<arr_size; m++)
                        {
                         stringstream v;
                         v << fixed << setprecision(15) << arr[m];
                         c1<<stat<<"\t"<<v.str()<<endl;
                         //cout<<stat<<"\t"<<v.str()<<endl;
                         stat++;
                        }
                        ++it1;
                    }
                  k++;
                  if(k<arr_set)   
                    getline(p1,line1);
                  }// end of while -k
                }// end of if p_c>=5
            } // end of if pmut<p_m
            p_c++;
        } // end of while loop
        c1.close();
        p1.close();
      //  remove(childfgfile);   
    } // end of for loop
    cout<<"Random searach Done"<<endl;
}

void replacementStrategy()
{
 int t=20;
 cout<<"In replacement strategy"<<endl;

 double ll, ll1; 
 string ssiters,ssiters1,ssll1,ssll,ssrunid, ssrunid1;
 //cout<<"declarations done "<<arrval[0]<<endl;
 for( size_t n = 0; n < t; n++ )
 {
	int val = arrval[n];
	//cout<<"val = "<<val<<endl;
	vector<EMRun*>::iterator it, it1; 
	int i=0;
	//cout<<"layer size = "<<layer_prev.size()<<endl;
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
		//cout<<val<<" "<<"runid = "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
		break;
	      }	
	      i++;
	}
	if(ll<=ll1)
	{
		//cout<<"Child  has a better LL so leave the child as it is."<<endl;
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
		
		string parentloc =  alarmlearnedfgloc + "learned/"+dirname1+"/"+"alarm_"+ssrunid+".fg";
		string childloc =  alarmlearnedfgloc + "learned/"+dirname+"/"+"alarm_"+ssrunid1+".fg";
		//cout<<"Copying files from "<<parentloc<<" to "<<childloc<<endl;
		char *ploc = &parentloc[0];
		char *cloc = &childloc[0];
		//cout<<"Replace the child with the parent."<<childloc<<endl;
		remove(cloc);// remove the child file from the child folder inside the learned folder
		ifstream parent(ploc);
		ofstream child(cloc);
		child << parent.rdbuf();// copy the parent file to the child folder location inside the learned folder
		child.close(); parent.close();
		out1<<population_no<<"  "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
		*it1=*it; //replacing the object in the layer_curr vector
	}
 } 
 
}

int main(int argc, char *argv[]) {
const char* fname =argv[2];
const char* fname1 =argv[6];
gibbsfile = argv[1]; 
istringstream ss( argv[3] );
ss >> pm;
stringstream ss1( argv[4] );
ss1 >> pc;
stringstream ss2(argv[5]);
alarmfgloc = ss2.str();
alarmlearnedfgloc = ss2.str();

out.open(fname);
out1.open(fname1);
time_t timer1, timer2;
time(&timer1);
clock_t tStart = clock();
while(population_no<max_gen)
{
	startEM();
	checkEMforTermination(); 
	//cout<<"population no"<<population_no<<endl;
	if(population_no > 0)
	{
	  replacementStrategy();
	}
	doRandomSearch();
	//doCrossover();
	//doMutation();
	//cout<<"Next population"<<endl;
	population_no=population_no+1;
}
time(&timer2);
double t = difftime(timer2,timer1);
(clock() - tStart)/CLOCKS_PER_SEC;
double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
out<<"Time taken: "<<t<<"s"<<endl;
out<<" Processor Time taken: "<<t_c<<"s"<<endl;
out.close();
return 0;
}

