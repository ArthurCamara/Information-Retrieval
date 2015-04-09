//
//  main.cpp
//  indexer
//
//  Created by Arthur Câmara on 9/3/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#include <iostream>
#include "CollectionReader.h"
#include <html/ParserDom.h>
#include "parser.h"

#define MAXMEMORY 40000000

using namespace std;
using namespace htmlcxx;

int main(int argc, const char * argv[]) {
  
  if(argc<3){
    cout<<"Parameters missing"<<endl;
    return 0;
  }
  
  string inputCollectionIndex(argv[1]);
  
  string inputDirectory(argv[2]);

  Parser parser(argv[2], argv[1]);

  return 0;
}
