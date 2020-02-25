//
//  basic.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "paras.h"
using namespace std;

//These all set their own seeds every time they are called! Rework to:
//      Set from a single seed for each simulation
//      switch to a better random number generator (like merseene twister or something)
//      switch out all the other random number generators being used for the modern equivalents (i.e. replace rand48, rand, others?)

//functions to generate random numbers with different distributions
double gaussian(double m_mu, double s_sigma){
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<double> distribution(m_mu, s_sigma);
    
    return distribution(generator);
}

int ztpoisson(double l_lambda){
    double r = drand48();
    
    int k = 1;
    double t = l_lambda/(exp(l_lambda)-1), s = t;
    while(s < r){
        ++k;
        t *= l_lambda/k;
        s += t;
    }
    return k;
}

double ztpoisson(int k, double l_lambda){
    double p = 1/(exp(l_lambda)-1);
    for(int j = 1; j <= k; ++j){
        p *= l_lambda/j;
    }
    return p;
}

int binomial(int r, double p){
	unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::binomial_distribution<int> distribution(r, p);
    
    return distribution(generator);
}

int poisson(double rate){
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::poisson_distribution<int> distribution(rate);
    
    return distribution(generator);
}

int negbinomial(double rate){
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::poisson_distribution<int> distribution(rate);
    
    return distribution(generator);
}

int L3LarvaePerMos(){
    // Generate a random number of L3 larvae in an infected mosquito. Modelled using a discritised version of the two parameter Weibull (approximates a zero truncated neg binomial reasonably well and comes builtin to std)
    // if the zero-truncated binomial is a better fit to the data we could probably generate these using a poisson-gamma mixture?
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::weibull_distribution<double> distribution(0.72666035,3.22927636);
    return int(distribution(generator)) + 1;
}
