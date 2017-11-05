//
//  agent.hpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef agent_hpp
#define agent_hpp

#include "headers.h"
using namespace std;

class agent{
public:
    int aid;
    int age;            //in days
    char gendr;        //m - male, f - female
    
    char hrole;    //h - holder, s - spouse, c - child, r - relatives, n - nonrelatives
    char works; //w - working, s - schooling, h - housing
    char margs; //u - unmarried, m - married, s - single
    int dtime;     //
    int bwind;   //birth window closed for female just having a baby
    
    char epids;     //s - susceptible, e - exposed, i - infectious, r - removed, d - dead
    
    agent *spw;
    map<string, agent*> chldr;
    map<string, agent*> parnt;
    
    hhold *hd;    //household
    workp *wp;    //workplace
    schol *sh;    //school
    mblok *mbk;
    cblok *cbk;
    
    agent(int aid, int age = -1, char gendr = '-', char margs = 'u', hhold *hd = NULL, workp *wp = NULL, schol *sh = NULL, mblok *mbk = NULL, cblok *cbk = NULL);
    
    ~agent();
    
    void add_child(agent *p);
    void add_parent(agent *p);
    void clr_relation();
};

#endif /* agent_hpp */
