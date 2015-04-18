//
//  main.cpp
//  indexer
//c
//  Created by Arthur Câmara on 9/3/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#include <iostream>
#include "CollectionReader.h"
#include <html/ParserDom.h>
#include <ctime>
#include "Indexer.h"

using namespace std;
using namespace htmlcxx;

int main(int argc, const char * argv[]) {
  
  string input_collection_index;
  string input_directory;
  
  if(argc<5){
    cout<<"Parameters missing"<<endl;
    return 0;
  }
  
  //Arguments parsing
  for(unsigned i =1; i< argc; ++i) {
    string param  = argv[i];
    //index file name
    if(param == "--index" || param == "-i") {
      i++;
      input_collection_index = argv[i];
    }
    //index file path
    else if (param == "--directory" || "-d") {
      i++;
      input_directory = argv[i];
    }
  
  }
  
  assert(input_directory.length() != 0);
  assert(input_collection_index. length() != 0);
  
  clock_t begin = clock();
  Indexer indexer(input_directory, input_collection_index);
  clock_t end = clock();

  double elapsed_seconds = double (end-begin)/CLOCKS_PER_SEC;
  
  cout<<elapsed_seconds<<"s"<<endl;
  
  return 0;
}
