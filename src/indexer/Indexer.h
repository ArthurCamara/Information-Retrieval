//
//  parser.h
//  
//
//  Created by Arthur Câmara on 19/3/15.
//
//

#ifndef ____parser__
#define ____parser__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <tuple>
#include <queue>
#include "../../lib/RiCode/CollectionReader.h"

typedef unsigned uInt;

using namespace std;

struct heapComparator{
  inline bool operator()(const tuple<uint, uint, uint, uint>& lhs,
                         const tuple<uint, uint, uint, uint>& rhs){
    if (get<0>(lhs) == get<0>(rhs))
      return get<1>(lhs) > get<1>(rhs);
    return get<0>(lhs) > get<0>(rhs);
  }
};

struct mySort {
  inline bool operator()(const tuple<uint, uint, uint>& lhs,
                         const tuple<uint, uint, uint>& rhs){
    if (get<0>(lhs) == get<0>(rhs))
      return get<1>(lhs) < get<1>(rhs);
    return get<0>(lhs) < get<0>(rhs);
  }
};


class Indexer{
private:
  //Reading
  uint number_of_documents_;
  uint number_of_runs_;
  size_t space_occupied_by_vector_;
  string input_directory_;
  string input_collection_index_;
  vector<tuple<uint, uint, uint> > keyword_vector_;
  unordered_map<uint, string> pagesIds_;
  void dumpIndex();
  void addKeywordsToKeywordVector(const unordered_map<string, uint>&,
                                  uint docid);
  void dumpPages();
  vector<tuple<uint, uint, uint> > test_;


  
  //Vocabulary
  uint vocabulary_size_;
  FILE* vocabulary_file_;
  unordered_map<string, pair<uint, uint> > vocabulary_;
  void updateVocabulary(unordered_map<string,  uint>);
  void dumpVocabulary();
  
  //Merging
  vector<uint> buff_;
  uint number_of_writes_on_merge;
  FILE* index_file_;
  priority_queue<tuple<uint, uint, uint, uint>,
                vector<tuple<uint, uint, uint, uint>>,
                heapComparator> heap_;
  void Merge();
  void dumpBuffer();
  
  
public:
  Indexer(string input_directory,
          string input_collection_index,
          bool merge = false);
  Indexer();
  void dumpData();
  void writeVocabulary();
  
};
#endif /* defined(____parser__) */
