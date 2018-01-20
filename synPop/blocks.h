//
//  blocks.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef blocks_hpp
#define blocks_hpp

#include "agent.h"
#include "agrps.h"
using namespace std;

//census blocks
class mblok;   //meshblock, smallest census unit
class cblok;   //cities or other large census area

class rbldg;        //residential
class wbldg;        //work buildings
class sbldg;        //school bldgs

class hhold{
public:
    int hid;                            //hhold id
    char typ;                           // c-couple, f-female, m-male, a-alone, o-organizational
    int siz;
    
    double lat, log;
    double area;
    rbldg *rdg;
    
    agent *hldr;                        //hhold holder
    map<int, agent*> mmbrs;             //hhold members
    
    hhold(int hid, int siz = 0, char typ = '-');
    ~hhold();
    
    void asg_bldg(rbldg *rdg);
    void add_mmbr(agent *p);
    bool asg_hldr(agent *p);
    void rmv_mmbr(agent *p);
    void updt_hhold();
};

class rbldg{
public:
    int bid;
    double log, lat;
    double area;
    hhold *hd;
    mblok *mbk;
    cblok *cbk;
    
    rbldg(int bid, double log, double lat, double area, mblok *mbk, cblok *cbk);
    ~rbldg();
};

class workp{
public:
    int wid;
    int siz;
    map<int, agent*> emplys;             //employees members
    double lat, log;
    double area;
    
    map<int, wbldg*> wdg;                         //building list
    
    workp(int wid, int siz);
    ~workp();
    
    void add_bldg(wbldg *wdg);
    void add_mmbr(agent *p);
    void rmv_mmbr(agent *p);
    void update();
    void removd();
};

class wbldg{
public:
    int bid;
    double lat, log;
    double area;
    workp *wkp;
    mblok *mbk;
    cblok *cbk;
    
    wbldg(int bid, double lat, double log, double area, workp *wkp, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->wkp = wkp;
        this->mbk = mbk;
        this->cbk = cbk;
    }
    
    ~wbldg(){
        wkp = NULL;
        mbk = NULL;
        cbk = NULL;
    }
};

class schol{
public:
    int sid;
    int siz;
    double lat, log;
    double area;
    
    map<int, sbldg*> sdg;
    
    map<int, agent*> tchrs;
    map<int, agent*> stdts;
    
    void add_bldg(sbldg *sdg);
    void add_tchr(agent *p);
    void add_stdt(agent *p);
    void rmv_tchr(agent *p);
    void rmv_stdt(agent *p);
    void update();
    void removd();
};

class sbldg{
    int bid;
    double lat, log;
    double area;
    schol *sch;
    mblok *mbk;
    cblok *cbk;
    
    sbldg(int bid, double lat, double log, double area, schol *sch, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->sch = sch;
        this->mbk = mbk;
        this->cbk = cbk;
    }
    
    ~sbldg(){
        sch = NULL;
        mbk = NULL;
        cbk = NULL;
    }
};

//family units
struct unit{
	agent *father;
	agent *mother;
	vector<agent*> child;
	vector<int> avail_ages;

	unit(agent *p = NULL, agent *q = NULL){ father = p;  mother = q;  child.clear();  avail_ages.clear(); }
	~unit(){
		father = NULL;
		mother = NULL;
		child.clear();
		child.shrink_to_fit();
		avail_ages.clear();
		avail_ages.shrink_to_fit();
	}
	int u_size(){
		int n = int(child.size());
		if (father != NULL) ++n;
		if (mother != NULL) ++n;
		return n;
	}
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
    void add_rbldg(rbldg *p);
    
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
    int next_aid;
    map<int, agent*> cblok_pop;
    map<int, agent*> fmal_marrd;       //married females
    
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
    double **euclid_dist;               //euclidean distance between meshblocks
    double **road_dist;                 //road distance between meshblocks
    
    //parameters from file inputs
    map<int, int> mblok_mpops;          //male pop in each mblok
    map<int, int> mblok_fpops;          //female pop in each mblok
    //map<int, int> mblok_hholds;         //hholds in each mblok
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
    
    double child_number_by_agrps[10];       //children ever born by age groups
    double child_number_by_age[50];         //smoothed children ever born
    int live_birth_order_by_agrps[10][8];
    int live_birth_order_by_age[10][35];
    int live_birth_order_interval[10][2];
    
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
    void add_vcnt_rbldg(rbldg *p);
    void rmv_vcnt_rbldg(rbldg *p);
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
    void calc_smoothed_agrp(double *p, int pL, double *res, int rL);
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
