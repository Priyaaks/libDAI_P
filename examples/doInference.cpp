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
    infprops.set("inference",string("SUMPROD"));
    PropertySet infprops1;
    infprops1.set( "verbose", (size_t)1 );
    infprops1.set( "updates", string("HUGIN") );
    infprops1.set("inference", string("MAXPROD"));
  
	// opening output files
    ofstream outfile;
    outfile.open(argv[2],ios::out | ios::app);
    ofstream outfile1;
    //outfile1.open(argv[5],ios::out | ios::app);
    outfile<<argv[4]<<" ";
	// reading evidence
	Evidence e;
    ifstream estream(argv[1]);
    e.addEvidenceTabFile( estream, NewasiaNetwork );
   // printing evidence

//cout << "Number of samples: " << e.nrSamples() << endl;
    Real error =0,success=0, sum=0,threshold=3,tp=0,tn=0,fp=0,fn=0;
//    
//    // Calculate joint probability of all four variables
//    Factor P;
//    for( size_t I = 0; I < NewasiaNetwork.nrFactors(); I++ )
//    {
//       // outfile<<" Fact name = "<<I<<endl;
//        //outfile<<NewasiaNetwork.factor( I )<<endl;
//        P *= NewasiaNetwork.factor( I );
//    }
//    //        cout<<"Joint probability = "<<P<<endl;
//     // P.normalize(); // Not necessary: a Bayesian network is already normalized by definition
//  
//    
    
    for( Evidence::iterator ps = e.begin(); ps != e.end(); ps++ ){
// cout << "Sample #" << (ps - e.begin()) << " has " << ps->size() << " observations." << endl;
   // for( Evidence::const_iterator e = _evidence.begin(); e != _evidence.end(); ++e ) {
        //InfAlg* clamped = newInfAlg( "JTREE", NewasiaNetwork, infprops );
        InfAlg* clamped1 = newInfAlg( "JTREE", NewasiaNetwork, infprops1 );
        Real ii=0,rating=0,user=0,movie=0,rmse=0;
        clamped1->init();
        // Apply evidence
        for( Evidence::Observation::const_iterator i = ps->begin(); i != ps->end(); ++i )
        {
            
            if(ii==0)
                user = i->second;
            else if(ii==1)
                movie =i->second;
            else
                rating =i->second;
            
            if(ii<2)
            {//clamped->clamp( clamped->fg().findVar(i->first), i->second );
            clamped1->clamp( clamped1->fg().findVar(i->first), i->second );
            }
           // cout<<" "<<i->second<<" ";//<<ii;
            ii=ii+1;
        }
        //clamped->run();
        clamped1->run();
//        Real bestMar =0; int bestRating=0,i=0;
//        for( i=1;i<6;i++)
//        {
//        // Calculate some probabilities
//        Real denom =P.marginal( VarSet(NewasiaNetwork.var(0),NewasiaNetwork.var(2) ))[user*movie];
//        //outfile<<P.marginal( VarSet(NewasiaNetwork.var(0),NewasiaNetwork.var(2) ))[user*movie]<<endl;
//        //cout << "denom " << denom << endl;
//        
//        VarSet V;
//        V |= NewasiaNetwork.var(0);
//        V |= NewasiaNetwork.var(2);
//        V |= NewasiaNetwork.var(3);
//        Real num = P.marginal( V)[user*movie*i];
//          //  cout<<"nume"<<num<<endl;
//        //outfile<<P.marginal( V)[user*movie*i]<<endl;
//            Real mar = num/denom;
//            if(bestMar<mar)
//            {
//                bestMar=mar;
//                bestRating=i;
//            }
//        //cout << "P(user=1,movie=74,rating =1 | user=1,movie=74) = " << num/denom << endl;
//        }
////        cout<<endl;
////        cout<<"Rating is "<<ii<<endl;
//        // Report variable marginals for fg, calculated by the junction tree algorithm
//        cout << "Exact variable marginals:" << endl;
//        for( size_t i = 0; i < clamped->fg().nrVars(); i++ ) // iterate over all variables in fg
//            cout<<"variabel name "<<clamped->fg().var(i)<<" ";
//            //cout << clamped->belief(clamped->fg().var(i)) << endl; // display the "belief" of jt for that variable
//        
        //PRbest cur;
        
        // calculate PR value
       // cur.value = clamped->logZ() / dai::log((Real)10.0);
        MPEbest cur,cur1;
        
        // calculate MPE state
        //cur.state = clamped->findMaximum();
        cur1.state = clamped1->findMaximum();
        
//        cout<<"Curr State = "<<cur.state<<endl;
//        cur.value = clamped->fg().logScore( cur.state );
//        cout<<"MAP log score = "<<cur.value<<endl;
//        
//        cout<<"Curr State = "<<cur1.state<<endl;
        cur1.value = clamped1->fg().logScore( cur1.state );
//        cout<<"MPE log score = "<<cur1.value<<endl;

      //  cout<<" ACTUAL "<<rating<<" "<<endl;
       //  cout<<" MAP "<<cur.state[3]<<" "<<endl;
       // cout<<" MPR "<<cur1.state[3]<<" "<<endl;
       /// cout<<" COND "<<bestRating<<" "<<endl;
        
        
//        if(rating>=threshold && bestRating>=threshold)
//            tp++;
//        else if(rating<threshold && bestRating>=threshold)
//            fn++;
//        else if(rating>=threshold && bestRating<threshold)
//            fp++;
//        else if(rating<threshold && bestRating<threshold)
//            tn++;
//        
//        if(rating!=bestRating)
//        {
//            error++;
//            rmse = pow((rating - bestRating),2);
//            cout<<rating<<" "<<bestRating<<endl;
//        }
//        else
//            success++;
//        
//        sum = sum+rmse;
        if(rating>=threshold && cur1.state[3]>=threshold)
            tp++;
        else if(rating<threshold && cur1.state[3]>=threshold)
            fn++;
        else if(rating>=threshold && cur1.state[3]<threshold)
            fp++;
        else if(rating<threshold && cur1.state[3]<threshold)
            tn++;
        
         if(rating!=cur1.state[3])
         {
             error++;
             rmse = pow((rating - cur1.state[3]),2);
             //cout<<rating<<" "<<cur.state[3]<<endl;
         }
        else
            success++;
    
        sum = sum+rmse;
    }
    Real tot = error+success;
    outfile<<error<<" "<<success<<" "<<(error/tot)<<" "<<sqrt(sum/tot)<<" "<<tp<<" "<<tn<<" "<<fp<<" "<<fn;
//    outfile<<error<<" "<<success<<" "<<"error = "<<(error/tot)<<" rmse = "<<sqrt(sum/tot);
//    outfile<<" tp = "<<tp<<" tn = "<<tn<<" fp = "<<fp<<" fn = "<<fn<<" ";

time(&timer2);
double t = difftime(timer2,timer1);
(clock() - tStart)/CLOCKS_PER_SEC;
double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
    outfile<<" "<<t<<" "<<t_c<<endl;
//outfile<<"Time "<<t<<"s"<<" ";
//outfile<<" Processor "<<t_c<<"s"<<endl;
outfile.close();	
   // outfile1.close();
    return 0;
}
