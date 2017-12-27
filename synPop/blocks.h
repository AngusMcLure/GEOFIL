//
//  blocks.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef blocks_hpp
#define blocks_hpp

#include "agroups.h"
#include "sites.h"
using namespace std;

class mblok{
public:
    int mid;                            //mblock id
    double lat, log;                    //latitude & longitude
    cblok *cbk;                          //city block
    
    map<int, agent*> mblok_pop;         //population
    
    map<int, hhold*> mblok_hholds;
    
    //map<int, rbldg*> mblok_rbldgs_occupy;
    //map<int, rbldg*> mblok_rbldgs_vacant;
    map<int, rbldg*> mblok_rbldgs;
    map<int, wbldg*> mblok_wbldgs;
    
    void intlz_pop();
    void add_hhold(hhold *p);
    void add_agent(agent *p);
    void rmv_hhold(hhold *p);
    void rmv_agent(agent *p);
    void bld_hhold(pf f);
    void bld_pop(int mm, int ff, agrps *pp);
    void adpt_chldrs(hhold *p);          //all members in p are adopted
    
    mblok(int mid, cblok *cbk, double lat = 0, double log = 0);
    ~mblok();
};

class cblok{
public:
    int cid;                            //cblock id
    string cname;                       //cblock name
    double lat, log;
    
    int cpop;
    int next_aid;
    map<int, agent*> cblok_pop;
    map<int, agent*> fmal_marrd;       //married females
    
    int next_hid;
    //map<int, hhold*> cblok_hholds;
    //map<int, rbldg*> cblok_rbldgs;
    map<int, rbldg*> cblok_rbldgs_vcnt; //vacant residential buildings
    //map<int, wbldg*> cblok_wbldgs;
    
    int next_mid, meshblocks;
    map<int, mblok*> mbloks;
    map<string, int> mbloksIndexA;       //meshblocks in cblocks
    map<int, string> mbloksIndexB;      
    map<int, double*> mblok_crdnt;      //meshblock coordinates
    double **euclid_dist;               //euclidean distance between meshblocks
    double **road_dist;                 //road distance between meshblocks
    
    //parameters from file inputs
    int hhold_types[unit_types];         //number of each type household
    map<int, int> mblok_mpops;          //male pop in each mblok
    map<int, int> mblok_fpops;          //female pop in each mblok
    map<int, int> mblok_hholds;         //hholds in each mblok
    map<int, agrps*> mblok_agrps;       //mblok pop by age group;
    
    int male_by_agrp[16];               //males by age groups
    int fmal_by_agrp[16];               //females by age groups
    int male_by_age[80];                //males by singular age - triangle smoothed
    int fmal_by_age[80];                //females by singlular age - triangle smoothed
    
    double male_marital_age[int((max_marital_age - min_marital_age)/5)+1];
    double fmal_marital_age[int((max_marital_age - min_marital_age)/5)+1];
    double male_marital_prob[max_marital_age-min_marital_age+1];          //male marriage probability by age
    double fmal_marital_prob[max_marital_age-min_marital_age+1];          //female marriage probability by age
    
    double mmortlty[18];                //male mortatlity
    double fmortlty[18];                //female mortatlity
    
    double sex_ratio[16];                //sex ratios by age groups
    double fertlty[sim_nd-sim_bg+1][6]; //fertility by age groups
    
    int pop_loss[sim_nd-sim_bg+1];      //net annual population loss due to migration
    
    cblok(int cid, string cname, double lat, double log);
    void read_demgrphcs();
    void read_parmtrs();
    void reset_cpop();
    void bld_mbloks();
    void bld_cblok_pop();
    void add_agent(agent *p);
    void rmv_agent(agent *p);
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
    void calc_smoothed_pop_agrp(int *p, int *res);
    void calc_marital_prob();
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
