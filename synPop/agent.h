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

//sites
class hhold;        //household
class workp;        //workplace
class schol;        //school

//buildings
class rbldg;        //residential
class wbldg;        //work buildings
class sbldg;        //school bldgs

class agent{
public:
    int aid;
    int age;        //in days
    char gendr;     //M - male, F - female
    
    char works;     //W - working, S - schooling, H - housing
    char margs;     //U - unmarried, M - married, S - single
    int bwind;      //birth window closed for female just having a baby
    
    char epids;     //S - susceptible, E - exposed, I - infectious, R - removed, D - dead
    
    agent *spw;
    map<int, agent*> chldr;
    map<int, agent*> parnt;
    
    hhold *hd;      //household
    workp *wp;      //workplace
    schol *sh;      //school
    
    agent(int aid, int age = -1, char gendr = '-', char margs = 'u', hhold *hd = NULL, workp *wp = NULL, schol *sh = NULL);
    
    ~agent();
    
    void add_child(agent *p);
    void add_parent(agent *p);
    void clr_sociallinks();
};

#endif /* agent_hpp */
