/* 
 *Filename: emrun.cpp
 *Description: class that handles emrun object, including startem, pauseem functions for layer class to interface with
 *Date: April 6, 2011
 */

#include <dai/alldai.h>
#include <dai/emalg.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <boost/thread/thread.hpp>
#include "dai/emrun.h"
#include <string>
#include <dai/util.h>
#include <unordered_map>
using namespace std; 
using namespace dai; 

const string alarmfgloc = "/Applications/Utilities/libDAI-0.3.1/alarm_factor_graphs/"; 
const string carstartsfgloc = "/Applications/Utilities/libDAI-0.3.1/carstarts_factor_graphs/"; 
 FactorGraph fg; 
//const char* learnedfile; 

EMRun::EMRun (const string network,  const int fg_num, const char* fgfile, const char* gibbsfilename, const char* emfileloc) : runNum(fg_num), numIters(0), ll(0.0), terminated(0), running(1), emobj(NULL), inf(NULL), e(), emstream(NULL){
  string number; 
  stringstream num; 
  //num << fg_num; 
  //number = num.str(); 
  string fgfileloc; 
  string fglearnedfileloc;
    
  //reads the factor graph from the file
 
 // const char* fgfile; 
  //fgfile = &fgfileloc[0]; 
  fg.ReadFromFile(fgfile); 
  nFactors = fg.nrFactors();
  //setting up junction tree object for inference algo
  PropertySet infprops; 
  infprops.set( "verbose", (size_t)1 ); 
  infprops.set( "updates", string("HUGIN") ); 
  inf = newInfAlg ("JTREE", fg, infprops); 
  inf->init(); 
  
  //loading the gibbs file data in
  ifstream estream( gibbsfilename ); 
  e.addEvidenceTabFile( estream, fg); 
  const char* emfile; 
  emfile = &emfileloc[0]; 
  emstream = new ifstream ( emfile ); 
  emobj = new EMAlg(e, *inf, *emstream);
  
}

EMRun::EMRun (const string network,  const int fg_num, FactorGraph& fg, const char* gibbsfilename, const char* emfileloc) : runNum(fg_num), numIters(0), ll(0.0), terminated(0), running(1), emobj(NULL), inf(NULL), e(), emstream(NULL){

    //setting up junction tree object for inference algo
    PropertySet infprops;
    infprops.set( "verbose", (size_t)1 );
    infprops.set( "updates", string("HUGIN") );
    inf = newInfAlg ("JTREE", fg, infprops);
    inf->init();
    
    //loading the gibbs file data in
    ifstream estream( gibbsfilename );
    e.addEvidenceTabFile( estream, fg);
    const char* emfile;
    emfile = &emfileloc[0];
    emstream = new ifstream ( emfile );
    emobj = new EMAlg(e, *inf, *emstream);
    
}
/*void EMRun::startEM(const int maxiters) {
  if (!(emobj->hasSatisfiedTermConditions())){
    {
      boost::mutex::scoped_lock lock(mutex_);
      while (!running){
	condition_.wait(lock); 
      }
    }
    while (!(emobj->hasSatisfiedTermConditions()) && numIters <= maxiters){
      {
	boost::mutex::scoped_lock lock(mutex_);
	if (!running){
	  break;
	}
      }
      ll = emobj->iterate(); 
      numIters++; 
    }
    if (running){
      boost::mutex::scoped_lock lock(mutex_); 
      running = 0; 
      
    }
    if (emobj->hasSatisfiedTermConditions()){
      terminated = 1; 
    }
  }
  }*/

