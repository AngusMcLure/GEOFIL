//
//  mda.cpp
//  ReadTest
//
//  Created by Angus on 12/8/19.
//  Copyright © 2019 Angus McLure. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "mda.h"
using namespace std;

mda_strat get_nth_mda_strat(string filename, int N)
{
    ifstream in;
    
    in.open(filename.c_str());
    if(!in){
        cout << "open " << filename << " failed" << endl;
        exit(1);
    }
    
    string line;
    //skip N lines
    for(int i = 0; i < N; ++i){
        getline(in, line);
    }
    
    getline(in,line);
    
    char *str = new char[line.size()+1];
    strcpy(str, line.c_str());
    char *p = NULL;
    
    p = strtok(str, ",");       double MDACoverage = atof(p);
    p = strtok(NULL, ",");      double MDAKillProb1 = atof(p);
    p = strtok(NULL, ",");      double MDAFullSterProb1 = atof(p);
    p = strtok(NULL, ",");      double MDAPartSterProb1 = atof(p);
    p = strtok(NULL, ",");      double MDASterDur1 = atof(p);
    p = strtok(NULL, ",");      double MDAPartSterMagnitude1 = atof(p);
    p = strtok(NULL, ",");      int MinAge1 = atoi(p);
    p = strtok(NULL, ",");      double MDAKillProb2 = atof(p);
    p = strtok(NULL, ",");      double MDAFullSterProb2 = atof(p);
    p = strtok(NULL, ",");      double MDAPartSterProb2 = atof(p);
    p = strtok(NULL, ",");      double MDASterDur2 = atof(p);
    p = strtok(NULL, ",");      double MDAPartSterMagnitude2 = atof(p);
    p = strtok(NULL, ",");      int MinAge2 = atoi(p);
    p = strtok(NULL, ",");      int MDAStartYear = atoi(p);
    p = strtok(NULL, ",");      int MDANumRound = atoi(p);
    p = strtok(NULL, ",");      int MDAYearsBetweenRound = atoi(p);
    p = strtok(NULL, ",");      int NumSims = atoi(p);
    delete []str;
    
    drugs drug1 {MDAKillProb1, MDAFullSterProb1, MDAPartSterProb1, MDASterDur1, MDAPartSterMagnitude1};
    drug1.print_drugs();
    drugs drug2 {MDAKillProb2, MDAFullSterProb2, MDAPartSterProb2, MDASterDur2, MDAPartSterMagnitude2};
    drug2.print_drugs();
    
    mda_strat strat {MDACoverage,
        drug1, MinAge1,
        drug2, MinAge2,
        MDAStartYear, MDANumRound, MDAYearsBetweenRound, NumSims};
    
    
    
    return strat;
}

int count_mda_scenarios(string file){
    ifstream in;
    string line;
    
    in.open(file.c_str());
    if(!in){
        cout << "open " << file << " failed" << endl;
        exit(1);
    }
    int MDAScenarioNum {-1}; //Count the number of lines (not including the first, which is just the column titles)
    while (getline(in, line)){
        ++MDAScenarioNum;
    }
    in.close();
    return MDAScenarioNum;
}

