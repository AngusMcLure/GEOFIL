//
//  blockStat.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

extern int SimulationNumber;
extern unsigned seed;
extern __iom_t10<char> SimulationDateStr;
extern string prv_out_loc;

// reset pop prevalence
void cblok::reset_prv(){
    memset(adult_prv, 0, sizeof(double)*40);
    memset(child_6_7_prv, 0, sizeof(double)*40);
    memset(all_prv, 0, sizeof(double)*40);
    memset(fagalli, 0, sizeof(double)*40);
    memset(iliili, 0, sizeof(double)*40);
    memset(hhold_prv, 0, sizeof(double)*40);
}

// get number of workers
void cblok::get_works(int year){
    int t_1 = 0, t_2 = 0, t_3 = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        
        int worker = 0;
        for(map<int, workp*>::iterator k = mbk->mblok_workps.begin(); k != mbk->mblok_workps.end(); ++k)
            worker += k->second->workers.size();
        
        //cout << mbloksIndexB[mbk->mid] << " " << worker << " " << int(mbk->jobs) << endl;
        t_1 += worker;
        t_2 += int(mbk->jobs);
        t_3 += int(mbk->labors);
    }
    cout << "total employment: wokers = " << t_1 << " jobs = " << t_2 << " labors = " << t_3 <<  endl;
}

// get number of households
void cblok::get_hhold(int year){
    chold = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        chold += j->second->mblok_hholds.size();
    }
    cout << "hhold = " << chold << endl;
}

// get number of students
void cblok::get_students(int year){
    int t_1 = 0, t_2 = 0, t_3 = 0, t = 0; // t_1 Elementary, t_2 High School, t_3 College, t All
    for(int i = 0; i < cblok_schols.size(); ++i){ //for each school
        schol *sh = cblok_schols[i];
        
        t += sh->student.size(); // tally total school population
        
        switch(sh->level){
            case 'E': t_1 += sh->student.size(); break; // tally elementary school population
            case 'H': t_2 += sh->student.size(); break; // tally hihgschool school population
            case 'B': // if a school with both elementary and highschool students
                for(map<int, agent*>::iterator j = sh->student.begin(); j != sh->student.end(); ++j){ //for each student in the school
                    int age = int(j->second->age/365);
                    if(age <= 13) ++t_1; //add them to the appropriate count by age
                    else ++t_2;
                }
                break;
            case 'C': t_3 += sh->student.size(); break; // tally college school population
        }
    }
    cout << "total students " << year+sim_bg << ": " << t << " " << t_1 << " " << t_2 << " " << t_3 << endl;
}

// get average household size
void cblok::get_hhold_size(int year){
    map<int, double> vec;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, hhold*>::iterator k = mbk->mblok_hholds.begin(); k != mbk->mblok_hholds.end(); ++k){
            hhold *cur = k->second;
            if(vec.find(cur->size) != vec.end()) ++vec[cur->size];
            else vec.insert(pair<int, int>(cur->size, 1));
        }
    }
    
    double total = 0;
    for(map<int, double>::iterator j = vec.begin(); j != vec.end(); ++j)
        total += j->second;
    
    string file = outdir;   file = file + syn_hhold_stat;
    ofstream out(file.c_str(), ios::app);
    
    out << year << ",";
    for(map<int, double>::iterator j = vec.begin(); j != vec.end(); ++j){
        out << j->first << ",";
    }
    out << endl;
    
    out << year << ",";
    out << std::setprecision(4) << std::setiosflags(ios::fixed);
    for(map<int, double>::iterator j = vec.begin(); j != vec.end(); ++j){
        out << j->second/(double)total << ",";
    }
    out << endl;
    
    out.close();
    vec.clear();
}

