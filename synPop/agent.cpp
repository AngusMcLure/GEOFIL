//
//  agent.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "agent.h"
#include "hhold.h"
#include "block.h"
#include <limits>

//extern int rb_working, rb_offwork;

// Constructor of agent, representing human individuals
// with id, age, gender, marital status, household, workplace, school
agent::agent(int aid, int age, char gendr, char margs, hhold *h_d, workp *w_p, schol *s_h){
    this->aid = aid;
    this->age = age;
    this->gendr = gendr;
    this->margs = margs;
   
    this->h_d = h_d;
    this->w_p = w_p;
    this->s_h = s_h;
    
    spw = NULL;
    mom = NULL;
    dad = NULL;
    
    births = 0;
    bth_wind = 0;
    
    LastDayWithAdultWorm = std::numeric_limits<int>::min();

    
    InfectiveBites = 0;
    epids = 's';
    //clock_inf = -1; // this seems to get changed and set a lot, but never actually used for anything of consequence
    mda_f = 1.0;
    ChangedEpiToday = false;
}

// Destructor of Agent
agent::~agent(){
    spw = NULL;
    mom = NULL;
    dad = NULL;
    h_d = NULL;
    w_p = NULL;
    s_h = NULL;
    
    chdr.clear();
    for(int i = 0; i < wvec.size(); ++i){
        delete wvec[i];
    }
    wvec.clear();
}

// add a child
void agent::add_child(agent *p){
    chdr.insert(pair<int, agent*>(p->aid, p));
}

// calculate force of infection, considering mosquito exposure,
// prevalence of infective mosquitoes, and probability of receiving mated worms
void agent::sim_bites(double prv, char time, double c){
    
    double pos_inf_bites_rate = c * prv; // mean number of possibly infected bites per period = bites per period * prevalence in mosquitoes
    if(time == 'd') pos_inf_bites_rate *= rb_working; //biting rate day vs night
    else pos_inf_bites_rate *= rb_offwork;
    InfectiveBites += poisson(pos_inf_bites_rate * (p_both_sex + p_one_sex)); //actual random number of bites from infected mosquitoes
    
    
    for(int b = 0; b < InfectiveBites ; ++b){
        int clock_pre = min_pre_period + int(drand48()*(max_pre_period-min_pre_period)); //
        int active = min_inf_period + int(drand48()*(max_inf_period-min_inf_period)); //the active period (infectious lifetime) of the worm to be a random uniform between min and max duration
        if(drand48() < p_both_sex/(p_both_sex + p_one_sex)){
            // make two new prepatent worms, one of each gender, with prepatent lifetimes 'clock_pre' and infectious lifetimes 'active'
            wvec.push_back(new worm('p', clock_pre, active, 'm'));
            wvec.push_back(new worm('p', clock_pre, active, 'f'));
        }else{
            char gender = 'f';
            if(drand48() < prob_worm_male){
                gender = 'm';
                wvec.push_back(new worm('p', clock_pre, active, gender)); // make a new prepatent worm with prepatent lifetime 'clock_pre' and infectious lifetime 'active' and gender 'gender'
            }
        }
        
    }
    //If they got at least one infective bite, and they weren't already infected update their epi-status accordingly

    if(InfectiveBites > 0 && epids == 's'){
        epids = 'e';
    }
    InfectiveBites = 0;
    
}

void worm::update(){
    if(status == 'p'){ // progress worm's prepatent count-down timer and move onto mature if time is up
        if(clock_pp == 0) status = 'm';
        else --clock_pp;
    }
    // progress worm's 'mature' count-down timer and move onto dead/'to be removed' if time is up
    if(status == 'm'){
        if(clock_mf == 0) status = 'd';
        else --clock_mf;
        
    }
    
    if(clock_mda > 0) --clock_mda;
    else mda_f = 1.0;
    
}

// update individual's state wrt worms
void agent::update(int year, int day){
    //update all worms in body
    if(wvec.size() > 0){ //if the person has worms update each worm
        for(int i = 0; i < wvec.size();){
            wvec[i]->update();
            
            // deal with any dead worms
            if(wvec[i]->status == 'd'){ // if the worm is (now) dead remove them from the list
                delete wvec[i];
                wvec.erase(wvec.begin() + i);
            }
            else ++i; // iterator increment is placed here because if a worm is removed from the list the next worm will now have the index of deleted worm
        }
    }
    
    char PrevEpiStatus = epids;
    
    bool HasFertileMale = false;
    bool HasAdult = false;
    double max_mda_female = 0.0;
    
    //update individual's epi state
    if(wvec.size() == 0){ // if they don't have worms their status is susceptible
        epids = 's';
        mda_f = 1.0;
    }
    else{
        // if the person has a mated pair of mature fertile worms, set human status to infectious and set the person's mda_f to the maximum of their female worms' mda_f
        // i.e. the infectiousness of the human is determined by the most fertile mature female worm (largest mda_f)
        // the mda_f for people who aren't infectious doesn't matter and will not reflect their potential fertility of their worms
        for(int i = 0; i < wvec.size(); ++i){
            if(wvec[i]->status == 'm'){
                HasAdult = true;
                if(wvec[i]->gender == 'm'){
                    if(wvec[i]->mda_f > 0.0) HasFertileMale = true;
                }
                else{
                    max_mda_female = max(wvec[i]->mda_f, max_mda_female);
                }
            }
            if(max_mda_female == 1.0 && HasFertileMale) break;
        }
        bool HasFertileFemale =  max_mda_female > 0.0;
        if(HasFertileMale && HasFertileFemale) epids = 'i'; //if has both genders of mature and fertile worms
        if(HasAdult & !(HasFertileMale && HasFertileFemale)) epids = 'u'; //has adult worm(s) but not a fertile male and a fertile female
        if(!HasAdult) epids = 'e'; // if has no mature worms -- i.e. only prepatent
        mda_f = max_mda_female;
        //if(epids == 'i' && mda_f > 0) cout << mda_f << endl;
    }
    
    //If their last adult worm has just died record the day this happened
    if((PrevEpiStatus == 'u' || PrevEpiStatus == 'i') && (epids == 's' || epids == 'e')){
        LastDayWithAdultWorm = year * 365 + day;
    }
    
}

