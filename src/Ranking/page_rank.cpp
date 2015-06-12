//
//  page_rank.cpp
//  Information Retrieval
//
//  Created by Arthur Câmara on 12/6/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#include "page_rank.h"


using namespace std;

typedef vector<pair<unsigned,double>> EdgeList;

PageRank::PageRank(string inputfile){
  cout<<"calculating pageRank"<<endl;
  FILE* pageRankFile = fopen(inputfile.c_str(), "rb");
  struct stat info;
  stat(inputfile.c_str(), &info);
  vector<uint> buffer(info.st_size);
  fread(&buffer[0], sizeof(uint), info.st_size, pageRankFile);
  fclose(pageRankFile);
  uint nv, ne;
  ne = (uint) buffer.size()/2;
  //count number of nodes in the graph
  nv = 0;
  unordered_map<uint, uint> number_of_edges;
  for(unsigned it = 0; it<buffer.size(); ++it){
    if (it>nv) nv = it;
    if(nv%2==0)
      number_of_edges[it]++;
  }
  vector<EdgeList> inlinks(nv);
  for (unsigned i = 0; i<buffer.size(); i+=2) {
    double w = 1/number_of_edges[buffer[i]];
    int s=buffer[i], d=buffer[i+1];
    inlinks[d].push_back(make_pair(s,w));
  }
  
  while(1){
    unordered_map<unsigned, double> p2;
    for (int d = 0; d<nv; ++d){
      p2[d]=0;
      for (auto edge: inlinks[d])
        p2[d] += page_score_[edge.first]*edge.second;
    }
    
    double max = -1;
    for (int d = 0 ; d < nv ; ++d) {
      double _max = abs(p2[d]-page_score_[d]);
      if (_max > max)
        max = _max;
    }
    page_score_ = p2;
    if (max < 0.0001)
      break;
  }
}