// get pop by age group
void cblok::get_cpop(int year){
    string outdata = outdir;   outdata = outdata + syn_pop_stat;
    ofstream out;
    
    ifstream in;
    in.open(outdata.c_str());
    if(!in){
        out.open(outdata.c_str());
        out << "age group,0-4,5-9,10-14,15-19,20-24,25-34,35-44,45-54,55-59,60-64,65-74,75-79,80-84,85+" << endl;
        out.close();
    }
    else in.close();
    
    out.open(outdata.c_str(), std::ios::app);
    
    int cc[14];     memset(cc, 0, sizeof(int)*14);
    int index = -1;
    int max_age = 0;
    for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
        for(map<int, agent*>::iterator j = k->second->mblok_males.begin(); j != k->second->mblok_males.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 25*365) index = int(int(j->second->age/365)/5);
            else if(j->second->age >= 25*365 && j->second->age < 35*365) index = 5;
            else if(j->second->age >= 35*365 && j->second->age < 45*365) index = 6;
            else if(j->second->age >= 45*365 && j->second->age < 55*365) index = 7;
            else if(j->second->age >= 55*365 && j->second->age < 60*365) index = 8;
            else if(j->second->age >= 60*365 && j->second->age < 65*365) index = 9;
            else if(j->second->age >= 65*365 && j->second->age < 75*365) index = 10;
            else if(j->second->age >= 75*365 && j->second->age < 80*365) index = 11;
            else if(j->second->age >= 80*365 && j->second->age < 85*365) index = 12;
            else if(j->second->age >= 85*365) index = 13;
            
            cc[index]++;
        }
        
        for(map<int, agent*>::iterator j = k->second->mblok_fmals.begin(); j != k->second->mblok_fmals.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 25*365) index = int(int(j->second->age/365)/5);
            else if(j->second->age >= 25*365 && j->second->age < 35*365) index = 5;
            else if(j->second->age >= 35*365 && j->second->age < 45*365) index = 6;
            else if(j->second->age >= 45*365 && j->second->age < 55*365) index = 7;
            else if(j->second->age >= 55*365 && j->second->age < 60*365) index = 8;
            else if(j->second->age >= 60*365 && j->second->age < 65*365) index = 9;
            else if(j->second->age >= 65*365 && j->second->age < 75*365) index = 10;
            else if(j->second->age >= 75*365 && j->second->age < 80*365) index = 11;
            else if(j->second->age >= 80*365 && j->second->age < 85*365) index = 12;
            else if(j->second->age >= 85*365) index = 13;
            
            cc[index]++;
        }
    }
    out << year+sim_bg << ",";
    for(int j = 0; j < 14; ++j){
        out << cc[j]/(double)cpop << ",";
    }
    out << endl;
    out.close();
}

void cblok::get_sexratio(int year){
    
}

// get sex ratio by age group
void cblok::get_sexratiob(int year){
    ofstream out;
    ifstream in;
    
    string outdata = outdir;
    outdata = outdata + syn_sex_ratios_broad;
    in.open(outdata.c_str());
    if(!in){
        out.open(outdata.c_str());
        out << "age group,0-14,15-64,65+" << endl;
        out.close();
    }
    else in.close();
    
    double mm[3], ff[3];
    memset(mm, 0, sizeof(double)*3);
    memset(ff, 0, sizeof(double)*3);
    
    int index = -1;
    int max_age = 0;
    for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
        for(map<int, agent*>::iterator j = k->second->mblok_males.begin(); j != k->second->mblok_males.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 15*365) index = 0;
            else if(j->second->age >= 15*365 && j->second->age < 65*365) index = 1;
            else if(j->second->age >= 65*365) index = 2;
            
            mm[index]++;
        }
        
        for(map<int, agent*>::iterator j = k->second->mblok_fmals.begin(); j != k->second->mblok_fmals.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 15*365) index = 0;
            else if(j->second->age >= 15*365 && j->second->age < 65*365) index = 1;
            else if(j->second->age >= 65*365) index = 2;
            
            ff[index]++;
        }
    }
    
    outdata = outdir;
    outdata = outdata + syn_sex_ratios_broad;
    out.open(outdata.c_str(), std::ios::app);
    
    //cout << t << endl;
    out << year+sim_bg << ",";
    for(int j = 0; j < 3; ++j){
        out << (double)100*mm[j]/ff[j] << ",";
        //cout << mm[j] << " " << ff[j] << endl;
    }
    out << cpop << endl;
    out.close();
}

void cblok::get_geographic(int year){
    
}

void cblok::get_bbldgarea(int year){
    
}

