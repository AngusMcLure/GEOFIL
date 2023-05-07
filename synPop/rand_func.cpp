//
//  basic.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "paras.h"
using namespace std;
extern mt19937 gen;

double gaussian(double m_mu, double s_sigma){ //Currently unused so not updated
    normal_distribution<double> distribution(m_mu, s_sigma);
    
    return distribution(gen);
}

int ztpoisson(double l_lambda){
    double r = random_real();
    
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
	binomial_distribution<int> distribution(r, p);
    return distribution(gen);
}

int poisson(double rate){
    poisson_distribution<int> distribution(rate);

    return distribution(gen);
}

double normal(double mean, double stddev){
    normal_distribution<double> distribution(mean, stddev);

    return distribution(gen);
}

double random_real(){
    uniform_real_distribution<> distribution(0.0,1.0);

    return distribution(gen);
}