//
//  basic.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include <iostream>
#include <random>
#include <ctime>

double gaussian(double mu, double sigma){
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<double> distribution (mu, sigma);
    
    return distribution(generator);
}

int ztpoisson(double lambda){
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(seed);
    double r = drand48();
    
    int k = 1;
    double t = lambda/(exp(lambda)-1), s = t;
    while(s < r){
        ++k;
        t *= lambda/k;
        s += t;
    }
    return k;
}