//get LF prevalence by age group
void cblok::get_epidemics(int year, mda_strat strategy){
    
    //Count residents of these four localities
    int fagalii_mid = mbloksIndexA["Fagalii"];
    int futiga_mid = mbloksIndexA["Futiga"];
    int iliili_mid = mbloksIndexA["Iliili"];
    int vaitogi_mid = mbloksIndexA["Vaitogi"];
    
    //The residents of iliili are the sum of the residents of Iliili proper, Vaitogi and Futiga
    double n_fagalii = 0;
    double n_ilili = 0;
    n_fagalii += mbloks[fagalii_mid]->mblok_fmals.size() + mbloks[fagalii_mid]->mblok_males.size();
    n_ilili += mbloks[futiga_mid]->mblok_fmals.size() + mbloks[futiga_mid]->mblok_males.size();
    n_ilili += mbloks[iliili_mid]->mblok_fmals.size() + mbloks[iliili_mid]->mblok_males.size();
    n_ilili += mbloks[vaitogi_mid]->mblok_fmals.size() + mbloks[vaitogi_mid]->mblok_males.size();
    
    // initialise counts for number of adults, 6-7 yolds and residents of fagalii and iliili. Note that adult here includes all persons over 15.
    
    double n_6_7 = 0;
    double n_6_9 = 0;
    double n_11_12 = 0;
    double n_15_16 = 0;
    double n_8_and_over = 0;
    double n_8_13 = 0;
    double n_10_and_over = 0;
    double n_15_and_over = 0;
    double n_16_and_over = 0;
    double n_male = 0;
    double n_female = 0;
    
    double inf_6_7 = 0;
    double inf_6_9 = 0;
    double inf_11_12 = 0;
    double inf_15_16 = 0;
    double inf_8_and_over = 0;
    double inf_8_13 = 0;
    double inf_10_and_over = 0;
    double inf_15_and_over = 0;
    double inf_16_and_over = 0;
    double inf_male = 0;
    double inf_female = 0;
    double inf_iliili = 0;
    double inf_fagalii = 0;
    vector<double> inf_villages;
    inf_villages.resize(mbloks.size());
    
    double antigen_pos_6_7 = 0;
    double antigen_pos_6_9 = 0;
    double antigen_pos_11_12 = 0;
    double antigen_pos_15_16 = 0;
    double antigen_pos_8_and_over = 0;
    double antigen_pos_8_13 = 0;
    double antigen_pos_10_and_over = 0;
    double antigen_pos_15_and_over = 0;
    double antigen_pos_16_and_over = 0;
    double antigen_pos_male = 0;
    double antigen_pos_female = 0;
    double antigen_pos_iliili = 0;
    double antigen_pos_fagalii = 0;
    vector<double> antigen_pos_villages;
    antigen_pos_villages.resize(mbloks.size());


    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        int mid = mbk->mid;
        n_male += j->second->mblok_males.size();
        n_female += j->second->mblok_fmals.size();
        
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *a = k-> second;
            int age = int(a->age/365);

            // add to appropriate tally of total population by age
            if(age >= 6 && age <= 7)++n_6_7;
            if(age >= 6 && age <= 9)++n_6_9;
            if(age >= 11 && age <= 12)++n_11_12;
            if(age >= 15 && age <= 16)++n_15_16;
            if(age >= 8)++n_8_and_over;
            if(age >= 8 && age <= 13)++n_8_13;
            if(age >= 10)++n_10_and_over;
            if(age >= 15)++n_15_and_over;
            if(age >= 16)++n_16_and_over;
            
            if(a->epids == 'i'){
                ++inf_villages[j->first - 1];
                ++inf_male;
                if(mid == fagalii_mid) ++inf_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++inf_iliili;
                if(age >= 6 && age <= 7) ++inf_6_7;
                if(age >= 6 && age <= 9) ++inf_6_9;
                if(age >= 11 && age <= 12) ++inf_11_12;
                if(age >= 15 && age <= 16) ++inf_15_16;
                if(age >= 8) ++inf_8_and_over;
                if(age >= 8 && age <= 13) ++inf_8_13;
                if(age >= 10) ++inf_10_and_over;
                if(age >= 15) ++inf_15_and_over;
                if(age >= 16) ++inf_16_and_over;
            }
            if(a->epids == 'i' || a->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - a->LastDayWithAdultWorm) ){ //all people infected with any number of mature worms or who still have lingering antibodies are counted
                ++antigen_pos_villages[j->first - 1];
                ++antigen_pos_male;
                
                if(mid == fagalii_mid) ++antigen_pos_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++antigen_pos_iliili;
                if(age >= 6 && age <= 7) ++antigen_pos_6_7;
                if(age >= 6 && age <= 9) ++antigen_pos_6_9;
                if(age >= 11 && age <= 12) ++antigen_pos_11_12;
                if(age >= 15 && age <= 16) ++antigen_pos_15_16;
                if(age >= 8) ++antigen_pos_8_and_over;
                if(age >= 8 && age <= 13) ++antigen_pos_8_13;
                if(age >= 10) ++antigen_pos_10_and_over;
                if(age >= 15) ++antigen_pos_15_and_over;
                if(age >= 16) ++antigen_pos_16_and_over;
            }
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *a = k-> second;
            int age = int(a->age/365);
            
            // add to appropriate tally of total population by age
            if(age >= 6 && age <= 7) ++n_6_7;
            if(age >= 6 && age <= 9) ++n_6_9;
            if(age >= 11 && age <= 12) ++n_11_12;
            if(age >= 15 && age <= 16) ++n_15_16;
            if(age >= 8) ++n_8_and_over;
            if(age >= 8 && age <= 13) ++n_8_13;
            if(age >= 10) ++n_10_and_over;
            if(age >= 15) ++n_15_and_over;
            if(age >= 16) ++n_16_and_over;
            
            if(a->epids == 'i'){
                ++inf_villages[j->first - 1];
                ++inf_female;
                
                if(mid == fagalii_mid) ++inf_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++inf_iliili;
                if(age >= 6 && age <= 7) ++inf_6_7;
                if(age >= 6 && age <= 9) ++inf_6_9;
                if(age >= 11 && age <= 12) ++inf_11_12;
                if(age >= 15 && age <= 16) ++inf_15_16;
                if(age >= 8) ++inf_8_and_over;
                if(age >= 8 && age <= 13) ++inf_8_13;
                if(age >= 10) ++inf_10_and_over;
                if(age >= 15) ++inf_15_and_over;
                if(age >= 16) ++inf_16_and_over;
            }

            if(a->epids == 'i' || a->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - a->LastDayWithAdultWorm) ){ //all people infected with any number of mature worms or who still have lingering antibodies are counted
                ++antigen_pos_villages[j->first - 1];
                ++antigen_pos_female;
                
                if(mid == fagalii_mid) ++antigen_pos_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++antigen_pos_iliili;
                if(age >= 6 && age <= 7) ++antigen_pos_6_7;
                if(age >= 6 && age <= 9) ++antigen_pos_6_9;
                if(age >= 11 && age <= 12) ++antigen_pos_11_12;
                if(age >= 15 && age <= 16) ++antigen_pos_15_16;
                if(age >= 8) ++antigen_pos_8_and_over;
                if(age >= 8 && age <= 13) ++antigen_pos_8_13;
                if(age >= 10) ++antigen_pos_10_and_over;
                if(age >= 15) ++antigen_pos_15_and_over;
                if(age >= 16) ++antigen_pos_16_and_over;
            }
        }
    }
    /*
     //write each infected person's info to line list along with Sim number sim year and seed for batch of sims
     for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){ // for each infected individual
         j->second->write_line_list(SimulationNumber, year, seed);
     }
     //write each prepatent person's info to line list along with Sim number sim year and seed for batch of sims
     for(map<int, agent*>::iterator j = pre_indiv.begin(); j != pre_indiv.end(); ++j){ // for each infected individual
         j->second->write_line_list(SimulationNumber, year, seed);
     }
     */
     
    
    //Write some summaries to screen to monitor progress of simulations
    cout << endl;
    cout << year+sim_bg << ": " << "prepatent = " << pre_indiv.size() << " uninfectious = " << uninf_indiv.size() << " infectious = " << inf_indiv.size() << " antigen positive = " << antigen_pos_female + antigen_pos_male << endl;
    cout << ">=15 years' prevalence = " << fixed << setprecision(2) << inf_15_and_over/(double)n_15_and_over*100 << "%" << endl;
    cout << "6-7 years' prevalence = " << fixed << setprecision(2) << inf_6_7/(double)n_6_7*100 << "%" << endl;
    cout << "fagalii prevalence = " << fixed << setprecision(2) << inf_fagalii/(double)n_fagalii*100 << "%" << endl;
    cout << "iliili prevalence = " << fixed << setprecision(2) << inf_iliili/(double)n_ilili*100 << "%" << endl;
    cout << "overall mf prevalence = " << fixed << setprecision(2) << inf_indiv.size()/(double)cpop*100 << "%" << endl;
    
    
    // write prevalence for the year to file
    // location where prevalence by year will be outputted
    string prv_dat = outdir;    prv_dat = prv_dat + prv_out_loc;
    
    ofstream out;   ifstream in;
    in.open(prv_dat.c_str()); // try opening the target for output
    if(!in){ // if it doesn't exist write a heading
        out.open(prv_dat.c_str());
        out << "SimulationNumber,";
        out << "Seed,";
        out << "Year,";
        out << "MDACoverageAttempted,";
        out << "MDAKillProb1,";
        out << "MDAFullSterProb1,";
        out << "MDAPartSterProb1,";
        out << "MDASterDur1,";
        out << "MDAPartSterMagnitude1,";
        out << "MDAMinAge1,";
        out << "MDAKillProb2,";
        out << "MDAFullSterProb2,";
        out << "MDAPartSterProb2,";
        out << "MDASterDur2,";
        out << "MDAPartSterMagnitude2,";
        out << "MDAMinAge2,";
        out << "MDAStartYear,";
        out << "MDANumRounds,";
        out << "MDAYearsBetweenRounds,";
        out << "Prevalence6_7,";
        out << "Prevalence6_9,";
        out << "Prevalence8_13,";
        out << "Prevalence11_12,";
        out << "Prevalence15_16,";
        out << "Prevalence8AndOver,";
        out << "Prevalence10AndOver,";
        out << "Prevalence15AndOver,";
        out << "Prevalence16AndOver,";
        out << "PrevalenceAll,";
        out << "PrevalenceMale,";
        out << "PrevalenceFemale,";
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << "Prevalence" << mbloksIndexB[j -> second -> mid] << ","; //For each village prints "Prevalence<Village Name>,"
        }
        out << "Prev_Antigen6_7,";
        out << "Prev_Antigen6_9,";
        out << "Prev_Antigen8_13,";
        out << "Prev_Antigen11_12,";
        out << "Prev_Antigen15_16,";
        out << "Prev_Antigen8AndOver,";
        out << "Prev_Antigen10AndOver,";
        out << "Prev_Antigen15AndOver,";
        out << "Prev_Antigen16AndOver,";
        out << "Prev_AntigenAll,";
        out << "Prev_AntigenMale,";
        out << "Prev_AntigenFemale,";
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << "Prev_Antigen" << mbloksIndexB[j -> second -> mid] << ","; //For each village prints "Prev_Antigen<Village Name>,"
        }
        out << "SimulationStartDateTime,";
        out << "AchievedMDACoverage,";
        out << "AchievedMDACoverageMales,";
        out << "AchievedMDACoverageFemales,";
        out << "ProbOneGender,";
        out << "ProbBothGender";
        out << endl;
        out.close();
    }
    else in.close();
    
    
    //write the prevalence for whole populations, by gender, by age group and for each village
    out.open(prv_dat.c_str(), ios::app);
    
    out << SimulationNumber << ",";
    out << seed<< ",";
    out << year + sim_bg << ",";
    out << strategy.Coverage << ",";
    out << strategy.drug1.KillProb << ",";
    out << strategy.drug1.FullSterProb << ",";
    out << strategy.drug1.PartSterProb << ",";
    out << strategy.drug1.SterDur << ",";
    out << strategy.drug1.PartSterMagnitude << ",";
    out << strategy.MinAge1 << ",";
    out << strategy.drug2.KillProb << ",";
    out << strategy.drug2.FullSterProb << ",";
    out << strategy.drug2.PartSterProb << ",";
    out << strategy.drug2.SterDur << ",";
    out << strategy.drug2.PartSterMagnitude << ",";
    out << strategy.MinAge2 << ",";
    out << strategy.StartYear << ",";
    out << strategy.NumRounds << ",";
    out << strategy.YearsBetweenRounds << ",";
    out << inf_6_7/(double)n_6_7 << ",";
    out << inf_6_9/(double)n_6_9 << ",";
    out << inf_8_13/(double)n_8_13 << ",";
    out << inf_11_12/(double)n_11_12 << ",";
    out << inf_15_16/(double)n_15_16 << ",";
    out << inf_8_and_over/(double)n_8_and_over << ",";
    out << inf_10_and_over/(double)n_10_and_over << ",";
    out << inf_15_and_over/(double)n_15_and_over << ",";
    out << inf_16_and_over/(double)n_16_and_over << ",";
    out << inf_indiv.size()/(double)cpop << ",";
    out << inf_male/(double)n_male << ",";
    out << inf_female/(double)n_female << ",";
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        double n_village = (j -> second -> mblok_fmals).size() + (j -> second -> mblok_males).size();
        if(n_village==0) out << "NA,"; // there's a chance that populations in small villages might drop to zero - this is to avoid crashes in that situation
        else out <<  inf_villages[j -> first - 1]/(double)n_village << ",";
        /*
         cout << endl << mbloksIndexB[j -> second -> mid] << ":" << endl;
         cout << "Population: " << n_village << endl;
         cout << "Infected: " << inf_villages.at((j->second->mid)-1) << endl;
         */
    }
    out << antigen_pos_6_7/(double)n_6_7 << ",";
    out << antigen_pos_6_9/(double)n_6_9 << ",";
    out << antigen_pos_8_13/(double)n_8_13 << ",";
    out << antigen_pos_11_12/(double)n_11_12 << ",";
    out << antigen_pos_15_16/(double)n_15_16 << ",";
    out << antigen_pos_8_and_over/(double)n_8_and_over << ",";
    out << antigen_pos_10_and_over/(double)n_10_and_over << ",";
    out << antigen_pos_15_and_over/(double)n_15_and_over << ",";
    out << antigen_pos_16_and_over/(double)n_16_and_over << ",";
    out << (antigen_pos_male + antigen_pos_female)/(double)cpop << ",";
    out << antigen_pos_male/(double)n_male << ",";
    out << antigen_pos_female/(double)n_female << ",";
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        double n_village = (j -> second -> mblok_fmals).size() + (j -> second -> mblok_males).size();
        if(n_village==0) out << "NA,"; // there's a chance that populations in small villages might drop to zero - this it to avoid crashes in that situation
        else out <<  antigen_pos_villages[j -> first - 1]/(double)n_village << ",";
    }
    out << SimulationDateStr << ",";
    out << achieved_coverage[year] << ",";
    out << achieved_coverage_m[year] << ",";
    out << achieved_coverage_f[year] << ",";
    out << strategy.ProbOneSex << ",";
    out << strategy.ProbBothSex;
    out << endl;
    out.close();
}

