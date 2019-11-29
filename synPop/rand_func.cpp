//
//  basic.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "paras.h"
using namespace std;

//function to generate random numbers with different distribution
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
