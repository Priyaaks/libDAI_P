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


size_t population_size = 20000; // population size

// 1. Creating initial population of EM runs 
	
char* gibbsfile;// = "/home/priya/libDAI-0.3.1/alarm/500/7.tab";
char* emfile;
string alarmfgloc;// = "/home/priya/libDAI-0.3.1/alarm_new_factor_graphs/"; 
string alarmlearnedfgloc;// = "/home/priya/libDAI-0.3.1/500_baseline/7/0.001/";
int arr[] ={0, 500, 1000, 1500,  2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000, 10500, 11000, 11500, 12000, 12500};
const char* networkname = "Movie4NodeBN2";
ofstream out;
const char* fname =argv[2];
gibbsfile = argv[1]; 
stringstream ss2(argv[5]);
alarmfgloc = ss2.str();
stringstream ss4(argv[6]);
alarmlearnedfgloc = ss4.str();
emfile = argv[7];

stringstream ss3(argv[3]);
int q;
ss3>>q;
//cout<<"q val"<<endl;

out.open(fname);
//cout<<fname<<endl;
size_t maxIters =1000; // maximum number of iterations
size_t i =0,j=0;

EMRun* runname;
vector <EMRun*> layer; 
vector <EMRun*> layer_finalval;
  //clock_t tStart = clock();
time_t timer1,timer2;
time(&timer1);
clock_t tStart = clock();
int no;
no = arr[q];
//cout<<" startin no."<<no<<endl;
for(int j=0;j<10;j++) // this j value is equivalent to generations in GAEM
{
i=0;
//cout<<j<<endl;
while(i<4) // this i value is equivalent to 20 individuals in the initial population in GAEM
{
 string number; 
 stringstream num; 
 num << no; // everytime a new factor graph is picked to run the EM
 number = num.str();

 string fgfileloc =  alarmfgloc + "Movie4NodeBN2_"+number+".fg";
 const char* fgfile = &fgfileloc[0];
 cout<<"Starting EM "<<fgfileloc<<endl;
 runname = new EMRun(networkname, no, fgfile, gibbsfile, emfile); // args are -1.network name 2.factor-graph number 3. evidencefile

 // 2. Run EM
 boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters)); // args - max iters 
 layer.push_back(runname); 
 sleep(1);
 // 4. Next individual
 no=no+1;
 i=i+1;
}

//sleep(30);

while (true){
    //cout<<"in while loop "<<layer.size()<<endl;
    vector<EMRun*>::iterator it; 
    for (it=layer.begin(); it != layer.end(); ++it){    
      //cout<<"layer size "<<layer.size()<<endl;	
      if((*it)->hasTerminated()) { //if it has terminated, set the run aside	   
        double ll = (*it)->getLL();
        double iters = (*it)->getIters();
        double runid = (*it)->getRun();
	string number; 
	stringstream num; 
	num << runid; 
	number = num.str();
 	string fglearnedfileloc =  alarmlearnedfgloc + "Movie4NodeBN2_"+number+".fg";
    	const char* fglearnedfile = &fglearnedfileloc[0];
        //cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
        // 3. Store the learned factor graphs
        InfAlg* inf = (*it)->getInf();
        ofstream learnedstream;
        //cout<<"learned file = "<<fglearnedfile<<endl;
        learnedstream.open(fglearnedfile);
        learnedstream.precision(2);
        learnedstream << inf->fg();
	learnedstream.flush();  
        learnedstream.close();
	delete inf;
	sleep(1);
        //cout<<"Run number "<<number<<" has terminated"<<endl;
	out<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
	layer_finalval.push_back(*it); 
	layer.erase(it);	
	if (layer.size() == 0){break;}
	else { it = layer.begin(); }
      }
      else if(((*it)->getIters()) == maxIters){ //we stop the run
        double ll = (*it)->getLL();
        double iters = (*it)->getIters();
        double runid = (*it)->getRun();
	/*string number; 
	stringstream num; 
	num << runid; 
	number = num.str();
 	string fglearnedfileloc =  alarmlearnedfgloc + "carstarts_"+number+".fg";
    	const char* fglearnedfile = &fglearnedfileloc[0];
        //cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
        //cout<<"Run "<<(*it)->getRun()<<" has reached maximum iterations, so stopping the run"<<endl;
	boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(*it))); 
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
	sleep(1);*/
        //cout<<"Run number "<<number<<" has reached maximum iterations so stopped"<<endl;
	out<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
        layer_finalval.push_back(*it); 
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

    
time(&timer2);
double t = difftime(timer2,timer1);
    double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
cout<<"Time taken: "<<t<<"s"<<endl;
out<<"Time taken: "<<t<<"s"<<endl;
out<<" Processor Time taken: "<<t_c<<"s"<<endl;
out.close();

return 0;
}
