//
//  blocks.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef blocks_hpp
#define blocks_hpp

#include "hhold.h"
#include "agrps.h"
#include "mda.h"
using namespace std;

//census blocks
class mblok;   //meshblock, smallest census unit, village here
class cblok;   //cities or other large census area, AS here

class mblok{
public:
    int mid;                            //mblock id
    double lat, log;                    //latitude & longitude

    int mda = 0; //unique id to keep track of village MDA
    int days_before_mda = 0;

    cblok *cbk;                         //city block
    
    double sum_mf;

    map<int, agent*> mblok_males;       // village male pop
    map<int, agent*> mblok_fmals;       // village female pop
    map<int, hhold*> mblok_hholds;      // village household
    
    map<int, rbldg*> mblok_ocpy_rbldgs; // occupied residential building
    map<int, rbldg*> mblok_vcnt_rbldgs; // unoccupied residential building
    //map<int, rbldg*> mblok_rbldgs;
    
    struct w_node{
        int wid;
        double p;
        
        w_node(int wid, double p): wid(wid), p(p){ }
    };
    
    double labors;                      // number of labor force
    map<int, workp*> mblok_workps;      // workplaces
    vector<w_node*> mblok_working;   
    
    struct c_node{
        int mid;
        double p;
        
        c_node(int mid, double p): mid(mid), p(p){ }
    };
    
    double jobs;
    map<int, double> mblok_commuting;
    vector<c_node*> mblok_comm;        //commuting
    
    void bld_mblok_pop();
    void add_hhold(hhold *p);
    void rmv_hhold(hhold *p);
    void add_member(agent *p);
    void rmv_member(agent *p);
    void rnd_margs(agent *p);
    void add_workp(workp *p);
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
    int cchild;
    int chold;
    int next_aid;
    
    int hhold_threshold;               //calculate with ztpoission

    map<int, agent*> fmal_marry;       //married females
    map<int, agent*> fmal_cbrs[11];    //female child-bearing (age, 15-49) child_num (0 - 10)
    
    //map<int, agent*> risk_indiv;     //individuals at risk
    map<int, agent*> pre_indiv;        //latent individuals
    map<int, agent*> inf_indiv;        //infectious individuals
    map<int, agent*> uninf_indiv;        //uninfectious individuals - these are people just a single gender of worm
    //map<int, agent*> rmv_indiv;        //removed individuals
    
    map<int, rbldg*> inf_rbldg_day;
    map<int, rbldg*> inf_rbldg_night;
    map<int, schol*> inf_schol;
    map<int, workp*> inf_workp;
    
    map<int, rbldg*> risk_rbldg;
    map<int, schol*> risk_schol;
    map<int, workp*> risk_workp;
 
    bool init;                         //for creating pop
    vector<agent*> mvec[vg_agrps];
    vector<agent*> fvec[vg_agrps];
    
    int next_hid;
    //map<int, hhold*> cblok_hholds;
    //map<int, rbldg*> cblok_rbldgs;
    map<int, rbldg*> cblok_vcnt_rbldgs; //vacant residential buildings
    
    map<int, workp*> cblok_workps;
    
    int next_sid;
    vector<schol*> cblok_schols;
    map<int, schol*> cblok_e_schols;
    map<int, schol*> cblok_h_schols;
    map<int, schol*> cblok_c_schols;
    
    double labor_force;                //total employment
    double LFPR_by_agrp[2][11];        //labor force participation rate
    double LFPR_by_age[2][55];
    
    int next_mid, meshblocks;
    map<int, mblok*> mbloks;
    map<string, int> mbloksIndexA;     //meshblocks in cblocks
    map<int, string> mbloksIndexB;      
    map<int, double*> mblok_crdnt;     //meshblock coordinates
    double *euclid_dst;                //euclidean distance between meshblocks
    double *road_dst;                  //road distance between meshblocks
    
    //parameters from file inputs
    map<int, int> mblok_mpops;          //male pop in each mblok
    map<int, int> mblok_fpops;          //female pop in each mblok
    map<int, agrps*> mblok_agrps;       //mblok pop by age group;

