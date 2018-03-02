//
//  bldgs.h
//  synPop
//
//  Created by Sting Xu on 22/1/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#ifndef bldgs_h
#define bldgs_h

#include "agent.h"
using namespace std;

class rbldg;        //residential
class workp;        //work place
class schol;        //school

class mblok;
class cblok;

struct r_node{
    double f;
    double p;
    r_node(double f, double p):f(f),p(p){}
};

class rbldg{
public:
    int bid;
    double log, lat;
    double area;
    hhold *h_d;
    mblok *mbk;
    cblok *cbk;
    
    vector<rbldg*> r_neigh;
    vector<workp*> w_neigh;
    vector<schol*> s_neigh;
    
    double day_p, night_p, t_f;
    vector<r_node*> r_vec;
    
    rbldg(int bid, double log, double lat, double area, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->h_d = NULL;
        this->mbk = mbk;
        this->cbk = cbk;
        
        r_neigh.clear();    r_neigh.shrink_to_fit();
        w_neigh.clear();    w_neigh.shrink_to_fit();
        s_neigh.clear();    s_neigh.shrink_to_fit();
        
        day_p = 0;  night_p = 0;    t_f = 0;
        r_vec.clear();      r_vec.shrink_to_fit();
    }
    
    ~rbldg(){
        h_d = NULL;
        mbk = NULL;
        cbk = NULL;
        
        r_neigh.clear();    r_neigh.shrink_to_fit();
        w_neigh.clear();    w_neigh.shrink_to_fit();
        s_neigh.clear();    s_neigh.shrink_to_fit();
        r_vec.clear();      r_vec.shrink_to_fit();
    }
};

class workp{
public:
    int wid;
    double lat, log;
    double area;
    mblok *mbk;
    cblok *cbk;
    
    vector<rbldg*> r_neigh;
    vector<workp*> w_neigh;
    vector<schol*> s_neigh;
    
    double day_p, t_f;
    vector<r_node*> r_vec;
    
    map<int, agent*> workers;
    
    workp(int wid, double log, double lat, double area, mblok *mbk, cblok *cbk){
        this->wid = wid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->mbk = mbk;
        this->cbk = cbk;
        
        workers.clear();
        
        r_neigh.clear();    r_neigh.shrink_to_fit();
        w_neigh.clear();    w_neigh.shrink_to_fit();
        s_neigh.clear();    s_neigh.shrink_to_fit();
        
        day_p = 0;  t_f = 0;
        r_vec.clear();      r_vec.shrink_to_fit();
    }
    
    ~workp(){
        mbk = NULL;
        cbk = NULL;
        
        workers.clear();
        
        r_neigh.clear();    r_neigh.shrink_to_fit();
        w_neigh.clear();    w_neigh.shrink_to_fit();
        s_neigh.clear();    s_neigh.shrink_to_fit();
    
        r_vec.clear();      r_vec.shrink_to_fit();
    }
};

class schol{
public:
    int sid;
    string name;
    char level;
    double lat, log;
    double radius;
    
    map<int, agent*> student;
    
    vector<rbldg*> r_neigh;
    vector<workp*> w_neigh;
    vector<schol*> s_neigh;
    
    double day_p, t_f;
    vector<r_node*> r_vec;
    
    schol(int sid, string name, char level, double log, double lat){
        this->sid = sid;
        this->name = name;
        this->level = level;
        this->lat = lat;
        this->log = log;
        
        this->student.clear();
        
        r_neigh.clear();    r_neigh.shrink_to_fit();
        w_neigh.clear();    w_neigh.shrink_to_fit();
        s_neigh.clear();    s_neigh.shrink_to_fit();
        
        day_p = 0;  t_f = 0;
        r_vec.clear();      r_vec.shrink_to_fit();
    }
    
    ~schol(){
        this->student.clear();
        this->name.clear();
        
        r_neigh.clear();    r_neigh.shrink_to_fit();
        w_neigh.clear();    w_neigh.shrink_to_fit();
        s_neigh.clear();    s_neigh.shrink_to_fit();
        r_vec.clear();      r_vec.shrink_to_fit();
    }
    
    void add_stdt(agent *p);
    void rmv_stdt(agent *p);
    void update();
    void removd();
};

#endif /* bldgs_h */