void EMRun::startEM(const int maxIters) {  
  if (terminated == 0 && numIters < maxIters){
   while (!(emobj->hasSatisfiedTermConditions()) && numIters < maxIters){
      stop_while_paused(); 
      {
        boost::unique_lock<boost::mutex> lock(m_numiter_mutex);
        //cout <<"iter number "<<numIters<<endl;
        ll = emobj->iterate();
    	numIters++;
        //cout<<" +++ +++++ +++++"<<numIters<<endl;
          dai::FactorGraph fg1 = inf->fg();
        fgTrace.push_back(fg1);
      }
    }
  if (emobj->hasSatisfiedTermConditions()){
      boost::unique_lock<boost::mutex> lock(m_terminate_mutex); 
      terminated = 1;
         llHistory = emobj->lastlogZ();
    }
      getParamTrace(nFactors);
    // for Global GAEM
    /*ll = emobj->run1();
    numIters++;
    boost::unique_lock<boost::mutex> lock(m_terminate_mutex); 
    terminated = 1;   */
  }
}
void EMRun::getParamTrace(int nFactors)
{
    mutateFactors.clear();
    for(int i=0;i<fg.nrFactors();i++)
    {
        hashmap[i]=0;
      //  cout<<i<<" HERE "<<hashmap.at(i)<<endl;
    }
    FactorGraph fg1,fg2;
    vector<FactorGraph>::iterator it = fgTrace.begin();
    fg1 = *it;
    int i=0;
    while(it != fgTrace.end())
    {
        fg2 = *it++;
        //cout<<i++<<endl;
        for( int I = 0; I < fg1.nrFactors(); I++ ) {
            for( int I1= 0; I1 < fg2.nrFactors(); I1++ ) {
                if(I==I1)
                {
                    //cout<<" factor "<<I;
                    //cout<<" "<<dist(fg2.factor(I1),fg1.factor(I),DISTKL)<<endl;
                    double val = hashmap[I];
                    size_t states = getStates(fg2.factor(I1),I1);
                    hashmap[I] = val+(dist(fg2.factor(I1),fg1.factor(I),DISTKL)/states);
//                    cout<<pop.left.at(I)<<" "<<(dist(fg2.factor(I1),fg1.factor(I),DISTKL)/states)<<endl;
                    //double val1 = pop.left.at(I)+(dist(fg2.factor(I1),fg1.factor(I),DISTKL)/states);
                    //pop.left.find(I)->second = val1;
                    //pop.insert( population(I,val1) );
                    //cout<<" "<<pop.left.at(I)<<" "<<val1<<endl;
                    //cout<<I<<" "<<states<<endl;
                    //params[I] = params[I]+dist(fg2.factor(I1),fg1.factor(I),DISTKL);
                    break;
                }
            }
        }
        fg1 = fg2;
        
    }
    for(int i=0;i<fg1.nrFactors();i++)
    {
       // cout<<hashmap.at(i)<<endl;
        pop.insert( population(i,hashmap.at(i)) );
    }
   // cout<<"params got"<<endl;
    int j=0;
   // cout<<"______"<<mutateFactors.size()<<endl;
    for( population_bimap::right_const_iterator
        i = pop.right.begin(), iend = pop.right.end();
        i != iend ; ++i )
    {
        double v =i->first;
       // if(v>0.4)//(j<10)
        {
            //std::cout << i->second << " with " << i->first << std::endl;
            mutateFactors.push_back(i->second);
        }
        j++;
    }
  //  cout<<"______"<<mutateFactors.size()<<endl;
}

size_t EMRun::getStates(dai::Factor P,size_t factor_no)
{
    size_t states;
    VarSet vs = P.vars();
     //cout<<" factor = "<<" "<<factor_no;
    for( VarSet::const_iterator v = vs.begin(); v != vs.end(); v++ )
    {
        //cout<<" vars = "<<" "<<v->label();
        
        if(v->label() == factor_no)
        {
            //cout<<" states = "<<v->states()<<endl;
            states = v->states();
        }
    }
    return states;
}
void EMRun::stop_while_paused() {
  boost::unique_lock<boost::mutex> lock(m_stop_mutex); 
  while (!running){
    m_stop_changed.wait(lock); 
  }
}

bool EMRun::hasTerminated() {
  boost::unique_lock<boost::mutex> lock(m_terminate_mutex); 
  return terminated; 
}

int EMRun::getIters() {
  boost::unique_lock<boost::mutex> lock(m_numiter_mutex); 
  return numIters;
}

int EMRun::getRun() {
  return runNum;
}



void EMRun::stopEM() {
  {
    boost::unique_lock<boost::mutex> lock(m_stop_mutex); 
    running = 0;
  }
  m_stop_changed.notify_all(); 
}

void EMRun::resumeEM(){
  {
    boost::unique_lock<boost::mutex> lock(m_stop_mutex); 
    running = 1;
  }
  m_stop_changed.notify_all();
}

double EMRun::getLL() {
  return ll; 
}



EMRun::~EMRun() {
  delete inf;  
  delete emobj; 
  emobj = NULL;
}




 

  
