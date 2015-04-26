//
//  QueryProcessing.h
//  
//
//  Created by Arthur Câmara on 22/4/15.
//
//

#ifndef ____QueryProcessing__
#define ____QueryProcessing__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#endif /* defined(____QueryProcessing__) */

using namespace std;

class QueryProcessing{
private:
  unordered_map<string, pair<uint, uint>> vocabulary_;
  unordered_map<uint, string> pages_;
  set<uint> retrieveForKeyword(string q);
  
public:
  QueryProcessing();
  void retrieve(string q);
};