// output epidemic map (household with infectious individuals) to draw map
void cblok::out_epidemics(int year, int day){
    string file = outdir;   file = file + "data_files/";
    file = file + to_string(year+sim_bg); file = file + "_";
    file = file + to_string(day);    file = file + "_epidemics.csv";
    ofstream out(file.c_str());
    
    map<int, hhold*> vec;
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        hhold *hd = j->second->h_d;
        vec.insert(pair<int, hhold*>(hd->hid, hd));
    }
    
    out << "year,hhold_id,hhold_lat,hhold_log" << endl;
    for(map<int, hhold*>::iterator j = vec.begin(); j != vec.end(); ++j){
        hhold *p = j->second;
        out << year << "," << p->hid << ",";
        out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
        out << p->lat << "," << p->log << endl;
    }
    out.close();
    
    vec.clear();
}

// output risk map
void cblok::out_riskmap(int year){
    string mapping = outdir;    mapping = mapping + "risk_mapping.csv";
    ifstream in;
    in.open(mapping.c_str());
    
    ofstream out;
    if(!in){
        out.open(mapping.c_str());
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << mbloksIndexB[j->first] << ",";
        }
        out << endl;
        
        out.close();
    }
    else in.close();
    
    double sum = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        sum += j->second->sum_mf;
    }
    
    out.open(mapping.c_str(), ios::app);
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        out << j->second->sum_mf/sum << ",";
    }
    out << endl;
    out << endl;
    out.close();
}

