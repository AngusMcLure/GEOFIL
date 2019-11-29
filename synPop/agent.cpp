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
    
    worms = 0;
    epids = 's';
    clock_inf = -1; // this seems to get changed and set a lot, but never actually used for anything of consequence
    mda_f = 0;
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
void agent::calc_risk(double prv, char time, double c){
    double p;
    if(time == 'd') p = rb_working * c * prv * r_w;      //bites * positive * probability receiving mated worm
    else p = rb_offwork * c * prv * r_w;
    if(drand48() < p) ++worms;
}

// update epidemic status based on new bites (from calc_risk) and life-stage progression of exiting worms
void agent::renew_epidemics(){
    
    // note that in Sting's code the new worm is added at this time only if it will mature into a Mf-producing worm later. Therefore a large proportion are discarded at this point, and the number with prepatent worms in the model will not include those who have prepatent worms that will never make it to Mf-producing state and so will be an under-estimate. It also assumes that even if the person recieved more than one pair of mated worms (that day) only one of them survives with probability equal to to probability that at least one of them survives. To amend this we used this code rather than Sting's original (below)
    for(int w = 0; w < worms ; ++w){
        int clock_pre = min_pre_period + int(drand48()*(max_pre_period-min_pre_period)); // prepatancy duration
        int clock_live_pre = int(log(drand48())*survive_dur/log(survive_prop)); // duration of life assuming prepatent
        if(clock_live_pre < clock_pre){ // if the worm will die before adulthood
            //do nothing -- uncomment below if we're interested in all prepatent worms
            //wvec.push_back(new worm('p', clock_live_pre, 0)); // make a new prepatent worm with prepatent lifetime 'clock_live_pre' and no infectious lifetime
        } else { // if the worm survives to adulthood
            int active = min_inf_period + int(drand48()*(max_inf_period-min_inf_period)); //the active period (infectious lifetime) of the worm to be a random uniform between min and max duration
            wvec.push_back(new worm('p', clock_pre, active)); // make a new prepatent worm with prepatent lifetime 'clock_pre' and infectious lifetime 'active'
        }
    }
    
    
// Sting's version of above
//    int clock_pre = min_pre_period + int(drand48()*(max_pre_period-min_pre_period)); // prepatancy duration uniform random between min and max duration
//    double s_w = pow(survive_prop, clock_pre/(double)survive_dur);
//    if(drand48() < 1-pow(1-s_w, worms)){
//        int active = min_inf_period + int(drand48()*(max_inf_period-min_inf_period)); //the active period (infectious lifetime) of the worm to be a random uniform between min and max duration
//        wvec.push_back(new worm('p', clock_pre, active)); // make a new prepatent worm with prepatent lifetime 'clock_pre' and infectious lifetime 'active'
//    }
    
    char ch = 's';
    //for individuals with worms, set their status to infected if at least one of the worms is mf, and set to exposed otherwise
    if(wvec.size() > 0){
        ch = 'e';
        for(int i = 0; i < wvec.size(); ++i){
            if(wvec[i]->status == 'm'){
                ch = 'i'; break;
            }
        }
    }
    
    if(epids == 's') epids = ch;
    else if(epids == 'e'){
        epids = ch;
        if(epids == 'i') clock_inf = 14;
    }
    else if(epids == 'i'){
        epids = ch;
        if(clock_inf > 0) --clock_inf;
    }
    
    worms = 0;
}
void worm::update(){
    if(status == 'p'){ // progress worm's prepatent count-down timer and move onto 'mf producing' if time is up
        //cout << status << ' ' << clock_pp << endl;
        if(clock_pp == 0) status = 'm';
        else --clock_pp;
    }
    // progress worm's 'mf-producing' count-down timer and move onto dead/'to be removed' if time is up
    if(status == 'm'){
        //cout << status << ' ' << clock_mf << endl;
        if(clock_mf == 0) status = 'd';
        else --clock_mf;
        
    }
    
    if(clock_mda > 0) --clock_mda;
    else mda_f = 0;
    
}

// update individual's state wrt worms
void agent::update(){
    //update all worms in body
    if(wvec.size() > 0){ //if the person has worms update each worm
        for(int i = 0; i < wvec.size();){
            wvec[i]->update();
            
            // deal with any dead worms
            if(wvec[i]->status == 'd'){ // if the worm is (now) dead remove them from the list
                delete wvec[i];
                wvec.erase(wvec.begin() + i);
            }
            else ++i; // increment is placed here because if a worm is removed from the list the next worm will now have the index of deleted worm
        }
    }
    
    //update individual's epi state
    if(wvec.size() == 0){ // if they don't have worms their status is susceptible
        epids = 's';
        mda_f = 0;
    }
    else{
        if(wvec.size() > 0) epids = 'e'; // if they have worms set them to exposed/prepatent (this will be over-ruled later in the case that they also have mf-producing worms)
        
        // if the person has a mated pair of mf-producing worms, set human status to infectious and set the person's mda_f to the minimum of their worms' mda_f
        // i.e. the infectiousness of the human is determined by the most infectious worm (smallest mda_f)
        for(int i = 0; i < wvec.size(); ++i){
            if(wvec[i]->status == 'm'){
                if(epids != 'i'){
                    epids = 'i';
                    mda_f = wvec[i]->mda_f;
                }
                else{
                    if(mda_f > wvec[i]->mda_f) mda_f = wvec[i]->mda_f;
                }
            }
            
            if(epids == 'i' && mda_f == 0) break;
        }
    }
}

// for MDA, drug efficiency: r1 indicates proportion of worms killed;
// r2 indicates proportion of worms sterlized;
// l indicates the effective length of the drug
void agent::get_drugs(drugs drug){
    if(wvec.size() > 0){ // if the person has worms
        double rr = drand48();
        
        for(int i = 0; i < wvec.size(); ++i){ // for every worm
            wvec[i]->clock_mda = int(drug.SterDur * 365); // set count-down timer for the duration of mda effects (only relavant for sterility which is not permanent)
            
            if(rr <= drug.KillProb) wvec[i]->status = 'd'; // kill worm with probability r1
            else if(rr <= drug.KillProb + drug.FullSterProb) wvec[i]->mda_f = 1.0; // sterilise worm with probability r2
            else if(rr <= drug.KillProb + drug.FullSterProb + drug.PartSterProb) wvec[i]->mda_f = drug.PartSterMagnitude;
        }
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
