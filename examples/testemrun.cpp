
#include <dai/emalg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <dai/alldai.h>
#include <boost/thread/thread.hpp>
#include <stdlib.h>
#include <vector>
#include "dai/emrun.h"

using namespace std; 
using namespace dai; 
using namespace boost; 

int main(int argc, char *argv[]){

  const  string alarmfgloc = "/home/priya/libDAI-0.3.1/alarm_factor_graphs/"; 
  const string alarmlearnedfgloc = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/";
  const char* gibbsfile = "/home/priya/libDAI-0.3.1/alarm/1000/23.tab";
  /*const char* fglearnedfileloc1 = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/alarm_5.fg"; 
  const char* fglearnedfileloc2 = "/home/priya/libDAI-0.3.1/alarm_learned_factor_graphs/alarm_7.fg";
  string fgfileloc1 =  alarmfgloc + "alarm_0.fg";
  string fgfileloc2 =  alarmfgloc + "alarm_1.fg";
  const char* fgfile1 = &fgfileloc1[0];
  const char* fgfile2 = &fgfileloc2[0];*/
  //EMRun* runname;
  vector <EMRun*> layer;  
  int pop_size=5;
  size_t i =0,j=0;
  size_t maxIters = 100;
  while(i<pop_size)
  {
	 string number; 
	 stringstream num; 
	 num << i; 
	 number = num.str();	 
	 string fgfileloc =  alarmfgloc + "/"+"alarm_"+number+".fg";
	 const char* fgfile = &fgfileloc[0];

	 EMRun* runname = new EMRun("alarm", i, fgfile, gibbsfile); // args are -1.network name 2.factor-graph number 3. evidencefile

	 // 2. Run EM
	 boost::thread threadid(boost::bind(&EMRun::startEM, boost::ref(runname), maxIters)); // args - max iters 
	 layer.push_back(runname); 
	 sleep(1);
	 // 4. Next individual
	 i=i+1;
 } 
while (true){
	   // cout<<"in while loop "<<layer.size()<<endl;
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
	 	string fglearnedfileloc =  alarmlearnedfgloc + "alarm_"+number+".fg";
	    	const char* fglearnedfile = &fglearnedfileloc[0];
		cout<<"Run number "<<number<<" ll = "<<ll<<" iters = "<<iters<<endl;	
		// 3. Store the learned factor graphs
		InfAlg* inf = (*it)->getInf();
		ofstream learnedstream;
		/*cout<<"learned file = "<<fglearnedfile<<endl;
		learnedstream.open(fglearnedfile);
		learnedstream.precision(12);
		learnedstream << inf->fg();  
		learnedstream.flush();
		learnedstream.close();*/
		delete inf;
		sleep(1);
		//cout<<"Run number "<<number<<" has terminated"<<endl;
		//out<<gibbsfile<<" "<<fglearnedfile<<" "<<runid<<" "<<iters<<" "<<ll<<" "<<endl; 
		layer.erase(it);	
		if (layer.size() == 0){break;}
		else { it = layer.begin(); }
	      }
	      else
		{
		}
	}
	if (layer.size() == 0){break;}
}
  //EMRun* testrun2 = new EMRun("alarm", 1, fgfile2, gibbsfile);

 /* EMRun* testrun3 = new EMRun("alarm", 13, gibbsfile); 
  EMRun* testrun4 = new EMRun("alarm", 32, gibbsfile); 

  boost::thread thread1(boost::bind(&EMRun::startEM, boost::ref(testrun), 100)); 
  boost::thread thread2(boost::bind(&EMRun::startEM, boost::ref(testrun2), 100)); 
 /* boost::thread thread3(boost::bind(&EMRun::startEM, boost::ref(testrun3), 100)); 
  boost::thread thread4(boost::bind(&EMRun::startEM, boost::ref(testrun4), 100)); */
//  sleep(10);
 // boost::thread thread9(boost::bind(&EMRun::stopEM, boost::ref(testrun)));
 // boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(testrun2))); 	
