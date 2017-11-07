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
#include <cstring>
#include <vector>
#include <random>
#include <ctime>
#include <map>

#define sim_bg              2010
#define sim_nd              2050
#define unit_types          5

//func for generating households
typedef int (*pf)(double *p);
int r_size(pf f, double *p);

#endif /* headers_h */
