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
size_t population_size = 40; // population size
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
vector <EMRun*> layer_finalval;
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
		layer_finalval.push_back(*it); 
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
		layer_finalval.push_back(*it); 
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
	const int HIGH = 19;
	//int t =5; 
	int t=20;
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
	int *arrval = new int[t];
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
					cout<<"Mutation is done at "<<c<<endl;
					cout<<line1<<endl;
					cout<<line2<<endl;
					line1 = doMutation(line1);
					line2 = doMutation(line2);
					cout<<"After mutation"<<endl;
					cout<<line1<<endl;
					cout<<line2<<endl;					
					}					
			 		c1<< line1<<endl;
					c2<< line2<<endl;					
				}
				else
				{
					if(p_c >= 6)
					{
					cout<<"Mutation is done at "<<c<<endl;
					cout<<line1<<endl;
					cout<<line2<<endl;
					line1 = doMutation(line1);
					line2 = doMutation(line2);
					cout<<"After mutation"<<endl;
					cout<<line1<<endl;
					cout<<line2<<endl;
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
	doCrossoverandMutation();
	population_no=population_no+1;
}
double t = (clock() - tStart)/CLOCKS_PER_SEC;
cout<<"Time taken: "<<t<<"s"<<endl;
out<<"Time taken: "<<t<<"s"<<endl;
out.close();


// 6. Do mutation on the children 


// 7. Let children replace all the parents

// 8. Goto step 2


//cout<<"Done"<<endl;
//sleep(5);

   /* // Read the factorgraph from the file
    FactorGraph NewasiaNetwork;
    NewasiaNetwork.ReadFromFile( argv[4] );

    // Prepare junction-tree object for doing exact inference for E-step
    PropertySet infprops;
    infprops.set( "verbose", (size_t)1 );
    infprops.set( "updates", string("HUGIN") );
    InfAlg* inf = newInfAlg( "JTREE", NewasiaNetwork, infprops );
    inf->init();

    ofstream outfile;
    outfile.open(argv[2],ios::out | ios::app);
    Evidence e;
    ifstream estream(argv[1]);
    e.addEvidenceTabFile( estream, NewasiaNetwork );
    ifstream emstream( argv[3] );
    EMAlg em(e, *inf, emstream);
    Real l;
// Iterate EM until convergence
    while( !em.hasSatisfiedTermConditions() ) {
       l = em.iterate();
    }
    outfile << argv[1] << " " << argv[4]<<" "<<em.Iterations()<<" " << l << endl;
    outfile << endl << "True factor graph:" << endl << "##################" << endl;
    outfile.precision(12);
    outfile << NewasiaNetwork;
   // Output learnedfactorgraph                                                                                                                                                                         
    outfile << endl << "Learned factor graph:" << endl << "#####################" << endl;
    outfile.precision(12);
    outfile << inf->fg();
     delete inf;
    outfile.close();	
*/
    return 0;
}

