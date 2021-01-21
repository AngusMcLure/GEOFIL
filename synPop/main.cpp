//
//  main.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <unistd.h>
#include "main.h"
#include "mda.h"

using namespace std;


// Simulation number and random seed are made global so that they can be accessed for writing data to files from across the program -- shouldn't be too difficult to rework to pass them through but it's a job...
int SimulationNumber;
unsigned seed;
auto t = time(nullptr);
auto tm = *localtime(&t);
__iom_t10<char> SimulationDateStr = put_time(&tm, "%Y/%m/%d %H:%M:%S");
string prv_out_loc;


int main(int argc, const char * argv[]) {
    
    //get location for outputting prevalence data
    cout << "Please enter a file name for the output file with annual prevalence reports:";
    getline(cin, prv_out_loc);
    
    char * dir = getcwd(NULL, 0);
    printf("Current dir: %s", dir);
    cout << endl;
    
    // contructor of AS, if no config pop data avaiable, synthetic generation will be called to generate pop
    // otherwise, the function will read pop config data to contruct the pop
    cblok *cbk = new cblok(as_cid, "American Samoa", as_lat, as_long);
    
    string data = outdir;
    data = data + syn_mosquitoes;
    
    ofstream out(data.c_str());
    for(map<int, mblok*>::iterator j = cbk->mbloks.begin(); j != cbk->mbloks.end(); ++j){
        out << cbk->mbloksIndexB[j->first] << ",";
    }
    out << endl;
    out.close();
    
    string MDAScenarioLoc = parameters; MDAScenarioLoc = MDAScenarioLoc + MDAParams;
    int NumMDAScenarios = count_mda_scenarios(MDAScenarioLoc);
    cout << "There are " << NumMDAScenarios << " scenarios" << endl;
    for(int ScenarioCount = 0; ScenarioCount<NumMDAScenarios; ++ScenarioCount){
        mda_strat strategy = get_nth_mda_strat(MDAScenarioLoc,ScenarioCount+1);
        strategy.print_mda_strat();
        for(int i = 0; i < strategy.NumSims; ++i){ // for every simulation
            // Set random seeds -- I am getting a new seed for each simulation so that each simulation is reproducible (rather than having to run the whole batch to reproduce)
            seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
            srand(seed);
            std::default_random_engine generator(seed);
            srand48(seed);
            
            SimulationNumber = i+1;
            auto t = time(nullptr);
            auto tm = *localtime(&t);
            SimulationDateStr = put_time(&tm, "%Y/%m/%d %H:%M:%S");
            
            cbk->reset_cpop();
            cbk->reset_prv();
            
            for(int year = 0; year < strategy.SimYears; ++year){ // for each year
                cbk->sim_pop(year,strategy);
            }
        }
    }
    
    return 0;
}
