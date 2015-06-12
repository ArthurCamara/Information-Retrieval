//
//  page_rank.h
//  Information Retrieval
//
//  Created by Arthur Câmara on 12/6/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#ifndef __Information_Retrieval__page_rank__
#define __Information_Retrieval__page_rank__

#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <iostream>

using namespace std;


class PageRank{
public:
  PageRank(string inputfile);
  double scoreForPage(unsigned page) { return page_score_[page]; }
  
private:
  //Results of pageRank
  unordered_map<unsigned, double> page_score_;
};

#endif /* defined(__Information_Retrieval__page_rank__) */
