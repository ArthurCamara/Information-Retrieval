//
//  main.cpp
//  indexer
//
//  Created by Arthur Câmara on 9/3/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <fstream>
#include "Indexer.h"
#include "QueryProcessing.h"


using namespace std;

int main(int argc, const char * argv[]) {
  std::ios::sync_with_stdio(false);
  
  ifstream arq(getenv("MYARQ"));
  cin.rdbuf(arq.rdbuf());
  
  cout<<argv[0]<<endl;
  string input_collection_index;
  string input_directory;
  string query;
  bool is_retrieval=false;
  bool is_merge = false;
  
  //Arguments parsing
  for(unsigned i = 1; i < argc; ++i) {
    string param  = argv[i];
    //index file name
    if(param == "--index" || param == "-i") {
      i++;
      input_collection_index = argv[i];
    }
    //index file path
    else if (param == "--directory" || param == "-d") {
      i++;
      input_directory = argv[i];
    }
    else if (param == "--retrieval" || param == "-r") {
      is_retrieval = true;
    }
    else if (param == "--merge" || param == "-m"){
      is_merge = true;
    }
    else if (param == "--retrieval" || param == "--r"){
      is_retrieval = true;
    }
  
  }
  clock_t begin = clock();
  if(is_merge){
    Indexer indexer(input_directory, input_collection_index, is_merge);
  }
  else if(is_retrieval){
    QueryProcessing q;
    while (getline(cin, query)){
      cout<<"Lookout for query "<<query<<endl;
      q.retrieve(query);
    }
  }
  else {
    Indexer indexer(input_directory, input_collection_index);
  }
  clock_t end = clock();

  double elapsed_seconds = double (end-begin)/CLOCKS_PER_SEC;

  cout<<elapsed_seconds<<"s"<<endl;

  return 0;
}