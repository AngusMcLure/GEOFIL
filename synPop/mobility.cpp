//
//  blockMMod.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

//radiation model
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
    
    struct _comp_cnode_s{
        bool operator() (const mblok::c_node *p, const mblok::c_node *q){ return (p->p < q->p);}
    } _smaller;
    
    double c_1 = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *src = j->second;
        
        //initialize meshblock distance and commuting matrix
        if(src->mblok_comm.size() == 0){
            int src_id = src->mid;
            for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
                mblok *dst = k->second;
                int dst_id = dst->mid;
                
                if(dst_id == src_id) continue;
                
                int index = (min(src_id, dst_id)-1)*(meshblocks*2-min(src_id, dst_id))/2 + abs(dst_id-src_id) - 1;
                src->mblok_commuting.insert(pair<int, double>(dst_id, 0));
                src->mblok_comm.push_back(new mblok::c_node(dst_id, d[index]));
            }
            
            stable_sort(src->mblok_comm.begin(), src->mblok_comm.end(), _smaller);
        }
        
        double mi = double(src->mblok_males.size()+src->mblok_fmals.size()), ti = src->labors*commuting_ratio, total = 0;
        for(int k = 0; k < src->mblok_comm.size(); ++k){
            mblok *mbk = mbloks[src->mblok_comm[k]->mid];
            double nj = double(mbk->mblok_males.size()+mbk->mblok_fmals.size()), sij = 0;
            for(int i = 0; i < k; ++i){
                mbk = mbloks[src->mblok_comm[i]->mid];
                sij += double(mbk->mblok_males.size()+mbk->mblok_fmals.size());
            }
            
            double tij = ti*mi*nj/(mi+nj)/(mi+nj+sij);
            
            src->mblok_commuting[src->mblok_comm[k]->mid] = tij;
            total += tij;
        }
        
        for(map<int, double>::iterator k = src->mblok_commuting.begin(); k != src->mblok_commuting.end(); ++k){
            k->second = k->second/total * commuting_ratio;
        }
        
        src->mblok_commuting.insert(pair<int, double>(src->mid, 1-commuting_ratio));
        c_1 += src->mblok_commuting[mid] * src->labors;
    }
    
    double c_2 = labor_force * cannery_avg;
    double c_3 = (c_2 - c_1) / (labor_force * commuting_ratio - c_1);
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        double c_4 = mbk->mblok_commuting[mid];
        
        for(map<int, double>::iterator k = mbk->mblok_commuting.begin(); k != mbk->mblok_commuting.end(); ++k){
            if(k->first == j->first) continue;
            
            if(k->first == mid) k->second += (commuting_ratio - c_4) * c_3;
            else k->second -= k->second * c_3;
        }
    }
    
    /*string file = outdir;   file = file + "commuting.csv";
    ofstream out(file.c_str());
    out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, double>::iterator k = mbk->mblok_commuting.begin(); k != mbk->mblok_commuting.end(); ++k){
            if(k->second * mbk->labors >= 20)
                out << mbk->lat << "," << mbk->log << "," << mbloks[k->first]->lat << "," << mbloks[k->first]->log << "," << k->second*mbk->labors << endl;
        }
    }
    out.close();*/
}
