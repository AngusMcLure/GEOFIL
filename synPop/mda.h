//
//  MDA.h
//  synPop
//
//  Created by Angus on 9/8/19.
//  Copyright © 2019 Angus McLure. All rights reserved.
//

#ifndef mda_h
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
    int NumSims;

    
    mda_strat(double C,
              drugs D1, int MA1,
              drugs D2, int MA2,
              int S, int N, int Y, int NS){
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
        NumSims = NS;
    }
    
   
    void print_mda_strat(){
        cout << endl;
        cout << "Coverage: " << Coverage << endl;
        drug1.print_drugs();
        cout << "MinAge1: " << MinAge1 << endl;
        drug2.print_drugs();
        cout << "MinAge2: " << MinAge2 << endl;
        cout << "StartYear: " << StartYear << endl;
        cout << "NumRounds: " << NumRounds << endl;
        cout << "YearsBetweenRounds: " << YearsBetweenRounds << endl;
        cout << "NumSims: " << NumSims << endl;
        cout << endl;
    }
    
    bool is_mda_year(int Year){ // Returns true if given year is an MDA year for the strategy
        return find(MDAYears.begin(), MDAYears.end(), Year) != MDAYears.end();
    }
};





#endif /* mda_h */