/* boost::thread thread6(boost::bind(&EMRun::stopEM, boost::ref(testrun3)));
  boost::thread thread7(boost::bind(&EMRun::stopEM, boost::ref(testrun4)));
  double ll1 = testrun->getLL();
  double iters1 = testrun->getIters();
  double ll2 = testrun2->getLL();
  double iters2 = testrun2->getIters();
  InfAlg *inf_1 = testrun->getInf();
  InfAlg *inf_2 = testrun2->getInf();
  ofstream learnedstream1;
      cout<<"learned file = "<<fglearnedfileloc1;
      learnedstream1.open(fglearnedfileloc1);
      learnedstream1.precision(12);
      learnedstream1 << inf_1->fg();  
      learnedstream1.close();
 ofstream learnedstream2;
      cout<<"learned file = "<<fglearnedfileloc2;
      learnedstream2.open(fglearnedfileloc2);
      learnedstream2.precision(12);
      learnedstream2 << inf_2->fg();  
      learnedstream2.close();

  cout<<"Stopping testrun.."<<endl; 
  cout << "LL for testrun eventually " <<ll1<<endl;
  cout << "iterations for testrun eventually "<<iters1<<endl;
 cout<<"Stopping testrun2.."<<endl; 
  cout << "LL for testrun2 eventually " <<ll2<<endl;
  cout << "iterations for testrun2 eventually "<<iters2<<endl;
 /*vector <EMRun*> layer2; 
  vector <EMRun*> layer2_finalval; 

  layer2.push_back(testrun3); 
  layer2.push_back(testrun4); 

  boost::thread thread1(boost::bind(&EMRun::startEM, boost::ref(testrun), 100)); 
  boost::thread thread2(boost::bind(&EMRun::startEM, boost::ref(testrun2), 100)); 
  cout <<"Started older layer, consisting of factor graphs "<< testrun->getRun() << " and " << testrun2->getRun() << endl; 

  int seconds = atoi (argv[1]); 
  sleep(seconds); 

  
  const int maxIters = 10;   
  boost::thread thread3(boost::bind(&EMRun::startEM, boost::ref(testrun3), maxIters));
  boost::thread thread4(boost::bind(&EMRun::startEM, boost::ref(testrun4), maxIters));
  cout <<"Started younger layer consisting of factor graphs "<< testrun3->getRun() << " and " << testrun4->getRun() <<endl; 
  
  while (true){
    vector<EMRun*>::iterator it; 
    for (it=layer2.begin(); it != layer2.end(); ++it){
      if((*it)->hasTerminated()) { //if it has terminated, set the run aside
	cout<<"Run number "<<(*it)->getRun()<<" has terminated"<<endl; 
	layer2_finalval.push_back(*it); 
	layer2.erase(it);
	if (layer2.size() == 0){break;}
	else { it = layer2.begin(); }
      }
      if ((*it)->getIters() == maxIters){ //we stop the ones in the higher layer - in reality, we can have a function that loops through a layer and stops everything
	cout<<"Run number "<<(*it)->getRun()<<" has reached the maximum iterations"<<endl; 
	int numit = (*it)->getIters(); 
	cout<<"The number of iterations it has undergone is "<<numit<<endl; 
	boost::thread thread5(boost::bind(&EMRun::stopEM, boost::ref(testrun))); 	
	boost::thread thread6(boost::bind(&EMRun::stopEM, boost::ref(testrun2)));
	cout<<"Stopped runs in the layer immediately above..."<<endl; 
	double ll1 = testrun->getLL(); 
	double ll2 = testrun2->getLL(); 
	double ll_test = (*it)->getLL(); 
	cout << "LL values in higher layer are "<< ll1 << " and "<< ll2 << endl; 
	cout<< "LL value for recently finished run in lower layer is " << ll_test << endl; 
	layer2.erase(it); 
	if (layer2.size() == 0) { break; }
	else {	it = layer2.begin(); }
	cout<<"Size of layer now is "<<layer2.size()<<endl; 
	cout<<"Resuming runs in the layer immediately above..."<<endl; 
	boost::thread thread7(boost::bind(&EMRun::resumeEM, boost::ref(testrun))); 
	boost::thread thread8(boost::bind(&EMRun::resumeEM, boost::ref(testrun2)));
      }
    }
    if (layer2.size() == 0){ break; }
  sleep(1); 
  }
      
  sleep(5); 
  cout<<"Waited 5 seconds..."<<endl; 

  boost::thread thread9(boost::bind(&EMRun::stopEM, boost::ref(testrun))); 
  boost::thread thread10(boost::bind(&EMRun::stopEM, boost::ref(testrun2))); 
  cout<<"Stopped testrun and testrun2..."<<endl; 
  double ll1 = testrun->getLL(); 
  double ll2 = testrun2->getLL(); 
  int iters1 = testrun->getIters(); 
  int iters2 = testrun2->getIters(); 
  cout << "LL for testrun1 eventually " <<ll1<<endl; 
  cout << "iterations for testrun1 eventually "<<iters1<<endl; 
  cout << "LL for testrun2 eventually " <<ll2<<endl; 
  cout << "iterations for testrun2 eventually "<<iters2<<endl; 

  if (testrun->hasTerminated()){
    cout<<"testrun has terminated..."<<endl; 
  }
  if (testrun2->hasTerminated()){
    cout<<"testrun2 has terminated..."<<endl; 
  }

  boost::thread thread11(boost::bind(&EMRun::resumeEM, boost::ref(testrun3))); 
  boost::thread thread12(boost::bind(&EMRun::startEM, boost::ref(testrun3), 20)); 
  sleep(seconds); 
  cout<<"Waited "<<seconds<<" seconds..."<<endl; 
  boost::thread thread13(boost::bind(&EMRun::stopEM, boost::ref(testrun3)));
  ll1 = testrun3->getLL(); 
  iters1 = testrun3->getIters(); 
  cout<<"LL and iters for testrun 3 are "<<ll1<<" and "<<iters1<<endl; 

 
  boost::thread thread7(boost::bind(&EMRun::resumeEM, boost::ref(testrun)));                                                                                                                        
  boost::thread thread8(boost::bind(&EMRun::resumeEM, boost::ref(testrun2)));         
  cout<<"Resuming testrun and testrun2..."<<endl; 
  sleep(seconds); 
  boost::thread thread11(boost::bind(&EMRun::stopEM, boost::ref(testrun)));
  boost::thread thread12(boost::bind(&EMRun::stopEM, boost::ref(testrun2)));
  ll1 = testrun->getLL();
  ll2 = testrun2->getLL();
  iters1 = testrun->getIters();
  iters2 = testrun2->getIters();
  cout<<"Stopping testrun and testrun2..."<<endl; 
  cout << "LL for testrun1 eventually " <<ll1<<endl;
  cout << "iterations for testrun1 eventually "<<iters1<<endl;
  cout << "LL for testrun2 eventually " <<ll2<<endl;
  cout << "iterations for testrun2 eventually "<<iters2<<endl;*/

  return 0; 
}
