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
#include <html/ParserDom.h>
#include <html/utils.h>
#include <html/Uri.h>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <tuple>
#include <queue>
#include "CollectionReader.h"

typedef unsigned uInt;

using namespace std;

struct heapComparator{
  inline bool operator()(const tuple<uInt, uint, uInt, uInt>& lhs, const tuple<uInt, uint, uInt, uInt>& rhs){
    if (get<0>(lhs) == get<0>(rhs)){
      return get<1>(lhs) > get<1>(rhs);
    }
    return get<0>(lhs) > get<0>(rhs);
  }
};

struct mySort {
  inline bool operator()(const tuple<uInt, uInt, uInt>& lhs, const tuple<uInt, uInt, uInt>& rhs){
    if (get<0>(lhs) == get<0>(rhs)){
      return get<1>(lhs) < get<1>(rhs);
    }
    return get<0>(lhs) < get<0>(rhs);
  }
};


class Indexer{
private:
  unsigned int number_of_documents;
  unsigned int vocabulary_size_;
  unsigned int number_of_runs_;
  size_t space_occupied_by_vector_;
  uint number_of_indexed_documents_;
  FILE* index_file_;
  FILE* vocabulary_file_;
  string input_directory_;
  string input_collection_index_;
  vector<tuple<uInt, uInt, uInt> > keyword_vector_;
  priority_queue<tuple<uInt, uint, uInt, uInt>, vector<tuple<uInt, uint, uInt, uInt>>, heapComparator> heap_;
  vector<tuple<uInt, uInt, uInt> > output_buffer_;
  
  //Vocabulary_ <word,<id, freq>>
  unordered_map<string, pair<uInt, uInt> > vocabulary_;
  void updateVocabulary(unordered_map<string,  uInt>);
  void dumpVocabulary();
  void dumpIndex();
  void addKeywordsToArray(const unordered_map<string, uInt>&, uint docid);
  void Merge();
  void dumpBuffer();
  
  
public:
  size_t number_of_indexed_documents() { return number_of_indexed_documents_; }
  void dumpData();
  void writeVocabulary();
  Indexer(string input_directory,string input_collection_index);
  Indexer();
  
};
#endif /* defined(____parser__) */