void agent::get_drugs(drugs drug){
    if(wvec.size() > 0){ // if the person has worms
        double rr = drand48();
        
        if(rr <= drug.KillProb){ // kill worms with probability KillProb
            for(int i = 0; i < wvec.size(); ++i){
                wvec[i]->status = 'd';
            }
        }
        else if(rr <= drug.KillProb + drug.FullSterProb){ // sterilise worms with probability FullSterProb
            for(int i = 0; i < wvec.size(); ++i){
                wvec[i]->mda_f = 0.0;
            }
        }
        else if(rr <= drug.KillProb + drug.FullSterProb + drug.PartSterProb){//Partially sterilise (reduce fertility to 1- PartSterMagnitude)  with probability PartSterProb
            for(int i = 0; i < wvec.size(); ++i){
                wvec[i]->mda_f = min(wvec[i]->mda_f, 1-drug.PartSterMagnitude);
            }
        }
        
/* Version where each worm has their own independent probability of being killed or sterilised -- here females only, in older versions which only modelled pairs of worms this happenned at the level of pairs of worms
        for(int i = 0; i < wvec.size(); ++i){
            if(wvec[i]->gender == 'f'){ // for every female worm
                wvec[i]->clock_mda = int(drug.SterDur * 365); // set count-down timer for the duration of mda effects (only relavant for sterility which is not permanent)
                
                if(rr <= drug.KillProb) wvec[i]->status = 'd'; // kill worm with probability KillProb
                else if(rr <= drug.KillProb + drug.FullSterProb) wvec[i]->mda_f = 0.0; // sterilise worm with probability FullSterProb
                else if(rr <= drug.KillProb + drug.FullSterProb + drug.PartSterProb) wvec[i]->mda_f = min(wvec[i]->mda_f, 1-drug.PartSterMagnitude); //Partially sterilise worm with probability PartSterProb
            }
        }
*/
    }
}

// Write linelist for person to file
void agent::write_line_list(int SimNum, int y, int s){
    // location where line list data by year will be outputted
    string line_list = outdir;    line_list = line_list + "line_list.csv";
    
    ofstream out;   ifstream in;
    in.open(line_list.c_str()); // try opening the target for output
    if(!in){ // if it doesn't exist write a heading
        out.open(line_list.c_str());
        out << "SimulationNumber,";
        out << "Seed,";
        out << "Year,";
        out << "EpiState,";
        out << "ID,";
        out << "Age,";
        out << "Gender,";
        out << "WormBurden,";
        out << "HouseholdID,";
        out << "HouseHoldSize,";
        out << "HomeBuildingID,";
        out << "HomeBuildingLatitude,";
        out << "HomeBuildingLongitude,";
        out << "HomeVillageID,";
        out << "WorkplaceID,";
        out << "WorkplaceWorkerNumber,";
        out << "WorkplaceVillageID,";
        out << "SchoolID,";
        out << "SchoolLevel,";
        out << "SchoolStudentNumber,";
        out << endl;
        out.close();
    }
    else in.close();
    
    //write the line list for all infected persons
    out.open(line_list.c_str(), ios::app);
    out << SimNum << ","; // Simulation Number
    out << s << ","; // seed used to generate this batch of sims
    out << y+sim_bg << ","; //year this observation occurred
    out << epids << ","; // their epi status
    out << aid << "," ; // person ID (two different ways)
    out << int(age/365) << ","; //age in years
    out << gendr << ","; //gender
    out << wvec.size() << ","; //worm number
    out << h_d->hid << ","; // household id
    out << h_d->size << ","; // number of people in household
    out << h_d->rdg->bid << ","; // House building id
    out << h_d->rdg->lat << ","; // House building latitude
    out << h_d->rdg->log << ","; // House building longitude
    out << h_d->rdg->mbk->mid << ","; // home village id of home
    
    //if they work give work details, NAs otherwise
    if(w_p){
        out << w_p->wid << ","; //workplace id
        out << w_p->workers.size() << ","; //number of workers at their workplace
        out << w_p->mbk->mid << ","; //village id of workplace
    }else{
        out << "NA,"; //workplace id
        out << "NA,"; //number of workers at their workplace
        out << "NA,"; //village id of workplace
    }
    
    //if they go to school give school details, NAs otherwise
    if(s_h){
        out << s_h->sid << ","; //school id
        out << s_h->level << ","; //school level (elementary/highschool/college)
        out << s_h->student.size() << ","; //total number of students at their school
    }else{
        out << "NA,"; //school id
        out << "NA,"; //school level (elementary/highschool/college)
        out << "NA,"; //total number of students at their school
    }
    out << endl;
    out.close();
}