    map<unsigned ,vector<pair<unsigned,unsigned>>> house_distance;

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
    
    double mmortlty[18];                    //male mortatlity
    double fmortlty[18];                    //female mortatlity
    
    double sex_ratio[age_grps];             //sex ratios by age groups
    double fertlty[sim_nd-sim_bg+1][7];     //fertility by age groups
    
    double pop_loss[sim_nd-sim_bg+1];       //net annual population loss due to migration
    double exposure_by_age[16];                //exposure by age: 0-4, 0.25; 5-15, 0.75
    
    cblok(int cid, string cname, double lat, double log);
    bool pop_reload();
    void read_demgrphcs();
    void read_parmtrs();
    void reset_cpop();
    void bld_mbloks();
    void bld_cblok_pop();
    void bld_cblok_hhold();
    void hndl_land_data();
    void allct_rbldgs();
    void calc_marital_prob();
    void rmv_agent(agent *p);
    void add_vcnt_rbldg(rbldg *p);
    void rmv_vcnt_rbldg(rbldg *p);
    void re_location(agent *p, hhold* h_hold);
    void hndl_rbldg(string ff, int low, int upper, int min_dist);
    void calc_smoothed_pop_agrp(int *p, int pL, int *res, int rL);
    void calc_smoothed_agrp(double *p, int pL, double *res, int rL);
    
    void radt_model(char m);
    
    void sim_pop(int year, mda_strat strategy);
    void rnd_jobs(agent *p);
    void hndl_jobs(int year);
    void hndl_schol(int year);
    void hndl_marrg(int year);
    void hndl_divrc(int year);
    void hndl_migrt(int year);
    void hndl_hold_rupt(int year);
    void renew_pop(int year, int day);
    void hndl_birth(int year, int day);
    void validate_pop(int year, int day);
    void select_schol(agent *p, char level);
    void calc_risk(int year, int day, mda_strat strat);
    void risk_loc_day(int year, int day);
    void risk_loc_night(int year, int day);        //time = 'd' or 'n', day/night
    void update_epi_status(int year, int day);
    void seed_epidemics(double p, int age_dn, int age_up, string village = "all");
    void seed_clustered_epidemics();
    void seed_clustered_epidemics_household();
    
    void implement_MDA(int year, mda_strat strat);
    void selective_MDA(int year, mda_strat strat);
    void village_mda(int year, mda_strat strat, vector<unsigned> keys);
    void refined_household_mda(int year, multimap<unsigned, unsigned> mda_houses, mda_strat strat);
    void workplace_mda(int year, mda_strat strat, multimap<unsigned, unsigned> mda_workplaces);
    void continuous_mda(int year, int day, mda_strat strat, targeted_mda *data);
    double achieved_coverage[sim_years]; // the actual drug coverage achieved each year (for each year of the simulation). Will be zero for most years.
    double achieved_coverage_m[sim_years]; // the actual drug coverage achieved each year (for each year of the simulation). Will be zero for most years.
    double achieved_coverage_f[sim_years]; // the actual drug coverage achieved each year (for each year of the simulation). Will be zero for most years.
    int number_treated[sim_years];
    int number_tested[sim_years];

    void householdistances(int max_distance, int village_number); //used to choose and store "nearest neighbours"
    void mda_countdown();

    double adult_prv[40];
    double child_6_7_prv[40];
    double child_11_12_prv[40];
    double teen_15_16_prv[40];
    double all_prv[40];
    double fagalli[40];
    double iliili[40];
    double hhold_prv[40];
    
    void reset_prv();
    void get_works(int year);
    void get_hhold(int year);
    void get_students(int year);
    void get_hhold_size(int year);
    void get_cpop(int year);
    void get_sexratio(int year);
    void get_sexratiob(int year);
    void get_geographic(int year);
    void get_bbldgarea(int year);
    void get_epidemics(int year,mda_strat strategy);
    void get_mosquitoes(int year);
    void out_epidemics(int year, int day);
    void out_riskmap(int year);
    void out_vg_prv(int year);
};

#endif /* blocks_hpp */
