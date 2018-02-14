//
//  dynamic_cell.cpp
//  synPop
//
//  Created by Sting Xu on 14/2/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#include "block.h"

schol::schol(int sid, string name, char level, double lat, double log, double radius){
    this->sid = sid;
    this->name = name;
    this->level = level;
    this->lat = lat;
    this->log = log;
    this->radius = radius;
    
    this->student.clear();
}

schol::~schol(){
    this->student.clear();
    this->name.clear();
}

void cblok::read_schools(){
    string file = datadir;      file = file + AS_schools;
    ifstream in(file.c_str());
    
    if(!in){
        cout << "err: school info not found in read_schools()!" << endl;
        exit(1);
    }
    
    string line;
    getline(in, line);
    
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        strncpy(str, line.c_str(), line.size());
        
        char *p = std::strtok(str, ",");    string name = p;
        p = std::strtok(NULL, ",");         char level = p[0];
        p = std::strtok(NULL, ",");         string cate = p;
        p = std::strtok(NULL, ",");         double lat = atof(p);
        p = std::strtok(NULL, ",");         double log = atof(p);
        
        double radius = 0;
        if(level == 'E') radius = 25;
        else if(level == 'H' || level == 'B') radius = 50;
        else if(level == 'C') radius = 100;
        
        schol *sh = new schol(next_sid++, name, level, lat, log, radius);
        cblok_schols.insert(pair<int, schol*>(sh->sid, sh));
        
        delete []str;
    }
}
