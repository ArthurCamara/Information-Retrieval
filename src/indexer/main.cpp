//
//  main.cpp
//  indexer
//
//  Created by Arthur Câmara on 9/3/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#include <iostream>
#include <ctime>
#include "Indexer.h"


using namespace std;

int main(int argc, const char * argv[]) {
  
  cout<<argv[0]<<endl;
  string input_collection_index;
  string input_directory;
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
  
  }
  clock_t begin = clock();
  if(is_merge){
    Indexer indexer(input_directory, input_collection_index, is_merge);
  }
  else {
    Indexer indexer(input_directory, input_collection_index);
  }
  clock_t end = clock();

  double elapsed_seconds = double (end-begin)/CLOCKS_PER_SEC;

  cout<<elapsed_seconds<<"s"<<endl;

  return 0;
}