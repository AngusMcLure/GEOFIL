//
//  main.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include <iostream>
#include <ctime>
#include "main.h"
using namespace std;

//int rb_working, rb_offwork;

int main(int argc, const char * argv[]) {
	unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	std::default_random_engine generator(seed);
	srand48(seed);
    
    // insert code here...
    cblok *cbk = new cblok(as_cid, "American Samoa", as_lat, as_long);
    
    //int rb_working_array[4] = {50, 60, 40, 40};
    //int rb_offwork_array[4] = {50, 40, 60, 40};
    
    /*for(int p = 0; p < 4; ++p){
        rb_working = rb_working_array[p];
        rb_offwork = rb_offwork_array[p];*/
        
        for(int i = 0; i < iter; ++i){
            cbk->reset_cpop();
            cbk->reset_prv();
            for(int year = 0; year < 40; ++year)
                cbk->sim_pop(year);
            
            string prv_dat = outdir;    prv_dat = prv_dat + "prv_dat.csv";
            ofstream out;   ifstream in;
            in.open(prv_dat.c_str());
            if(!in){
                out.open(prv_dat.c_str());
                out << "Year,";
                for(int i = 2010; i < 2050; ++i) out << i << ",";
                out << endl;
                out.close();
            }
            else in.close();
            
            out.open(prv_dat.c_str(), ios::app);
            out << ">=15 yro";
            for(int j = 0; j < 40; ++j) out << "," << cbk->adult_prv[j];
            out << endl;
            
            out << "6-7 yro";
            for(int j = 0; j < 40; ++j) out << "," << cbk->child_prv[j];
            out << endl;
            
            out << "overall";
            for(int j = 0; j < 40; ++j) out << "," << cbk->all_prv[j];
            out << endl;
            
            double sum = 0;
            for(map<int, mblok*>::iterator j = cbk->mbloks.begin(); j != cbk->mbloks.end(); ++j){
                sum += j->second->sum_mf;
            }
            out.close();
            
            string mapping = outdir;    mapping = mapping + "risk_mapping.csv";
            in.open(mapping.c_str());
            if(!in){
                out.open(mapping.c_str());
                out << "village,";
                
                for(map<int, mblok*>::iterator j = cbk->mbloks.begin(); j != cbk->mbloks.end(); ++j){
                    out << cbk->mbloksIndexB[j->first] << ",";
                }
                out << endl;
                
                out.close();
            }
            else in.close();
            
            out.open(mapping.c_str(), ios::app);
            out << "percentage";
            for(map<int, mblok*>::iterator j = cbk->mbloks.begin(); j != cbk->mbloks.end(); ++j){
                out << "," << j->second->sum_mf/sum;
            }
            out << endl;
            out.close();
        }
    //}
    
    return 0;
}
