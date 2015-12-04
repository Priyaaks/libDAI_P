/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/factorgraph.h>
#include <iostream>
#include <fstream>
#include <dai/util.h>
#include <dai/alldai.h>
#include <fstream>
#include "dai/emrun.h"
#include <dai/util.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <iomanip>      // std::setprecision
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include<math.h>
#include <string>
#include <iostream>

#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
using namespace std;
using namespace dai;
// Define a random number generator and initialize it with a reproducible
// seed.
mt19937 generator(42);

// Define a uniform random number distribution which produces "double"
// values between 0 and 1 (0 inclusive, 1 exclusive).
boost::uniform_real<> uni_dist(0,1);
boost::variate_generator<mt19937&, boost::uniform_real<> > uni(generator, uni_dist);
int main() {
    // This example program illustrates how to construct a factorgraph
    // by means of the sprinkler network example discussed at
    // http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html

    Var C(0, 2);  // Define binary variable Cloudy (with label 0)
    Var S(1, 2);  // Define binary variable Sprinkler (with label 1)
    Var R(2, 2);  // Define binary variable Rain (with label 2)
    Var W(3, 2);  // Define binary variable Wetgrass (with label 3)

    // Define probability distribution for C
    Factor P_C( C );
    P_C.set(0, 0.5);   // C = 0
    P_C.set(1, 0.5);   // C = 1
    
    // Define conditional probability of R given C
    Factor P_R_given_C( VarSet( R, C ) );
    P_R_given_C.set(0, 0.8);   // C = 0, R = 0
    P_R_given_C.set(1, 0.2);   // C = 1, R = 0
    P_R_given_C.set(2, 0.2);   // C = 0, R = 1
    P_R_given_C.set(3, 0.8);   // C = 1, R = 1

    // Define conditional probability of S given C
    Factor P_S_given_C( VarSet( S, C ) );
    P_S_given_C.set(0, 0.5);   // C = 0, S = 0
    P_S_given_C.set(1, 0.9);   // C = 1, S = 0
    P_S_given_C.set(2, 0.5);   // C = 0, S = 1
    P_S_given_C.set(3, 0.1);   // C = 1, S = 1

    // Define conditional probability of W given S and R
    Factor P_W_given_S_R( VarSet( S, R ) | W );
    P_W_given_S_R.set(0, 1.0);  // S = 0, R = 0, W = 0
    P_W_given_S_R.set(1, 0.1);  // S = 1, R = 0, W = 0
    P_W_given_S_R.set(2, 0.1);  // S = 0, R = 1, W = 0
    P_W_given_S_R.set(3, 0.01); // S = 1, R = 1, W = 0
    P_W_given_S_R.set(4, 0.0);  // S = 0, R = 0, W = 1
    P_W_given_S_R.set(5, 0.9);  // S = 1, R = 0, W = 1
    P_W_given_S_R.set(6, 0.9);  // S = 0, R = 1, W = 1
    P_W_given_S_R.set(7, 0.99); // S = 1, R = 1, W = 1

    // Build factor graph consisting of those four factors
    vector<Factor> SprinklerFactors;
    SprinklerFactors.push_back( P_C );
    SprinklerFactors.push_back( P_S_given_C );
    SprinklerFactors.push_back( P_R_given_C );
    SprinklerFactors.push_back( P_W_given_S_R );
    FactorGraph SprinklerNetwork( SprinklerFactors );
    FactorGraph SprinklerNetwork1( SprinklerFactors );
    // Write factorgraph to a file
    SprinklerNetwork.WriteToFile( "sprinkler.fg" );
    cout << "Sprinkler network written to sprinkler.fg" << endl;

    // Output some information about the factorgraph
    cout << SprinklerNetwork.nrVars() << " variables" << endl;
    cout << SprinklerNetwork.nrFactors() << " factors" << endl;

 //Code for mutation
    for( size_t I = 0; I < SprinklerNetwork.nrFactors(); I++ )
    {
        int no_states = SprinklerNetwork.factor( I ).nrStates();// total no. of states
        int no_arr_arrset =0; // factor_states
        int arr_size =1; //states of parents before factor_pos
        int arr_set =0; //states of parents after factor_pos
        int arr_set_states =0; // no. of states till factor_pos
        
        cout<<SprinklerNetwork.factor( I ).nrStates()<<endl;
        Factor P = SprinklerNetwork.factor( I );
        VarSet vs = P.vars();
        cout<<"vars = "<<vs<<endl;
        cout << "{";
        for( VarSet::const_iterator v = vs.begin(); v != vs.end(); v++ )
        {
            cout << (v != vs.begin() ? ", " : "") << *v;
            if(v->label() == I)
            {
                cout<<" states = "<<v->states()<<endl;
                no_arr_arrset = v->states();
                arr_set_states = arr_size * v->states();
                break;
            }
            arr_size = arr_size * v->states();
        }
        cout << "}"<<endl;
        arr_set = no_states/arr_set_states;
        dai::TProb<double> myvector = P.p();
        
        for (dai::TProb<double>::iterator it = myvector.begin();it != myvector.end(); ++it)
            std::cout << ' ' << *it;
        std::cout << '\n';
//        cout<<"no. of states "<<no_states<<"\t"<<"array set "<<arr_set<<"\t"<<"no_arr_arrset "<<no_arr_arrset<<"\t"<<"array size "<<arr_size<<endl;
        int k=0;
        while(k<arr_set)
        {
            //Random values are generated---~----~----
            std::vector< double * > Arrays1;
            for(int l=0; l<no_arr_arrset; l++)
            {
                Arrays1.push_back( new double[arr_size]);
            }
            vector<double*>::iterator it;
            for(int m=0; m<arr_size; m++)
            {
                it=Arrays1.begin();
                std::vector<double> vec;
                for(int i=0; i<no_arr_arrset; i++)
                {
                    vec.push_back(uni());
                }
                const double total = std::accumulate(vec.begin(), vec.end(), 0.0);
                for (double& value: vec)
                {
                    double* arr = *it;
                    value /= total;
                    arr[m] = value;
                    ++it;
                    //cout<<value <<endl;
                }
                // cout<<endl;
                vec.clear();
            }
            //Random values are generated---x----x----
            //Random or actual values will be updated -----~------~
            vector<double*>::iterator it11;
            it11=Arrays1.begin();
            int stat=0;
            for(int l=0; l<no_arr_arrset; l++)
            {
                double* arr = *it11;
                for(int m=0; m<arr_size; m++)
                {
                    stringstream v;
                    v << fixed << setprecision(12) << arr[m];
                    //c1<<stat<<"   "<<v.str()<<endl;
                    //cout<< "replaced"<<v.str()<<endl;
                    P.set(stat,arr[m]);
                    stat++;
                }
                ++it11;
            }
            //Random values are updated ------x------x--------
            k++;
        }
        
        SprinklerNetwork.setFactor(I,P);
        dai::TProb<double> myvector1 = P.p();
        
        for (dai::TProb<double>::iterator it1 = myvector1.begin() ; it1 != myvector1.end(); ++it1)
            std::cout << ' ' << *it1;
        std::cout << '\n';
        
    }
    // Code for crossover
    
    int population_size=2;
    // compute crossover probability
    double pcross = uni(); // generates random a value between 0 and 1
	cout<<"Crossover probability "<<pcross;
	// 4. Select 2 parents randomly
	const int LOW = 0;
	const int HIGH = population_size-1;
    set<int> myset;
	set<int>::iterator it;
    int *arrval;
	size_t p=0;
	arrval = new int[population_size];
    /*Declare variable to hold seconds on clock.*/
	time_t seconds;
	/*Get value from system clock and place in seconds variable.*/
	time(&seconds);
	/*Convert seconds to a unsigned integer.*/
	srand(time(0));
    for( size_t n = 0; n < population_size; n++ )
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
                cout <<" Random value is"<<arrval[n]<<" and "<< arrval[n-1];
                // randomly select a crossover point
                int cr_pt = (rand() % SprinklerNetwork.nrVars()) +1;
                int c = 0; // count of parameters
                int p_c1 = 0, p_c2 =0; // counter to skip the first four config lines in the cpt of the fg file
                cout<<" Crossover point is "<< cr_pt<<endl;
                p=0;
                for( size_t I = cr_pt; I < SprinklerNetwork.nrFactors(); I++ )
                {
                    Factor P = SprinklerNetwork.factor( I );
                    dai::TProb<double> myvector = P.p();
                    Factor P1 = SprinklerNetwork1.factor( I );
                    dai::TProb<double> myvector1 = P1.p();
                    dai::TProb<double>::iterator it1 = myvector1.begin();
                    dai::TProb<double>::iterator it = myvector.begin();
                    for (int state=0;state<myvector.size();state++)
                    {
                        P.set(state,*it1);
                        P1.set(state,*it);
                        ++it1;++it;
                    }
                    SprinklerNetwork.setFactor(I,P);
                    SprinklerNetwork1.setFactor(I,P1);
                }
            }
        }
        else
        {
            n=n-1;
            continue;
        }
    }
    // Calculate joint probability of all four variables
    Factor P2;
    for( size_t I = 0; I < SprinklerNetwork.nrFactors(); I++ )
    {
        cout<<SprinklerNetwork.factor( I ).nrStates()<<endl;
        P2 = SprinklerNetwork.factor( I );
        dai::TProb<double> myvector = P2.p();

        for (dai::TProb<double>::iterator it = myvector.begin() ; it != myvector.end(); ++it)
            std::cout << ' ' << *it;
        std::cout << '\n';
    }
    std::cout << '\n';
    Factor P3;
    for( size_t I = 0; I < SprinklerNetwork1.nrFactors(); I++ )
    {
        cout<<SprinklerNetwork1.factor( I ).nrStates()<<endl;
        P3 = SprinklerNetwork1.factor( I );
        dai::TProb<double> myvector = P3.p();
        
        for (dai::TProb<double>::iterator it = myvector.begin() ; it != myvector.end(); ++it)
            std::cout << ' ' << *it;
        std::cout << '\n';
    }
