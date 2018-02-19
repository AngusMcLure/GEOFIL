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
    
    struct node{
        int mid;
        double d;
        
        node(int mid, double d): mid(mid), d(d){ }
    };
    
    struct _comp_node{
        bool operator() (const node *p, const node *q){ return (p->d < q->d);}
    } _closer;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *src = j->second;
        int src_id = src->mid;
        
        vector<node*> vec;
        
        for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
            mblok *dst = k->second;
            int dst_id = dst->mid;
            
            if(dst_id == src_id) continue;
            
            int index = (min(src_id, dst_id)-1)*(meshblocks*2-min(src_id, dst_id))/2 + abs(dst_id-src_id) - 1;
            vec.push_back(new node(dst_id, d[index]));
        }
        
        stable_sort(vec.begin(), vec.end(), _closer);
        
        double mi = src->jobs, ti = src->jobs*commuting_ratio, total = 0;
        for(int k = 0; k < vec.size(); ++k){
            mblok *dst = mbloks[vec[k]->mid];
            
            double nj = dst->jobs, sij = 0;
            for(int i = 0; i < k; ++i) sij += mbloks[vec[i]->mid]->jobs;
            
            double tij = ti*mi*nj/(mi+nj)/(mi+nj+sij);
            
            map<int, double>::iterator it = src->mblok_comm.find(dst->mid);
            if(it == src->mblok_comm.end()) src->mblok_comm.insert(pair<int, double>(dst->mid, tij));
            else it->second = tij;
            
            total += tij;
        }
        
        for(map<int, double>::iterator k = src->mblok_comm.begin(); k != src->mblok_comm.end(); ++k){
            k->second = k->second/total*commuting_ratio;
        }
        
        src->mblok_comm[src->mid] = (1 - commuting_ratio);
        
        for(int k = 0; k < vec.size(); ++k) delete vec[k];
        vec.clear(); vec.shrink_to_fit();
    }
}
