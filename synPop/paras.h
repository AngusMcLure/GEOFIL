//
//  headers.h
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef headers_h
#define headers_h

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>
#include <set>
#include <map>

#define sim_bg              2010
#define sim_nd              2050
#define unit_types          5

#define vg_agrps            12
#define age_grps            16
#define max_ages            79

#define min_marital_age     15
#define max_marital_age     69
#define marital_agrps       11            // int((max_marital_age - min_marital_age)/5)+1
#define marital_ages        55
#define marital_a_m         4.16433
#define marital_b_m         0.9225
#define marital_a_f         5.57703
#define marital_b_f         0.90252

#define mu                  6.06          //household Gaussian
#define sigma               2.86
#define lambda              6.47          //household Poisson

#define max_births          10
#define birth_window        365
#define male_born           0.522         //probablity of newborn to be male
#define marg_relocation     0.0
#define adult_relocation    0.0
#define mortality_improve   0.99
#define annual_divorce      0.0001
#define hhold_rup_p_1       0.8
#define hhold_rup_p_2       0.2
#define hhold_rup_p_3       1.0/3

#define cannery_avg         0.176
#define commuting_ratio     0.703       //2010 data, 289/411

//parameters for risk
#define r_r                 100            //risk range, 100m average, Aedes polynesiensis
#define rb_working          50             //bitten rate per half day
#define rb_offwork          50
#define r_w                 0.1412         //probability of presence of mated worms due to one bite
#define c0_4                0.25           //relative exposure to mosquitos, age 0-4
#define c5_15               0.75           //relative exposure to mosquitos, age 5-15
#define s_l3                0.1093         //survive to more than 13 days (L3 larave)
#define r_i                 0.3881         //infected of mosquitos survive to L3
//#define s_w               0.0005833    //survive to adult worms

//epidemic seeds
#define iter                20

//functions
int ztpoisson(double l_lambda);
int binomial(int r, double p);
double ztpoisson(int k, double l_lambda);
double gaussian(double m_mu, double s_sigma);

//files ----------------------------------------------------------------------------------
//paths
#define datadir                 "/Users/stingxu/Desktop/Coding/SynPop/data/"
#define config                  "/Users/stingxu/Desktop/Coding/SynPop/$config/"
#define config_pop              "/Users/stingxu/Desktop/Coding/SynPop/$config/pop/"
#define config_hhold            "/Users/stingxu/Desktop/Coding/SynPop/$config/hold/"
#define config_unit             "/Users/stingxu/Desktop/Coding/SynPop/$config/unit/"
#define config_bldg             "/Users/stingxu/Desktop/Coding/SynPop/$config/bldg/"
#define parameters              "/Users/stingxu/Desktop/Coding/SynPop/parameters/"
#define outdir                  "/Users/stingxu/Desktop/Coding/SynPop/output/"

//parameters
#define fertility               "fertility.csv"
#define net_pop_loss            "net_migrants.csv"
#define mortality_male          "mortality_male.csv"
#define mortality_female        "mortality_female.csv"
#define labor_force_pp_rate     "LFPRs.csv"
#define exposure_age            "exposure_age.csv"

//demographics
#define villages                "villages.dat"
#define village_pop_gender      "village_pop_gender.dat"
#define village_pop_age         "village_pop_age.dat"
#define population_age          "pop_age.dat"
#define sex_ratio_age           "sex_ratio_age.dat"
#define village_units           "village_households.dat"
#define marital_male            "marital_male.csv"
#define marital_female          "marital_female.csv"
#define number_of_children      "number_of_childen_ever_born.csv"
#define live_birth              "live_birth_by_mother_and_order.csv"

//land use data
#define AS_rbldgs_origin        "AS_rbldgs_origin.csv"
#define AS_schols_origin        "AS_schols_origin.csv"
#define AS_workps_origin        "AS_workps_origin.csv"
#define AS_rbldgs               "AS_rbldgs.dat"
#define AS_workps               "AS_workps.dat"
#define AS_schols               "AS_schols.dat"
#define AS_occupy_rbldgs        "AS_occupy_rbldgs.dat"
#define AS_bldg_risk_range      "AS_bldg_risk_range.dat"

//geographics
#define village_coordinates     "villages_lat_lon.csv"
#define village_excluded        "village_excluded.dat"

#define vil_euclid              "vil_euclid.dat"
#define vil_road                "vil_road.dat"

//synpop
#define syn_pop                 "syn_pop.dat"
#define syn_units               "syn_units.dat"
#define syn_hhold_members       "syn_hhold_members.dat"
#define syn_relationship        "syn_relationship.dat"
#define syn_pop_stat            "syn_pop_stat.csv"
#define syn_hhold_stat          "syn_hhold_stat.csv"
#define syn_sex_ratios          "syn_sex_ratios.dat"
#define syn_sex_ratios_broad    "syn_sex_ratios_broad.csv"
#define syn_geo_stat            "syn_geo_stat.dat"
#define syn_total_pop           "syn_total_pop.dat"

#endif /* headers_h */
