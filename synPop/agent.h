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
class worm;
class agent;
class hhold;
class workp;
class schol;
class drugs;
class mda_strat;

// LF mated worms
class worm{
public:
    char status;    //p - prepatent, m - mature, d - to be removed
    int clock_pp;
    int clock_mf; //active period in days, 4-6 years
    
    int clock_mda;
    double mda_f; // the infectiousness of the worm. Defaults to 1. can be reduced by mda
    char gender;
    
    worm(char s, int p, int l, char g){
        status = s;
        clock_pp = p;
        clock_mf = l;
        
        clock_mda = 0;
        mda_f = 1.0; //worms have fertility 1 by default. This can be reduced by MDA
        gender = g;
    }
    
    void update();
};

// human individual
class agent{
public:
    int aid;
    int age;        //in days
    char gendr;     //m - male, f - female
    
    char margs;     //m - married, s - single, d - divorce, w - widowed
    int bth_wind;   //birth window closed for female just having a baby
    
    vector<worm*> wvec;
    int clock_inf;
    double mda_f; // the relative infectiousness of the host assuming they have an fertile male worm. 1 is default. However can be reduced if all female worms are partially sterilised. mda_f is the fertility of the most fertile female
    char epids;     //s - susceptible, e - prepatent, i - infectious, u - infected with 'U'nmated worm (not infectious but antigen positive)
    double LastDayWithAdultWorm; // For people who are not infected with an adult worm, to keep track of the last day on which they were infected with an adult worm. Defaults to negative infinity (integer doesn't have one). Is set to the current day whenever someone goes from the i or u states to the e or s states. It is not particuarly meaningful for people who are currently infected with an adult worm
    bool ChangedEpiToday; //whether or not the epi status of the person changed today -- we keep track of this so that we don't update these people multiple times a day
    
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
    void sim_bites(double prv, char time, double c, double ProbOneSex, double ProbBothSex);
    void update(int year, int day);
    void get_drugs(drugs drug);
    void write_line_list(int SimNum, int y, int s);
};

#endif /* agent_hpp */