//        P.randomize();
//        P.normalized();
//        dai::TProb<double> myvector1 = P.p();
//        
//        for (dai::TProb<double>::iterator it1 = myvector1.begin() ; it1 != myvector1.end(); ++it1)
//            std::cout << ' ' << *it1;
        std::cout << '\n';
//        VarSet vs = P.vars();
//        cout<<"vars = "<<vs<<endl;
//        cout << "{";
//        for( VarSet::const_iterator v = vs.begin(); v != vs.end(); v++ )
//        {
//            cout << (v != vs.begin() ? ", " : "") << *v;
////            std::string result,result1;
////            result = name + boost::lexical_cast<std::string>(I);
////            result1 = ""+boost::lexical_cast<std::string>(*v);
////            cout<<" factor = "<<result<<" "<<v->label()<<endl;
//            
//            if(v->label() == I)
//            {
//                cout<<" states = "<<v->states()<<endl;
//            }
//        }
//        cout << "}"<<endl;
        //size_t f =SprinklerNetwork.findVar(v);
        //cout<<"factor index "<<f<<endl;
       // P *= SprinklerNetwork.factor( I );
//    }// P.normalize();  // Not necessary: a Bayesian network is already normalized by definition
//    // Calculate some probabilities
//    Real denom = P.marginal( W )[1];
//    //VarSet v = P.vars();
//    //cout<<"vars = "<<v<<endl;
//   
//    //for( size_t n = 0; n < v.nrStates(); X_n++ )
//        //cout<<" vars inside "<<*X_n<<endl;
//    //cout<<P.marginal( *X_n )<<" "<< P.marginal( W )[1]<<endl;
//    cout<<P.normalize()<<endl;
//    cout << "P(W=1) = " << denom << endl;
//    cout << "P(S=1 | W=1) = " << P.marginal( VarSet( S, W ) )[3] / denom << endl;
//    cout << "P(R=1 | W=1) = " << P.marginal( VarSet( R, W ) )[3] / denom << endl;
//
    return 0;
}
