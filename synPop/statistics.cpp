//
//  blockStat.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"
#include <cstring>
extern int SimulationNumber;
extern unsigned seed;
//extern __iom_t10<char> SimulationDateStr;
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

    double mn_6_7 = 0;
    double mn_6_14 = 0;
    double mn_15_24 = 0;
    double mn_25_34 = 0;
    double mn_35_44 = 0;
    double mn_45_54 = 0;
    double mn_55_64 = 0;
    double mn_65_74 = 0;
    double mn_75_84 = 0;
    double mn_85_94 = 0;

    double fn_6_7 = 0;
    double fn_6_14 = 0;
    double fn_15_24 = 0;
    double fn_25_34 = 0;
    double fn_35_44 = 0;
    double fn_45_54 = 0;
    double fn_55_64 = 0;
    double fn_65_74 = 0;
    double fn_75_84 = 0;
    double fn_85_94 = 0;

    double n_male = 0;
    double n_female = 0;

    double minf_6_7 = 0;
    double minf_6_14 = 0;
    double minf_15_24 = 0;
    double minf_25_34 = 0;
    double minf_35_44 = 0;
    double minf_45_54 = 0;
    double minf_55_64 = 0;
    double minf_65_74 = 0;
    double minf_75_84 = 0;
    double minf_85_94 = 0;

    double finf_6_7 = 0;
    double finf_6_14 = 0;
    double finf_15_24 = 0;
    double finf_25_34 = 0;
    double finf_35_44 = 0;
    double finf_45_54 = 0;
    double finf_55_64 = 0;
    double finf_65_74 = 0;
    double finf_75_84 = 0;
    double finf_85_94 = 0;

    double inf_male = 0;
    double inf_female = 0;

    double inf_iliili = 0;
    double inf_fagalii = 0;
    vector<double> inf_villages;
    inf_villages.resize(mbloks.size());

    double mant_6_7 = 0;
    double mant_6_14 = 0;
    double mant_15_24 = 0;
    double mant_25_34 = 0;
    double mant_35_44 = 0;
    double mant_45_54 = 0;
    double mant_55_64 = 0;
    double mant_65_74 = 0;
    double mant_75_84 = 0;
    double mant_85_94 = 0;

    double fant_6_7 = 0;
    double fant_6_14 = 0;
    double fant_15_24 = 0;
    double fant_25_34 = 0;
    double fant_35_44 = 0;
    double fant_45_54 = 0;
    double fant_55_64 = 0;
    double fant_65_74 = 0;
    double fant_75_84 = 0;
    double fant_85_94 = 0;

    double ant_male = 0;
    double ant_female = 0;
    double ant_iliili = 0;
    double ant_fagalii = 0;
    vector<double> antigen_pos_villages;
    antigen_pos_villages.resize(mbloks.size());

    //Males
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        int mid = mbk->mid;
        n_male += j->second->mblok_males.size();
        n_female += j->second->mblok_fmals.size();
        
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *a = k-> second;
            int age = int(a->age/365);

            // add to appropriate tally of total population by age
            if(age >= 6 && age <= 7)++mn_6_7;
            if(age >= 6 && age <= 14)++mn_6_14;
            if(age >= 15 && age <= 24)++mn_15_24;
            if(age >= 25 && age <= 34)++mn_25_34;
            if(age >= 35 && age <= 44)++mn_35_44;
            if(age >= 45 && age <= 54)++mn_45_54;
            if(age >= 55 && age <= 64)++mn_55_64;
            if(age >= 65 && age <= 74)++mn_65_74;
            if(age >= 75 && age <= 84)++mn_75_84;
            if(age >= 85 && age <= 94)++mn_85_94;

            if(a->epids == 'i'){
                ++inf_villages[j->first - 1];
                ++inf_male;
                if(mid == fagalii_mid) ++inf_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++inf_iliili;

                if(age >= 6 && age <= 7)++minf_6_7;
                if(age >= 6 && age <= 14)++minf_6_14;
                if(age >= 15 && age <= 24)++minf_15_24;
                if(age >= 25 && age <= 34)++minf_25_34;
                if(age >= 35 && age <= 44)++minf_35_44;
                if(age >= 45 && age <= 54)++minf_45_54;
                if(age >= 55 && age <= 64)++minf_55_64;
                if(age >= 65 && age <= 74)++minf_65_74;
                if(age >= 75 && age <= 84)++minf_75_84;
                if(age >= 85 && age <= 94)++minf_85_94;
            }
            if(a->epids == 'i' || a->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - a->LastDayWithAdultWorm) ){ //all people infected with any number of mature worms or who still have lingering antibodies are counted
                ++antigen_pos_villages[j->first - 1];
                ++ant_male;
                
                if(mid == fagalii_mid) ++ant_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++ant_iliili;
                if(age >= 6 && age <= 7)++mant_6_7;
                if(age >= 6 && age <= 14)++mant_6_14;
                if(age >= 15 && age <= 24)++mant_15_24;
                if(age >= 25 && age <= 34)++mant_25_34;
                if(age >= 35 && age <= 44)++mant_35_44;
                if(age >= 45 && age <= 54)++mant_45_54;
                if(age >= 55 && age <= 64)++mant_55_64;
                if(age >= 65 && age <= 74)++mant_65_74;
                if(age >= 75 && age <= 84)++mant_75_84;
                if(age >= 85 && age <= 94)++mant_85_94;
            }
        }
        //Females
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *a = k-> second;
            int age = int(a->age/365);
            
            // add to appropriate tally of total population by age
            if(age >= 6 && age <= 7)++fn_6_7;
            if(age >= 6 && age <= 14)++fn_6_14;
            if(age >= 15 && age <= 24)++fn_15_24;
            if(age >= 25 && age <= 34)++fn_25_34;
            if(age >= 35 && age <= 44)++fn_35_44;
            if(age >= 45 && age <= 54)++fn_45_54;
            if(age >= 55 && age <= 64)++fn_55_64;
            if(age >= 65 && age <= 74)++fn_65_74;
            if(age >= 75 && age <= 84)++fn_75_84;
            if(age >= 85 && age <= 94)++fn_85_94;
            
            if(a->epids == 'i'){
                ++inf_villages[j->first - 1];
                ++inf_female;
                
                if(mid == fagalii_mid) ++inf_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++inf_iliili;
                if(age >= 6 && age <= 7)++finf_6_7;
                if(age >= 6 && age <= 14)++finf_6_14;
                if(age >= 15 && age <= 24)++finf_15_24;
                if(age >= 25 && age <= 34)++finf_25_34;
                if(age >= 35 && age <= 44)++finf_35_44;
                if(age >= 45 && age <= 54)++finf_45_54;
                if(age >= 55 && age <= 64)++finf_55_64;
                if(age >= 65 && age <= 74)++finf_65_74;
                if(age >= 75 && age <= 84)++finf_75_84;
                if(age >= 85 && age <= 94)++finf_85_94;
            }

            if(a->epids == 'i' || a->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - a->LastDayWithAdultWorm) ){ //all people infected with any number of mature worms or who still have lingering antibodies are counted
                ++antigen_pos_villages[j->first - 1];
                ++ant_female;

                if(mid == fagalii_mid) ++ant_fagalii;
                if(mid == futiga_mid || mid == iliili_mid || mid == vaitogi_mid) ++ant_iliili;
                if(age >= 6 && age <= 7)++fant_6_7;
                if(age >= 6 && age <= 14)++fant_6_14;
                if(age >= 15 && age <= 24)++fant_15_24;
                if(age >= 25 && age <= 34)++fant_25_34;
                if(age >= 35 && age <= 44)++fant_35_44;
                if(age >= 45 && age <= 54)++fant_45_54;
                if(age >= 55 && age <= 64)++fant_55_64;
                if(age >= 65 && age <= 74)++fant_65_74;
                if(age >= 75 && age <= 84)++fant_75_84;
                if(age >= 85 && age <= 94)++fant_85_94;
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
    cout << year+sim_bg << ": " << "prepatent = " << pre_indiv.size() << " uninfectious = " << uninf_indiv.size() << " infectious = " << inf_indiv.size() << " antigen positive = " << ant_female + ant_male << endl;
   //cout << ">=15 years' prevalence = " << fixed << setprecision(2) << inf_15_and_over/(double)n_15_and_over*100 << "%" << endl;
    //cout << "6-7 years' prevalence = " << fixed << setprecision(2) << inf_6_7/(double)n_6_7*100 << "%" << endl;
    //cout << "fagalii prevalence = " << fixed << setprecision(2) << inf_fagalii/(double)n_fagalii*100 << "%" << endl;
    //cout << "iliili prevalence = " << fixed << setprecision(2) << inf_iliili/(double)n_ilili*100 << "%" << endl;
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
        out << "Add_MDA";
        out << "Add_Scheme";
        out << "Add_Start";
        out << "Add_Rounds";
        out << "Add_Years_Between";
        out << "Add_N_Work_or_Vil";
        out << "Add_School_Coverage";
        out << "Add_Work_Coverage";
        out << "Male_6_7_pop,";
        out << "Male_6_14_pop,";
        out << "Male_15_24_pop,";
        out << "Male_25_34_pop,";
        out << "Male_35_44_pop,";
        out << "Male_45_54_pop,";
        out << "Male_55_64_pop,";
        out << "Male_65_74_pop,";
        out << "Male_75_84_pop,";
        out << "Male_85_94_pop,";
        out << "Female_6_7_pop,";
        out << "Female_6_14_pop,";
        out << "Female_15_24_pop,";
        out << "Female_25_34_pop,";
        out << "Female_35_44_pop,";
        out << "Female_45_54_pop,";
        out << "Female_55_64_pop,";
        out << "Female_65_74_pop,";
        out << "Female_75_84_pop,";
        out << "Female_85_94_pop,";
        out << "Male_6_7_prev,";
        out << "Male_6_14_prev,";
        out << "Male_15_24_prev,";
        out << "Male_25_34_prev,";
        out << "Male_35_44_prev,";
        out << "Male_45_54_prev,";
        out << "Male_55_64_prev,";
        out << "Male_65_74_prev,";
        out << "Male_75_84_prev,";
        out << "Male_85_94_prev,";
        out << "Female_6_7_prev,";
        out << "Female_6_14_prev,";
        out << "Female_15_24_prev,";
        out << "Female_25_34_prev,";
        out << "Female_35_44_prev,";
        out << "Female_45_54_prev,";
        out << "Female_55_64_prev,";
        out << "Female_65_74_prev,";
        out << "Female_75_84_prev,";
        out << "Female_85_94_prev,";
        out << "Male_6_7_ant,";
        out << "Male_6_14_ant,";
        out << "Male_15_24_ant,";
        out << "Male_25_34_ant,";
        out << "Male_35_44_ant,";
        out << "Male_45_54_ant,";
        out << "Male_55_64_ant,";
        out << "Male_65_74_ant,";
        out << "Male_75_84_ant,";
        out << "Male_85_94_ant,";
        out << "Female_6_7_ant,";
        out << "Female_6_14_ant,";
        out << "Female_15_24_ant,";
        out << "Female_25_34_ant,";
        out << "Female_35_44_ant,";
        out << "Female_45_54_ant,";
        out << "Female_55_64_ant,";
        out << "Female_65_74_ant,";
        out << "Female_75_84_ant,";
        out << "Female_85_94_ant,";
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << "Prevalence" << mbloksIndexB[j -> second -> mid] << ","; //For each village prints "Prevalence<Village Name>,"
        }
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << "Prev_Antigen" << mbloksIndexB[j -> second -> mid] << ","; //For each village prints "Prev_Antigen<Village Name>,"
        }
        out << "SimulationStartDateTime,";
        out << "AchievedMDACoverage,";
        out << "AchievedMDACoverageMales,";
        out << "AchievedMDACoverageFemales,";
        out << "ProbOneGender,";
        out << "ProbBothGender,";
        out << "InitType,";
        out << "InitPrev,";
        out << "SimYears";
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
    out << strategy.Ad_MDA << ",";
    out << strategy.Ad_Scheme << ",";
    out << strategy.Ad_Start << ",";
    out << strategy.Ad_Rounds << ",";
    out << strategy.Ad_Years << ",";
    out << strategy.Ad_N_Buildings << ",";
    out << strategy.Ad_C_School << ",";
    out << strategy.Ad_C_Workplace << ",";
    out << mn_6_7 << ",";
    out << mn_6_14 << ",";
    out << mn_15_24 << ",";
    out << mn_25_34 << ",";
    out << mn_35_44 << ",";
    out << mn_45_54 << ",";
    out << mn_55_64 << ",";
    out << mn_65_74 << ",";
    out << mn_75_84 << ",";
    out << mn_85_94 << ",";
    out << fn_6_7 << ",";
    out << fn_6_14 << ",";
    out << fn_15_24 << ",";
    out << fn_25_34 << ",";
    out << fn_35_44 << ",";
    out << fn_45_54 << ",";
    out << fn_55_64 << ",";
    out << fn_65_74 << ",";
    out << fn_75_84 << ",";
    out << fn_85_94 << ",";
    out << minf_6_7 << ",";
    out << minf_6_14 << ",";
    out << minf_15_24 << ",";
    out << minf_25_34 << ",";
    out << minf_35_44 << ",";
    out << minf_45_54 << ",";
    out << minf_55_64 << ",";
    out << minf_65_74 << ",";
    out << minf_75_84 << ",";
    out << minf_85_94 << ",";
    out << finf_6_7 << ",";
    out << finf_6_14 << ",";
    out << finf_15_24 << ",";
    out << finf_25_34 << ",";
    out << finf_35_44 << ",";
    out << finf_45_54 << ",";
    out << finf_55_64 << ",";
    out << finf_65_74 << ",";
    out << finf_75_84 << ",";
    out << finf_85_94 << ",";
    out << mant_6_7 << ",";
    out << mant_6_14 << ",";
    out << mant_15_24 << ",";
    out << mant_25_34 << ",";
    out << mant_35_44 << ",";
    out << mant_45_54 << ",";
    out << mant_55_64 << ",";
    out << mant_65_74 << ",";
    out << mant_75_84 << ",";
    out << mant_85_94 << ",";
    out << fant_6_7 << ",";
    out << fant_6_14 << ",";
    out << fant_15_24 << ",";
    out << fant_25_34 << ",";
    out << fant_35_44 << ",";
    out << fant_45_54 << ",";
    out << fant_55_64 << ",";
    out << fant_65_74 << ",";
    out << fant_75_84 << ",";
    out << fant_85_94 << ",";
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
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        double n_village = (j -> second -> mblok_fmals).size() + (j -> second -> mblok_males).size();
        if(n_village==0) out << "NA,"; // there's a chance that populations in small villages might drop to zero - this it to avoid crashes in that situation
        else out <<  antigen_pos_villages[j -> first - 1]/(double)n_village << ",";
    }
    //out << SimulationDateStr << ",";
    out << achieved_coverage[year] << ",";
    out << achieved_coverage_m[year] << ",";
    out << achieved_coverage_f[year] << ",";
    out << strategy.ProbOneSex << ",";
    out << strategy.ProbBothSex << ",";
    out << strategy.InitType << ",";
    out << strategy.InitPrev << ",";
    out << strategy.SimYears;
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
