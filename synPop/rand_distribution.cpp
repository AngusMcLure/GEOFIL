//
//  basic.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "paras.h"
using namespace std;

int gaussian(double m_mu, double s_sigma){
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<double> distribution(m_mu, s_sigma);
    
    return int(distribution(generator)+0.5);
}

int ztpoisson(double l_lambda){
    double r = drandom();
    
    int k = 1;
    double t = l_lambda/(exp(l_lambda)-1), s = t;
    while(s < r){
        ++k;
        t *= l_lambda/k;
        s += t;
    }
    return k;
}

int binomial(int r, double p){
	unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::binomial_distribution<int> distribution(r, p);
    
    return distribution(generator);
}

double drandom(){
	//return drand48();
	return rand() / (double)RAND_MAX;
}

int irandom(){
    return rand();
}