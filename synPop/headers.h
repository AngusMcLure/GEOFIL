//
//  classes.h
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef classes_h
#define classes_h

#include <iostream>
#include <cstring>
#include <vector>
#include <random>
#include <ctime>
#include <map>

#define sim_bg              2010
#define sim_nd              2050
#define unit_types          5

//individuals
class agent;

//buildings
class rbldg;    //residential
class bbldg;    //business
class gbldg;    //govermental
class sbldg;    //school bldgs

//sites
class hhold;    //household
class workp;    //workplace
class schol;    //school

//census blocks
class mblok;   //meshblock, smallest census unit
class cblok;   //cities or other large census area

//pop by age groups
class agrp;
class agrps;

//func for generating households
typedef int (*pf)(double *p);
int r_size(pf f, double *p);



#endif /* classes_h */
