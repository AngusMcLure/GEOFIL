//
//  blocks.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef blocks_hpp
#define blocks_hpp

#include "social_cell.h"
#include "agrps.h"
using namespace std;

//census blocks
class mblok;   //meshblock, smallest census unit
class cblok;   //cities or other large census area

class mblok{
public:
    int mid;                            //mblock id
    double lat, log;                    //latitude & longitude
    cblok *cbk;                          //city block
    
    map<int, agent*> mblok_males;
    map<int, agent*> mblok_fmals;
    map<int, hhold*> mblok_hholds;
    
    map<int, rbldg*> mblok_ocpy_rbldgs;
    map<int, rbldg*> mblok_vcnt_rbldgs;
    //map<int, rbldg*> mblok_rbldgs;
    map<int, wbldg*> mblok_wbldgs;
    
    void bld_mblok_pop();
    void add_hhold(hhold *p);
    void rmv_hhold(hhold *p);
    void add_member(agent *p);
    void rmv_member(agent *p);
    void rnd_margs(agent *p);
    void add_rbldg(rbldg *p, hhold* h_hold = NULL);
    
    //for building population
    void bld_pop(int mm, int ff, agrps *pp);
    
    //for building household
    void bld_hhold();
    void bld_family_unit(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                         vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec,
                         vector<agent*> &chld, vector<unit*> &famly);
    void match_couple(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                      vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec,
                      vector<unit*> &famly);
    void allocate_child(vector<agent*> &chld, vector<unit*> &famly);
    int binary_search(vector<agent*> &vec, int age);

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
    int chold;
    int next_aid;
    
    int hhold_threshold;               //calculate with ztpoission

    map<int, agent*> fmal_marry;       //married females
    map<int, agent*> fmal_cbrs[11];    //female child-bearing (age, 15-49) child_num (0 - 10)
 
    bool init;                         //for creating pop
    vector<agent*> mvec[vg_agrps];
    vector<agent*> fvec[vg_agrps];
    
    int next_hid;
    //map<int, hhold*> cblok_hholds;
    //map<int, rbldg*> cblok_rbldgs;
    map<int, rbldg*> cblok_vcnt_rbldgs; //vacant residential buildings
    //map<int, wbldg*> cblok_wbldgs;
    
    int next_mid, meshblocks;
    map<int, mblok*> mbloks;
    map<string, int> mbloksIndexA;       //meshblocks in cblocks
    map<int, string> mbloksIndexB;      
    map<int, double*> mblok_crdnt;      //meshblock coordinates
    double *euclid_dst;               //euclidean distance between meshblocks
    double *road_dst;                 //road distance between meshblocks
    
    //parameters from file inputs
    map<int, int> mblok_mpops;          //male pop in each mblok
    map<int, int> mblok_fpops;          //female pop in each mblok
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
    
    double male_mrg_prob[marital_ages];    //male marriage probability by age
    double fmal_mrg_prob[marital_ages];    //female marriage probability by age
    
    double child_number_by_agrps[10];       //children ever born by age groups
    double child_number_by_age[50];         //smoothed children ever born
    int live_birth_order_by_agrps[10][8];
    int live_birth_order_by_age[10][35];
    int live_birth_order_interval[10][2];
    
    double live_birth_order_pro[10];        //birth order probability
    double live_birth_age_pro[10][7];       //birth age prob
    
    double mmortlty[18];                //male mortatlity
    double fmortlty[18];                //female mortatlity
    
    double sex_ratio[age_grps];               //sex ratios by age groups
    double fertlty[sim_nd-sim_bg+1][7]; //fertility by age groups
    
    double pop_loss[sim_nd-sim_bg+1];      //net annual population loss due to migration
    
    cblok(int cid, string cname, double lat, double log);
    bool pop_reload();
    void read_demgrphcs();
    void read_parmtrs();
    void rnd_mother();
    void reset_cpop();
    void bld_mbloks();
    void bld_cblok_pop();
    void bld_cblok_hhold();
    void hndl_land_data();
    void allct_rbldgs();
    void allct_bbldgs();
    void calc_bldg_dist();
    void calc_marital_prob();
    void rmv_agent(agent *p);
    void add_vcnt_rbldg(rbldg *p);
    void rmv_vcnt_rbldg(rbldg *p);
    void re_location(agent *p, hhold* h_hold);
    void hndl_rbldg(string ff, int low, int upper, int min_dist);
    void hndl_bbldg(string ff, int low, int upper, int min_dist);
    void calc_smoothed_pop_agrp(int *p, int pL, int *res, int rL);
    void calc_smoothed_agrp(double *p, int pL, double *res, int rL);
    
    void radt_model(char d);
    
    void sim_pop(int year);
    void hndl_marrg(int year);
    void hndl_divrc(int year);
    void hndl_migrt(int year);
    void hndl_hold_rupt(int year);
    void renew_pop(int year, int day);
    void hndl_birth(int year, int day);
    void validate_pop(int year, int day);
    
    void get_pop(int year);
    void get_hhold(int year);
    void get_hhold_size(int year);
    void get_sexratio(int year);
    void get_sexratiob(int year);
    void get_geographic(int year);
    void get_bbldgarea(int year);
    void prt_hhold(std::ofstream &out, hhold* hh);
};

#endif /* blocks_hpp */
