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
#include "time.h"
using namespace std;
using namespace dai;

int main(int argc, char *argv[]) {

// GA - initialization
size_t max_gen =3; // no. of generations
size_t population_size = 100; // population size
Real pc = 0.6; // crossover probability - between 0.6 an 0.9
Real pm = 0.05; // mutation probability - between 1/pop.size and 1/chromosome_length


// 1. Creating initial population of EM runs 
	
const char* gibbsfile = "/home/priya/libDAI-0.3.1/alarm/500/7.tab";
const  string alarmfgloc = "/home/priya/libDAI-0.3.1/alarm_factor_graphs/"; 
const string alarmlearnedfgloc = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/";
const char* networkname = "alarm";

size_t maxIters =1000; // maximum number of iterations
size_t i =0,j=0;

ofstream out;
const char* fname ="/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/outmtem.log"; 
out.open(fname);
EMRun* runname;
vector <EMRun*> layer; 
vector <EMRun*> layer_finalval;
  clock_t tStart = clock();
while(i<population_size)
{
 string number; 
 stringstream num; 
 num << i; 
 number = num.str();

 string fgfileloc =  alarmfgloc + "alarm_"+number+".fg";
 const char* fgfile = &fgfileloc[0];

 runname = new EMRun(networkname, i, fgfile, gibbsfile); // args are -1.network name 2.factor-graph number 3. evidencefile

 // 2. Run EM
 boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters)); // args - max iters 
 layer.push_back(runname); 
 sleep(1);
 // 4. Next individual
 i=i+1;
}

//sleep(30);

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
 	string fglearnedfileloc =  alarmlearnedfgloc + "alarm_"+number+".fg";
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
	out<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
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
 	string fglearnedfileloc =  alarmlearnedfgloc + "alarm_"+number+".fg";
    	const char* fglearnedfile = &fglearnedfileloc[0];
        cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
        cout<<"Run "<<(*it)->getRun()<<" has reached maximum iterations, so stopping the run"<<endl;
	boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(*it))); 
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
        cout<<"Run number "<<number<<" has reached maximum iterations so stopped"<<endl;
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
double t = (clock() - tStart)/CLOCKS_PER_SEC;
cout<<"Time taken: "<<t<<"s"<<endl;
out<<"Time taken: "<<t<<"s"<<endl;
out.close();

//cout<<"Done"<<endl;
//sleep(5);


// 7. Let children replace all the parents

// 8. Goto step 2



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
