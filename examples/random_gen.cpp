#include <dai/alldai.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;
using namespace dai;
// Type for storing a joint state of all variables
typedef std::vector<size_t> stateVec;
//./example_Newasia.em ./500/coopy.tab out.log alarm.em alarm_1.fg out1.log
struct MPEbest {
    Real value;
    stateVec state;
    bool ready;
    MPEbest() : value(-INFINITY), state(), ready(false) {}
};
struct PRbest {
    Real value;
    Real maxdiff;
    bool ready;
    PRbest() : value(0.0), maxdiff(INFINITY), ready(false) {}
};
int main(int argc, char *argv[]) {
time_t timer1, timer2;
time(&timer1);
clock_t tStart = clock();
    // Read the factorgraph from the file
    FactorGraph NewasiaNetwork;
    NewasiaNetwork.ReadFromFile( argv[4] );

    // Prepare junction-tree object for doing exact inference for E-step
    PropertySet infprops;
    infprops.set( "verbose", (size_t)1 );
    infprops.set( "updates", string("HUGIN") );
    InfAlg* inf = newInfAlg( "JTREE", NewasiaNetwork, infprops );
    inf->init();
	// opening output files
    ofstream outfile;
    outfile.open(argv[2],ios::out | ios::app);
    ofstream outfile1;
    //outfile1.open(argv[5],ios::out | ios::app);
    outfile<<argv[4]<<endl;
	// reading evidence
	Evidence e;
    ifstream estream(argv[1]);
    e.addEvidenceTabFile( estream, NewasiaNetwork );
   // printing evidence

cout << "Number of samples: " << e.nrSamples() << endl;
    Real error =0,success=0, sum=0;
    for( Evidence::iterator ps = e.begin(); ps != e.end(); ps++ ){
// cout << "Sample #" << (ps - e.begin()) << " has " << ps->size() << " observations." << endl;
   // for( Evidence::const_iterator e = _evidence.begin(); e != _evidence.end(); ++e ) {
        InfAlg* clamped = newInfAlg( "JTREE", NewasiaNetwork, infprops );
        Real ii=0,rating=0,rmse=0;
        clamped->init();
        // Apply evidence
        for( Evidence::Observation::const_iterator i = ps->begin(); i != ps->end(); ++i )
        {
            if(ii<2)
            clamped->clamp( clamped->fg().findVar(i->first), i->second );
            else
                rating = i->second;
                
         //   cout<<" "<<i->second<<" ";//<<ii;
            ii=ii+1;
        }
//        cout<<endl;
//        cout<<"Rating is "<<ii<<endl;
        clamped->run();
        // Report variable marginals for fg, calculated by the junction tree algorithm
//        cout << "Exact variable marginals:" << endl;
//        for( size_t i = 0; i < clamped->fg().nrVars(); i++ ) // iterate over all variables in fg
//            cout << clamped->belief(clamped->fg().var(i)) << endl; // display the "belief" of jt for that variable
        
        //PRbest cur;
        
        // calculate PR value
       // cur.value = clamped->logZ() / dai::log((Real)10.0);
        MPEbest cur;
        
        // calculate MPE state
        cur.state = clamped->findMaximum();
        
        // calculate MPE value
        cur.value = clamped->fg().logScore( cur.state );

        // cout<<" "<<cur.state[3]<<" "<<endl;
         if(rating!=cur.state[3])
         {
             error++;
             rmse = pow((rating - cur.state[3]),2);
             //cout<<rating<<" "<<cur.state[3]<<endl;
         }
        else
            success++;
        //break;
        sum = sum+rmse;
    }
    Real tot = error+success;
    outfile<<error<<" "<<success<<" "<<"error = "<<(error/tot)<<" rmse = "<<sqrt(sum/tot)<<endl;

time(&timer2);
double t = difftime(timer2,timer1);
(clock() - tStart)/CLOCKS_PER_SEC;
double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
outfile<<"Time taken: "<<t<<"s"<<endl;
outfile<<" Processor Time taken: "<<t_c<<"s"<<endl;
     delete inf;
    outfile.close();	
   // outfile1.close();
    return 0;
}
