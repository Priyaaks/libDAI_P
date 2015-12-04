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

using namespace std;
using namespace dai;
//./example_Newasia.em ./500/coopy.tab out.log alarm.em alarm_1.fg out1.log

size_t getStates(dai::Factor P,size_t factor_no)
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

    ofstream outfile;
    outfile.open(argv[2],ios::out | ios::app);
    ofstream outfile1;
    outfile1.open(argv[5],ios::out | ios::app);
    Evidence e;
    ifstream estream(argv[1]);
    e.addEvidenceTabFile( estream, NewasiaNetwork );
    ifstream emstream( argv[3] );
    EMAlg em(e, *inf, emstream);
    Real l;
	//cout<<"Starting EM"<<endl;
// Iterate EM until convergence
    int i=0;
//    stringstream ss2(argv[6]);
//    string alarmfgloc = ss2.str();
    double params[37];
    vector<FactorGraph> fgTrace;
    
   while( !em.hasSatisfiedTermConditions() ) {
       l = em.iterate();
       outfile<<l<<" ";
       ofstream outfile2;
 //      char* filename;`//       std::string s = std::to_string(i);
//       string fgfileloc =  alarmfgloc +"_"+s+".log";
//       filename = &fgfileloc[0];
//       outfile2.open(filename,ios::out | ios::app);
       cout<<l<<" ************ "<<i<<endl;
//       outfile2 << inf->fg();
       FactorGraph fg1 = inf->fg();
       fgTrace.push_back(fg1);
//              outfile2.close();
       i++;
    }
//    FactorGraph fg1,fg2;
//    vector<FactorGraph>::iterator it = fgTrace.begin();
//    fg1 = *it;
//    hash_map<int, double> hashmap;
//    //`cout<<" i am here "<<endl;
//    for(int i=0;i<37;i++)
//    {
//        hashmap[i]=0;
//       // cout<<i<<" HERE "<<hashmap.at(i)<<endl;
//    }
//    while(it != fgTrace.end())
//    {
//        fg2 = *it++;
//        for( size_t I = 0; I < fg1.nrFactors(); I++ ) {
//            for( size_t I1= 0; I1 < fg2.nrFactors(); I1++ ) {
//                cout<<fg1.factor(I)[0]<<" "<<fg2.factor(I)[0]<<endl;
//                if(I==I1)
//                {
//                   cout<<" factor "<<I;
////                    cout<<" "<<dist(fg2.factor(I1),fg1.factor(I),DISTKL)<<endl;
//                    params[I] = params[I]+dist(fg2.factor(I1),fg1.factor(I),DISTKL);
//                    size_t states = getStates(fg2.factor(I1),I1);
//                    //cout<<hashmap.at(I)<<endl;
//                    double val = hashmap.at(I);
//                    hashmap.at(I) = val+(dist(fg2.factor(I1),fg1.factor(I),DISTKL)/states);
//                    break;
//                }
//            }
//        }
//        fg1 = fg2;
//
//    }
////
//////    
//////    hash_map<size_t, size_t> hashmap;
//    cout<<"hash map"<<endl;
//    for(int i=0;i<fg2.nrFactors();i++)
//    {
//        cout<<hashmap.at(i)<<endl;
//    }
//    sort(params,params+37);
//    cout<<"params "<<endl;
//    for(int i=0;i<37;i++)
//    {
//        
//        cout<<i<<" "<<params[i]<<endl;
//    }
   // l = em.run1();
    outfile << argv[1] << " " << argv[4]<<" "<<em.Iterations()<<" " << l << endl;
   /* outfile << endl << "True factor graph:" << endl << "##################" << endl;
    outfile.precision(12);
    outfile << NewasiaNetwork;
   // Output learnedfactorgraph  */                                                                                                                                                                       
    //outfile1 << endl << "Learned factor graph:" << endl << "#####################" << endl;
     outfile1.precision(4);
     outfile1 << inf->fg();
time(&timer2);
double t = difftime(timer2,timer1);
(clock() - tStart)/CLOCKS_PER_SEC;
double t_c = (clock() - tStart)/CLOCKS_PER_SEC;
outfile<<"Time taken: "<<t<<"s"<<endl;
outfile<<" Processor Time taken: "<<t_c<<"s"<<endl;
     delete inf;
    outfile.close();	
    outfile1.close();
    return 0;
}
