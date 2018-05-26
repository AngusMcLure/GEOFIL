//
//  agent.hpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef agent_hpp
#define agent_hpp

#include "paras.h"
using namespace std;

//individuals
class agent;
class hhold;
class workp;
class schol;

class agent{
public:
    int aid;
    int age;        //in days
    char gendr;     //m - male, f - female
    
    char margs;     //m - married, s - single, d - divorce, w - widowed
    int bth_wind;   //birth window closed for female just having a baby
    
    int worms;
    char epids;     //s - susceptible, e - prepatent, i - infectious
    int clock_pre;
    int clock_inf;
    int active_len; //active period in days, 4-6 years
    int clock_rmv;
    
    agent *spw;
    agent *dad;
    agent *mom;
    
    int births;
    map<int, agent*> chdr;
    
    hhold *h_d;      //household
    workp *w_p;      //workplace
    schol *s_h;      //school
    
    agent(int aid, int age = -1, char gendr = '-', char margs = 's', hhold *h_d = NULL, workp *w_p = NULL, schol *s_h = NULL);
    
    ~agent();
    
    void add_child(agent *p);
    void calc_risk(double prv, char time);
    void renew_epidemics();
};

#endif /* agent_hpp */
