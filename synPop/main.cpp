////
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

int SimulationNumber;
unsigned seed;
auto t = time(nullptr);
auto tm = *localtime(&t);

random_device r;
seed_seq seeds{r(), r(), r(), r(), r(), r(), r(), r()};
mt19937 gen(seeds);


string prv_out_loc;


int main(int argc, const char * argv[]) {
    
    
    prv_out_loc = argv[1];
    
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

    string TargettedMDA = parameters; TargettedMDA = TargettedMDA + TMDA;
    string MDAScenarioLoc = parameters; MDAScenarioLoc = MDAScenarioLoc + MDAParams;

    int NumMDAScenarios = count_mda_scenarios(MDAScenarioLoc);
    cout << "There are " << NumMDAScenarios << " scenarios" << endl;

    
    mda_strat strategy = get_nth_mda_strat(MDAScenarioLoc,1);
    strategy.print_mda_strat();

    for(int i = 0; i < strategy.NumSims; ++i){ // for every simulation
            // Set random seeds -- I am getting a new seed for each simulation so that each simulation is reproducible (rather than having to run the whole batch to reproduce)
            seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
            srand(seed);

            SimulationNumber = i+1;
            auto t = time(nullptr);
            auto tm = *localtime(&t);
            //SimulationDateStr = put_time(&tm, "%Y/%m/%d %H:%M:%S");
            
            cbk->reset_cpop();
            cbk->reset_prv();
            
            for(int year = 0; year < strategy.SimYears; ++year){ // for each year
                cout << "Year is: " << year << endl;
                cbk->sim_pop(year,strategy);
            }
    }
    
    return 0;
}
