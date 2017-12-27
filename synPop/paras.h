//
//  headers.h
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef headers_h
#define headers_h

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <random>
#include <ctime>
#include <map>

#define sim_bg              2010
#define sim_nd              2050
#define unit_types          5

#define min_marital_age     15
#define max_marital_age     59
#define marital_a_m         4.16433
#define marital_b_m         0.9225
#define marital_a_f         5.57703
#define marital_b_f         0.90252

//func for generating households
typedef int (*pf)(double *p);
int r_size(pf f, double *p);
int gaussian(double *p);
int ztpoisson(double *p);


//files ----------------------------------------------------------------------------------
//paths
#define datadir                 "/Users/stingxu/Desktop/Coding/SynPop/data/"
#define config                  "/Users/stingxu/Desktop/Coding/SynPop/$config/"
#define parameters              "/Users/stingxu/Desktop/Coding/SynPop/parameters/"
#define outdir                  "/Users/stingxu/Desktop/Coding/SynPop/output/"

//parameters
#define fertility               "fertility.csv"
#define net_pop_loss            "net_migrants.csv"
#define mortality_male          "mortality_male.csv"
#define mortality_female        "mortality_female.csv"
#define marital_age             "marital_age.dat"

//demographics
#define villages                "villages.dat"
#define village_pop_gender      "village_pop_gender.dat"
#define village_pop_age         "village_pop_age.dat"
#define population_age          "pop_age.dat"
#define sex_ratio_age           "sex_ratio_age.dat"
#define household_types         "household_types.dat"
#define village_units           "village_households.dat"

//land use data
#define AS_rbldgs_origin        "AS_rbldgs_origin.csv"
#define AS_sbldgs_origin        "AS_sbldgs_origin.csv"
#define AS_wbldgs_origin        "AS_wbldgs_origin.csv"
#define AS_rbldgs               "AS_rbldgs.dat"
#define AS_sbldgs               "AS_sbldgs.dat"
#define AS_wbldgs               "AS_wbldgs.dat"
#define AS_bldg_dist            "syn_bldg_distance.dat"

//geographics
#define village_coordinates     "villages_lat_lon.dat"
#define village_excluded        "village_excluded.dat"

#define vil_euclid_origin       "vil_euclid_origin.dat"
#define vil_road_origin         "vil_road_origin.dat"
#define vil_euclid              "vil_euclid.dat"
#define vil_road                "vil_road.dat"

//synpop
#define syn_pop                 "syn_pop.dat"
#define syn_units               "syn_units.dat"
#define syn_hhold_members       "syn_hhold_members.dat"
#define syn_relationship        "syn_relationship.dat"
#define syn_pop_stat            "syn_pop_stat.dat"
#define syn_hhold_stat          "syn_hhold_stat.dat"
#define syn_sex_ratios          "syn_sex_ratios.dat"
#define syn_sex_ratios_broad    "syn_sex_ratios_broad.dat"
#define syn_geo_stat            "syn_geo_stat.dat"
#define syn_total_pop           "syn_total_pop.dat"

#endif /* headers_h */
