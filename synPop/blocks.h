//
//  blocks.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef blocks_hpp
#define blocks_hpp

#include "headers.h"
using namespace std;

class mblok{
public:
    int mid;                            //mblock id
    double lat, log;                    //latitude & longitude
    cblok *cb;                          //city block
    
    map<int, agent*> mblok_pop;         //population
    
    map<int, hhold*> mblok_hholds;
    
    //map<int, rbldg*> mblok_rbldgs_occupy;
    //map<int, rbldg*> mblok_rbldgs_vacant;
    map<int, rbldg*> mblok_rbldgs;
    map<int, bbldg*> mblok_bbldgs;
    
    void add_hhold(hhold *p);
    void add_agent(agent *p);
    void rmv_hhold(hhold *p);
    void rmv_agent(agent *p);
    void bld_hhold(pf f);
    
    mblok(int mid, cblok *ct);
};

class cblok{
public:
    int cid;                            //cblock id
    double lat, log;
    
    int cpop;
    int next_aid;
    map<int, agent*> cblok_pop;
    map<int, agent*> fmal_marrid;       //married females
    
    int next_hid;
    map<int, hhold*> cblok_hholds;
    map<int, rbldg*> cblok_rbldgs;
    map<int, rbldg*> cblok_rbldgs_vcnt; //vacant residential buildings
    map<int, bbldg*> cblok_bbldgs;
    
    map<int, mblok*> mbloks;
    map<string, int> mbloksIndex;       //meshblocks in cblocks
    double **euclid_dist;               //euclidean distance between meshblocks
    double **road_dist;                 //road distance between meshblocks
    
    //parameters from file inputs
    int hholdtypes[unit_types];         //number of each type household
    map<int, int> mblok_mpops;          //male pop in each mblok
    map<int, int> mblok_fpops;          //female pop in each mblok
    map<int, int> mblok_hholds;         //hholds in each mblok
    map<int, agrps*> mblok_agrps;       //population by age group;
    
    int malebyage[16];                  //males by age groups
    int fmalbyage[16];                  //females by age groups
    double mmortlty[18];                //male mortatlity
    double fmortlty[18];                //female mortatlity
    double sexratio[16];                //sex ratios by age groups
    double fertlty[sim_nd-sim_bg+1][6]; //fertility by age groups
    int pop_loss[sim_nd-sim_bg+1];      //net annual population loss due to migration
    
    
    cblok(string datasets);
    void reset(string datasets);
    void read_data(string data, char type);
    void bld_cblok_pop();
    void add_agent(agent *p);
    void add_mblok(mblok *p);
    void bld_cblok_hhold();
    void hndl_land_data();
    void allct_rbldgs();
    void allct_bbldgs();
    void calc_bldg_dist();
    void radt_model(char d);
    void hndl_birth(int t);
    void hndl_death(int t);
    void hndl_marrg(int t);
    void hndl_divrc(int t);
    void hndl_ploss(int t);
    void hndl_mvout(int t);
    void hndl_mvgin(int t);
    void hndl_hldfrc(int t);
    void sim_pop(int t);
    void get_pop(int t);
    void get_hhold(int t);
    void get_sexratio(int t);
    void get_sexratiob(int t);
    void get_geographic(int t);
    void get_bbldgarea();
    void vldt_hhold(string str);
    void prt_hhold(std::ofstream &out, hhold* hh);
    void hndl_rbldg(string ff, int low, int upper, int min_dist);
    void hndl_bbldg(string ff, int low, int upper, int min_dist);
};

#endif /* blocks_hpp */
