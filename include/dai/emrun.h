/*
 *File: emrun.h
 * This interface provides the functions needed to interact with the emrun class
 */

#ifndef _emrun_h
#define _emrun_h

#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <dai/alldai.h>
#include <dai/factor.h>
#include <dai/daialg.h>
#include <dai/evidence.h>
#include <dai/index.h>
#include <dai/properties.h>

#include <boost/config.hpp>

#include <iostream>
#include <string>

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

using namespace boost::bimaps;
using namespace boost;
using namespace dai;
class EMRun {
 public: 
  EMRun (const std::string network, const int fg_num,  const char* fgfile, const char* gibbsfilename, const char* emfileloc);
    EMRun (const std::string network, const int fg_num,  dai::FactorGraph& fg, const char* gibbsfilename, const char* emfileloc);
  void startEM (const int maxIters); 
  void resumeEM (); 
  void stopEM();
    void forceStopped(){ forceStop=1; }
    int isForceStop(){ return forceStop;}
  double getLL(); 
  bool hasTerminated(); 
  int getIters(); 
  int getRun();
  std::vector<dai::Real> lastlog(){ return llHistory;}
    std::vector<int> getmutateFactors(){ return mutateFactors;}
  dai::InfAlg* getInf() { return inf; }
  dai::hash_map<int, double> getParams() { return hashmap; }
  boost::bimaps::bimap<boost::bimaps::unordered_set_of<int>, boost::bimaps::multiset_of<double, std::greater<double> > >::right_map getBimap() { return pop.right; }

  void getParamTrace(int nFactors);
    size_t getStates(dai::Factor P,size_t factor_pos);
    
  ~EMRun(); 


 private:
    dai::hash_map<int, double> hashmap;
    typedef bimap<
    
    unordered_set_of< int >,
    multiset_of< double, std::greater<double> >
    
    > population_bimap;
    
    typedef population_bimap::value_type population;
    population_bimap pop;
  
  int runNum;
  int numIters;
  dai::Real ll;
    std::vector<int> mutateFactors;
  std::vector<dai::Real> llHistory;
  std::vector<dai::FactorGraph> fgTrace;
  bool terminated;
  bool running;
  int forceStop;
  int nFactors;
  dai::EMAlg* emobj; 
  dai::InfAlg* inf; 
  dai::Evidence e; 
  std::ifstream* emstream;  
  boost::mutex m_stop_mutex; 
  boost::condition m_stop_changed; 
  boost::mutex m_terminate_mutex; 
  boost::mutex m_numiter_mutex;
  void stop_while_paused();
  
};

#endif 
  
