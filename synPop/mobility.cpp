//
//  blockMMod.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::radt_model(char m){
    double *d = NULL;
    if(m == 'r') d = road_dst;
    else if(m == 'e') d = euclid_dst;
    
    double c_jobs = cannery_avg;
    
    int mid = mbloksIndexA["Atuu"];
    double total_pop = cpop;
    total_pop -= int(mbloks[mid]->mblok_males.size() + mbloks[mid]->mblok_fmals.size());
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        int mpop = int(mbk->mblok_males.size() + mbk->mblok_fmals.size());
        
        if(mbk->mid != mid) mbk->jobs = labor_force*(1-c_jobs)*mpop/total_pop;
        else mbk->jobs = labor_force*c_jobs;
    }
    
    struct _comp_node_s{
        bool operator() (const mblok::node *p, const mblok::node *q){ return (p->p < q->p);}
    } _smaller;
    
    struct _comp_node_l{
        bool operator() (const mblok::node *p, const mblok::node *q){ return (p->p > q->p);}
    } _larger;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *src = j->second;
        int src_id = src->mid;
        
        for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
            mblok *dst = k->second;
            int dst_id = dst->mid;
            
            if(dst_id == src_id) continue;
            
            int index = (min(src_id, dst_id)-1)*(meshblocks*2-min(src_id, dst_id))/2 + abs(dst_id-src_id) - 1;
            src->mblok_comm.push_back(new mblok::node(dst_id, d[index]));
        }
        
        stable_sort(src->mblok_comm.begin(), src->mblok_comm.end(), _smaller);
        
        double mi = src->jobs, ti = src->jobs*commuting_ratio, total = 0;
        for(int k = 0; k < src->mblok_comm.size(); ++k){
            double nj = mbloks[src->mblok_comm[k]->mid]->jobs, sij = 0;
            for(int i = 0; i < k; ++i) sij += mbloks[src->mblok_comm[i]->mid]->jobs;
            
            double tij = ti*mi*nj/(mi+nj)/(mi+nj+sij);
            
            src->mblok_comm[k]->p = tij;
            total += tij;
        }
        
        for(int k = 0; k < src->mblok_comm.size(); ++k){
            src->mblok_comm[k]->p = src->mblok_comm[k]->p/total*commuting_ratio;
        }
        
        src->mblok_comm.push_back(new mblok::node(src->mid, 1-commuting_ratio));
        stable_sort(src->mblok_comm.begin(), src->mblok_comm.end(), _larger);
    }
    
    string file = outdir;   file = file + "tafuna.csv";
    ofstream out(file.c_str());
    mblok *tf = mbloks[mbloksIndexA["Tafuna"]];
    
    for(int i = 0; i < tf->mblok_comm.size(); ++i){
        out << mbloksIndexB[tf->mblok_comm[i]->mid] << "," << tf->mblok_comm[i]->p << endl;
    }
    out.close();
}
