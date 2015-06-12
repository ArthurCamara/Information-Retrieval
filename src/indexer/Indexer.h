//
//  Indexer.h
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
#include <unordered_set>
#include <htmlcxx/html/utils.h>
#include <htmlcxx/html/Uri.h>
#include "../../lib/RiCode/CollectionReader.h"

typedef unsigned uInt;

using namespace std;


//Comparator for heapsort
struct heapComparator{
  inline bool operator()(const tuple<uint, uint, uint, uint>& lhs,
                         const tuple<uint, uint, uint, uint>& rhs){
    if (get<0>(lhs) == get<0>(rhs))
      return get<1>(lhs) > get<1>(rhs);
    return get<0>(lhs) > get<0>(rhs);
  }
};
struct internalHeapComparator{
  inline bool operator()(const tuple<uint, uint, uint>& lhs,
                         const tuple<uint, uint, uint>& rhs){
    if (get<0>(lhs) == get<0>(rhs))

      return get<1>(lhs) < get<1>(rhs);
    return get<0>(lhs) < get<0>(rhs);
  }
};


//Comparator for sorting keywords
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
  uint number_of_anchor_files_;
  size_t space_occupied_by_vector_;
  size_t space_occupied_by_links_;
  size_t space_occupied_by_anchor_texts_;
  string input_directory_;
  string input_collection_index_;

  vector<tuple<uint, uint, uint> > keyword_vector_;
//  unordered_map<uint, unordered_set<uint>> page_links_;
  vector<pair<uint, uint>> page_links_;
  vector<tuple<uint, uint, uint>> anchor_texts_;
  unordered_map<uint, string> pagesIds_;
  unordered_map<string, uint> pagesUri_;

  void addKeywordsToKeywordVector(const unordered_map<string, uint>&, uint docid);
  void updateAnchorTextsAndLinks(const unordered_map<string, unordered_map<string, uint>>&, uint);
  void dumpPages();
  void dumpIndex();
  void dumpAnchor();
  void dumpLinks();
  
  
  


  
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
  void Merge(string outfile, string inprefix, unsigned nruns);
  void dumpBuffer();
  void MergeAnchor(string outfile, string inprefix, unsigned nruns);
  
public:
  Indexer(string input_directory,
          string input_collection_index,
          bool merge = false);
  Indexer();
  void dumpData();
  void writeVocabulary();
  
};
#endif /* defined(____parser__) */
