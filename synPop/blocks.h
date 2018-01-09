//
//  blocks.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef blocks_hpp
#define blocks_hpp

#include "agrps.h"
#include "sites.h"
using namespace std;

//family units
struct unit{
    agent *father;
    agent *mother;
    vector<agent*> child;
    
    unit(agent *p = NULL, agent *q = NULL){ father = p;  mother = q;  child.clear();}
};

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
    void rnd_margs(agent *p);
    void bld_hhold(int n);
    void bld_pop(int mm, int ff, agrps *pp);
    void bld_family_unit(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                         vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec,
                         vector<agent*> &m_chld, vector<agent*> &f_chld, vector<unit*> &famly);
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
    
    bool init;                         //for creating pop
    vector<agent*> mvec[vg_agrps];
    vector<agent*> fvec[vg_agrps];
    
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
    map<int, int> mblok_mpops;          //male pop in each mblok
    map<int, int> mblok_fpops;          //female pop in each mblok
    map<int, int> mblok_hholds;         //hholds in each mblok
    map<int, agrps*> mblok_agrps;       //mblok pop by age group;
    
    int male_by_agrp[age_grps];         //males by age groups
    int fmal_by_agrp[age_grps];         //females by age groups
    int male_by_age[max_ages+1];        //males by singular age - triangle smoothed
    int fmal_by_age[max_ages+1];        //females by singlular age - triangle smoothed
    
    double male_married[marital_agrps];
    double male_widowed[marital_agrps];
    double male_divorce[marital_agrps];
    double fmal_married[marital_agrps];
    double fmal_widowed[marital_agrps];
    double fmal_divorce[marital_agrps];
    double male_single[marital_agrps];
    double fmal_single[marital_agrps];
    double male_marital_prob[marital_ages];    //male marriage probability by age
    double fmal_marital_prob[marital_ages];    //female marriage probability by age
    
    double children_by_agrps[10];       //children ever born by age groups
    double children_by_age[50];         //smoothed children ever born
    
    double mmortlty[18];                //male mortatlity
    double fmortlty[18];                //female mortatlity
    
    double sex_ratio[age_grps];               //sex ratios by age groups
    double fertlty[sim_nd-sim_bg+1][6]; //fertility by age groups
    
    int pop_loss[sim_nd-sim_bg+1];      //net annual population loss due to migration
    
    cblok(int cid, string cname, double lat, double log);
    bool pop_reload();
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
    void calc_smoothed_pop_agrp(int *p, int pL, int *res, int rL);
    void calc_smoothed_chd_agrp(double *p, int pL, double *res, int rL);
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
