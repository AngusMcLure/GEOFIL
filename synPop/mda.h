//
//  MDA.h
//  synPop
//
//  Created by Angus on 9/8/19.
//  Copyright © 2019 Angus McLure. All rights reserved.
//

#ifndef mda_h
#include "paras.h"
#define mda_h
#include <vector>
#include <algorithm>

using namespace std;

class mda_strat;
class drugs;
int count_mda_scenarios(string file);
mda_strat get_nth_mda_strat(string filename, int N);

class drugs{
public:
    double KillProb;
    double FullSterProb;
    double PartSterProb;
    double SterDur;
    double PartSterMagnitude;
    
    
    drugs(double K=0.0, double FSP=0.0, double PSP=0.0, double SD=0.0, double PSM=0.0){
        KillProb = K;
        FullSterProb = FSP;
        PartSterProb = PSP;
        SterDur = SD;
        PartSterMagnitude = PSM;
    }
    
    
    void print_drugs(){
        cout << "KillProb: " << KillProb << endl;
        cout << "FullSterProb: " << FullSterProb << endl;
        cout << "PartSterProb: " << PartSterProb << endl;
        cout << "SterDur: " << SterDur << endl;
        cout << "PartSterMagnitude: " << PartSterMagnitude << endl;
    }
    
};

class mda_strat{
public:
    double Coverage;
    drugs drug1;
    int MinAge1;
    drugs drug2;
    int MinAge2;
    int StartYear;
    int NumRounds;
    int YearsBetweenRounds;
    vector<int> MDAYears;
    vector<int> MDAYearstarg;
    char Ad_MDA;
    char Ad_Scheme;
    int Ad_Start;
    int Ad_Rounds;
    int Ad_Years;
    int Ad_N_Buildings;
    double Ad_C_School;
    double Ad_C_Workplace;
    int NumSims;
    double ProbOneSex;
    double ProbBothSex;
    char InitType; // C = custom, A = Scenario A
    double InitPrev; //The initialisation prevalence of Mf-positive people. Only used if InitType == 'C'
    int SimYears; //Number of Simulation Years
    
    mda_strat(double C,
              drugs D1, int MA1,
              drugs D2, int MA2,
              int S, int N, int Y,
              char ADM, char ADS,
              int AD_S, int AD_N, int AD_Y,
              int ADNB, double ADCS, double ADCW,
              int NS, double POS,
              double PBS,char IT,
              double IP, int SY){
        Coverage = C;
        drug1 = D1;
        MinAge1 = MA1;
        drug2 = D2;
        MinAge2 = MA2;
        StartYear = S;
        NumRounds = N;
        YearsBetweenRounds = Y;
        MDAYears.resize(NumRounds);
        for(int count = 0; count<NumRounds; ++count){
            MDAYears[count] = StartYear + count * YearsBetweenRounds;
            cout << MDAYears[count] << " is a MDA year" << endl;
        }
        Ad_MDA = ADM;
        Ad_Scheme = ADS;// What scheme to run
        Ad_Start = AD_S;//Start year of additional
        Ad_Rounds = AD_N; //Number of additional
        Ad_Years = AD_Y; //years between ad rounds
        MDAYearstarg.resize(Ad_Rounds);
        for (int i = 0; i<Ad_Rounds; ++i){
            MDAYearstarg[i] = Ad_Start + i * Ad_Years;
            cout << MDAYearstarg[i] << " is an targeted MDA year" << endl;
        }
        Ad_N_Buildings = ADNB;
        Ad_C_School = ADCS; //School coverage
        Ad_C_Workplace = ADCW; //Workplace Coverage
        NumSims = NS;
        ProbOneSex = POS;
        ProbBothSex = PBS;
        InitType = IT;
        InitPrev = IP;
        SimYears = SY;
    }
    
   
    void print_mda_strat(){
        cout << endl;
        cout << "Coverage: " << Coverage * 100 << "%" << endl;
        drug1.print_drugs();
        cout << "Min Age 1: " << MinAge1 << endl;
        drug2.print_drugs();
        cout << "Min Age 2: " << MinAge2 << endl;
        cout << "MDA Start Year: " << StartYear << endl;
        cout << "Number of Rounds: " << NumRounds << endl;
        cout << "Years Between Rounds: " << YearsBetweenRounds << endl;
        cout << "Number of Simulations: " << NumSims << endl;
        cout << "Prob One Sex: " << ProbOneSex << endl;
        cout << "Prob Both Sex: " << ProbBothSex << endl;
        cout << "Init Type: " << InitType << endl;
        cout << "Init Prevalence: " << InitPrev * 100 << "%" << endl;
        cout << "Final Sim Year: " << SimYears + sim_bg - 1 << endl;
        cout << endl;
    }
    
    bool is_mda_year(int Year){ // Returns true if given year is an MDA year for the strategy
        return find(MDAYears.begin(), MDAYears.end(), Year) != MDAYears.end();
    }

    bool is_additonal_mda_year(int Year){
        return find(MDAYearstarg.begin(), MDAYearstarg.end(), Year) != MDAYearstarg.end();
    }
};




#endif /* mda_h */
