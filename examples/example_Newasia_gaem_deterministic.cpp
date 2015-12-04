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


#include <sstream>
#include <string>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace dai;

// GA - initialization
size_t max_gen = 5; // no. of generations
size_t population_size = 10;//40; // population size
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

ofstream out;
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
	    cout<<"in while loop "<<layer.size()<<endl;
	    vector<EMRun*>::iterator it; 
	    for (it=layer.begin(); it != layer.end(); ++it){    
	      cout<<"layer size "<<layer.size()<<endl;	
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
		cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		// 3. Store the learned factor graphs
		InfAlg* inf = (*it)->getInf();
		ofstream learnedstream;
		cout<<"learned file = "<<fglearnedfile<<endl;
		learnedstream.open(fglearnedfile);
		learnedstream.precision(12);
		learnedstream << inf->fg();  
		learnedstream.flush();
		learnedstream.close();
		delete inf;
		sleep(1);
		cout<<"Run number "<<number<<" has terminated"<<endl;
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
		cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		cout<<"Run "<<(*it)->getRun()<<" has reached maximum iterations, so stopping the run"<<endl;
		boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(*it))); 
	 	// 3. Store the learned factor graphs
		InfAlg *inf = (*it)->getInf();
	    	ofstream learnedstream;
	    	cout<<"learned file = "<<fglearnedfile<<endl;
	    	learnedstream.open(fglearnedfile);
	    	learnedstream.precision(12);
	    	learnedstream << inf->fg();  
	    	learnedstream.close();
		delete inf;
		sleep(1);		
		cout<<"Run number "<<number<<" has reached maximum iterations so stopped"<<endl;
		out<<pc<<" "<<pm<<" "<<gibbsfile<<" "<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
		layer_curr.push_back(*it); 
		layer.erase(it);
	//	if (layer.size() == 0){break;}
	//	else { it = layer.begin(); }
	      }
	      cout<<"Going .."<<endl;     
	 }
	 if (layer.size() == 0){ cout<<"breaking while"<<endl; break; }
	 else { cout<<"iterating"<<endl;}
	  sleep(1); 
	}
}
std::string doMutation(string line)
{	
	seed(); 
	std::vector<std::string> fields;
	fields = tokenizeString( line, true,"  ");
	string state_line, param_line;
	stringstream v;
	double val = unifRand();
	v << val; // a random number is generated
	for( size_t i = 0; i < fields.size(); ++i ) {
	   stringstream n;
	   n << fields[i]; 
	   string s = n.str();				   
	   if( s.find_first_not_of(" ")!= std::string::npos){
	 //	cout << " val - " <<s<<endl;
		if(i == 0)
		state_line =s; // this is the index
	  }
	}
	line = state_line+"   "+v.str(); // combining the index and the random value
	return line; // return the new cpt value
}
void doCrossoverandMutation()
{
	// compute crossover probability
	 seed();
	 double pcross = unifRand(); // generates random a value between 0 and 1	
	// 4. Select 2 parents randomly
	const int LOW = 0;
	const int HIGH = 9;//39;
	int t =10; 
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
	arrval = new int[t];
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
	for( size_t n = 0; n < t; n++ )
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
		 cout <<"Random value is"<<arrval[n]<<" and "<< arrval[n-1]<<endl;
		 string number1; 
		 stringstream num1; 
		 num1 << arrval[n]; 
		 number1 = num1.str();
		 string parent1 =  alarmlearnedfgloc +"learned/"+dirname+"/"+ "alarm_"+number1+".fg";
		 string child1 =  childloc+ "alarm_"+number1+".fg";
		 const char* parentfgfile1 = &parent1[0];
		 const char* childfgfile1 = &child1[0];
		 string number2; 
		 stringstream num2; 
		 num2 << arrval[n-1]; 
		 number2 = num2.str();
		 string parent2 = alarmlearnedfgloc +"learned/"+dirname+"/"+ "alarm_"+number2+".fg";
		 string child2 =  childloc+ "alarm_"+number2+".fg";
		 const char* parentfgfile2 = &parent2[0];
		 const char* childfgfile2 = &child2[0];	 
		 cout<<"parent files are "<< parentfgfile1<< " and "<< parentfgfile2<<endl;
		 cout<<"child files are "<< childfgfile1<<" and "<<childfgfile2<<endl;
	 
		 string line1,line2;
		 ifstream p1 (parentfgfile1);  ifstream p2 (parentfgfile2);
		 ofstream c1 (childfgfile1);  ofstream c2 (childfgfile2);
		// 5. Do crossover and create two children
		// randomly select a crossover point
		 int cr_pt = rand() % 37;
		 int c = 0; // count of parameters
	 	 int p_c = 0; // counter to skip the first four config lines in the cpt of the fg file
		 cout<<"Crossover point is "<< cr_pt<<endl;
		 double pmut=0;
		 if (p1.is_open() && p2.is_open())
		 {
			 while(getline(p1,line1) && getline(p2,line2))
			 {
				//cout<<line1 << " "<< line2 <<endl;
				line1 = trim(line1); line2 = trim(line2);
				if(line1.empty())
				{
				 c++; p_c = 0;	
				 seed();
				 pmut = unifRand(); // generates random a value between 0 and 1	
                                 cout<<"Mutation probability "<<pmut<<endl;				 
				}
				if(pmut <= pm) // checking mutation probability
				{
				 p_c++;
				}
				if(c <= cr_pt || pcross >= pc) // checking crossover probability
				{
					if(p_c >= 6)
				        {
					//cout<<"Mutation is done at "<<c<<endl;
					//cout<<line1<<endl;
					//cout<<line2<<endl;
					line1 = doMutation(line1);
					line2 = doMutation(line2);
					//cout<<"After mutation"<<endl;
					//cout<<line1<<endl;
					//cout<<line2<<endl;					
					}					
			 		c1<< line1<<endl;
					c2<< line2<<endl;					
				}
				else
				{
					if(p_c >= 6)
					{
					//cout<<"Mutation is done at "<<c<<endl;
					//cout<<line1<<endl;
					//cout<<line2<<endl;
					line1 = doMutation(line1);
					line2 = doMutation(line2);
					//cout<<"After mutation"<<endl;
					//cout<<line1<<endl;
					//cout<<line2<<endl;
					}
					//if(pcross <= pc)
					{		
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
 int t=10;
 cout<<"In replacement strategy"<<endl;
 string ssrunid, ssrunid1;
 double ll, ll1; 
 for( size_t n = 0; n < t; n++ )
 {
	int val = arrval[n];
	cout<<"val = "<<val<<endl;
	vector<EMRun*>::iterator it, it1; 

	for (it=layer_prev.begin(); it != layer_prev.end(); ++it)
        {    
	      //cout<<"layer size "<<layer.size()<<endl;		   
	      ll = (*it)->getLL();
	      double iters = (*it)->getIters();
	      double runid = (*it)->getRun();
	      stringstream sll;
	      stringstream siters;	 
	      stringstream srunid;
	      string ssll; sll<<ll; ssll = sll.str();
	      string ssiters; siters <<iters; ssiters = siters.str();
	      srunid <<runid; ssrunid = srunid.str();
	      cout<<"runid = "<<ssrunid<<"  "<<ssll<<" "<<ssiters<<endl;
	      if(val==runid)
	      {
 		cout<<val<<" "<<runid<<endl;
		break;
	      }	
	}
        for (it1=layer_curr.begin(); it1 != layer_curr.end(); ++it1)
        {    
	      //cout<<"layer size "<<layer.size()<<endl;		   
	      ll1 = (*it1)->getLL();
	      double iters1 = (*it1)->getIters();
	      double runid1 = (*it1)->getRun();
	      stringstream sll1;
	      stringstream siters1;	 
	      stringstream srunid1;
	      string ssll1; sll1<<ll1; ssll1 = sll1.str();
	      string ssiters1; siters1 <<iters1; ssiters1 = siters1.str();
	      srunid1 <<runid1; ssrunid1 = srunid1.str();
	      cout<<"runid = "<<ssrunid1<<"  "<<ssll1<<" "<<ssiters1<<endl;
	      if(val==runid1)
	      {
	       cout<<val<<" "<<runid1<<endl;
	       break;
	      }	
	}
	if(ll<=ll1)
	{
		cout<<"Child  has a better LL so leave the child as it is."<<endl;
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
		cout<<"Copying files from "<<parentloc<<" to "<<childloc<<endl;
		char *ploc = &parentloc[0];
		char *cloc = &childloc[0];
		cout<<"Replace the child with the parent."<<childloc<<endl;
		remove(cloc);// remove the child file from the child folder inside the learned folder
		ifstream parent(ploc);
		ofstream child(cloc);
		child << parent.rdbuf();// copy the parent file to the child folder location inside the learned folder
	}
 } 
 
}
int main(int argc, char *argv[]) {
const char* fname =argv[2];
gibbsfile = argv[1]; 
istringstream ss( argv[3] );
ss >> pm;
stringstream ss1( argv[4] );
ss1 >> pc;
stringstream ss2(argv[5]);
alarmfgloc = ss2.str();
alarmlearnedfgloc = ss2.str();

out.open(fname);
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
	doCrossoverandMutation();
	population_no=population_no+1;
}
double t = (clock() - tStart)/CLOCKS_PER_SEC;
cout<<"Time taken: "<<t<<"s"<<endl;
out<<"Time taken: "<<t<<"s"<<endl;
out.close();
return 0;
}

