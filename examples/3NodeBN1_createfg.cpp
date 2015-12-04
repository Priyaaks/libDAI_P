/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
 */


#include <dai/factorgraph.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace dai;

int main() {
    // This example program illustrates how to construct a factorgraph
    // by means of the sprinkler network example discussed at
    // http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html

    Var C(0, 2);  // Define binary variable Cloudy (with label 0)
    Var S(1, 2);  // Define binary variable Sprinkler (with label 1)
    Var D(2, 2);  // Define binary variable Sprinkler (with label 2)
    
    // Define probability distribution for C
    Factor P_C( C );
    P_C.set(0, 0.5);   // C = 0
    P_C.set(1, 0.5);   // C = 1

    // Define conditional probability of S given C
    Factor P_S_given_C( VarSet( S, C ) );
    P_S_given_C.set(0, 0.5);   // C = 0, S = 0
    P_S_given_C.set(1, 0.9);   // C = 1, S = 0
    P_S_given_C.set(2, 0.5);   // C = 0, S = 1
    P_S_given_C.set(3, 0.1);   // C = 1, S = 1

    // Define conditional probability of D given S
    Factor P_D_given_S( VarSet( D, S ) );
    P_D_given_S.set(0, 0.3);   // S = 0, D = 0
    P_D_given_S.set(1, 0.9);   // S = 1, D = 0
    P_D_given_S.set(2, 0.7);   // S = 0, D = 1
    P_D_given_S.set(3, 0.1);   // S = 1, D = 1

   // Build factor graph consisting of those four factors
    vector<Factor> SprinklerFactors;
    SprinklerFactors.push_back( P_C );
    SprinklerFactors.push_back( P_S_given_C );
    SprinklerFactors.push_back( P_D_given_S );
    FactorGraph SprinklerNetwork( SprinklerFactors );

    // Write factorgraph to a file
    SprinklerNetwork.WriteToFile( "3NodeBN1.fg" );
    cout << "Sprinkler network written to 3NodeBN1.fg" << endl;

    // Output some information about the factorgraph
    cout << SprinklerNetwork.nrVars() << " variables" << endl;
    cout << SprinklerNetwork.nrFactors() << " factors" << endl;

    // Calculate joint probability of all four variables
    Factor P;
    for( size_t I = 0; I < SprinklerNetwork.nrFactors(); I++ )
        P *= SprinklerNetwork.factor( I );
    // P.normalize();  // Not necessary: a Bayesian network is already normalized by definition

    // Calculate some probabilities
    /*Real denom = P.marginal( W )[1];
    cout << "P(W=1) = " << denom << endl;
    cout << "P(S=1 | W=1) = " << P.marginal( VarSet( S, W ) )[3] / denom << endl;
    cout << "P(R=1 | W=1) = " << P.marginal( VarSet( R, W ) )[3] / denom << endl;
*/
    return 0;
}