// get village prevalence
void cblok::out_vg_prv(int year){
    //output village prevalence
    map<int, double> vg_prv;    //output village prevalence
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j)
        vg_prv[j->first] = 0;
    
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        int mid = j->second->h_d->rdg->mbk->mid;
        vg_prv[mid]++;
    }
    
    string mapping = outdir;    mapping = mapping + "vg_prv.csv";
    ifstream in;
    in.open(mapping.c_str());
    
    ofstream out;
    if(!in){
        out.open(mapping.c_str());
        out << "village,";
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << mbloksIndexB[j->first] << ",";
        }
        out << endl;
        
        out.close();
    }
    else in.close();
    
    out.open(mapping.c_str(), ios::app);
    out << year+sim_bg << ",";
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        out << vg_prv[j->first]/(j->second->mblok_fmals.size() + j->second->mblok_males.size()) << ",";
    }
    out << endl;
    out << endl;
    out.close();
}

// get prevalence of infective mosquitoes
void cblok::get_mosquitoes(int year){
    int pools = 5;
    vector<rbldg*> rbldg_ids;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        if(j->second->mblok_ocpy_rbldgs.size() <= pools){
            for(map<int, rbldg*>::iterator k = j->second->mblok_ocpy_rbldgs.begin(); k != j->second->mblok_ocpy_rbldgs.end(); ++k)
                rbldg_ids.push_back(k->second);
        }
        else{
            for(int i = 0; i < pools; ++i){
                int index = rand() % j->second->mblok_ocpy_rbldgs.size();
                map<int, rbldg*>::iterator it = j->second->mblok_ocpy_rbldgs.begin();
                while(index-- > 0) ++it;
                
                rbldg_ids.push_back(it->second);
            }
        }
    }
    
    map<int, vector<double>> mp;
    for(int i = 0; i < rbldg_ids.size(); ++i){
        rbldg *rg = rbldg_ids[i];
        
        double prv_day = 0, prv_night = 0;
        
        rg->t_f = 1.0;
        double t_f2 = 1.0;
        
        //r_neigbors
        for(int i = 0; i < rg->r_neigh.size(); ++i){
            rbldg *r_2 = rg->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            rg->t_f += 1 - rg->r_neigh_d[i]/r_r;
            t_f2 += 1 - rg->r_neigh_d[i]/r_r;
        }
        
        //s_neigbors
        for(int i = 0; i < rg->s_neigh.size(); ++i){
            schol *sh = rg->s_neigh[i];
            if(sh->student.size() == 0) continue;
            rg->t_f += 1 - rg->s_neigh_d[i]/r_r;
        }
        
        //w_neigbors
        for(int i = 0; i < rg->w_neigh.size(); ++i){
            workp *wp = rg->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            rg->t_f += 1 - rg->w_neigh_d[i]/r_r;
        }
        
        double f = 1.0 / rg->t_f;
        prv_day += f * rg->day_p;
        
        f = 1.0 / t_f2;
        prv_night += f * rg->night_p;
        
        for(int i = 0; i < rg->r_neigh.size(); ++i){
            rbldg *r_2 = rg->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            
            f = (1 - rg->r_neigh_d[i]/r_r) / rg->t_f;
            prv_day += f * r_2->day_p;
            
            f = (1 - rg->r_neigh_d[i]/r_r) / t_f2;
            prv_night += f * r_2->night_p;
        }
        
        for(int i = 0; i < rg->s_neigh.size(); ++i){
            schol *sh = rg->s_neigh[i];
            if(sh->student.size() == 0) continue;
            f = (1 - rg->s_neigh_d[i]/r_r) / rg->t_f;
            prv_day += f * sh->day_p;
        }
        
        for(int i = 0; i < rg->w_neigh.size(); ++i){
            workp *wp = rg->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            f = (1 - rg->w_neigh_d[i]/r_r) / rg->t_f;
            prv_day += f * wp->day_p;
        }
        
        prv_day *= (r_i * s_l3);
        prv_night *= (r_i * s_l3);
        
        double pp = (prv_day + prv_night) / 2;
        mp[rg->mbk->mid].push_back(pp);
    }
    
    string data = outdir;
    data = data + syn_mosquitoes;
    
    ofstream out(data.c_str(), ios::app);
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        map<int, vector<double>>::iterator it = mp.find(j->first);
        
        if(it == mp.end()) out << " ,";
        else{
            double p = 0;
            for(int i = 0; i < it->second.size(); ++i){
                p += it->second[i];
            }
            
            out << p/it->second.size() << ",";
        }
    }
    out << endl;
    out.close();
}
