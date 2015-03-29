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
#define MAXMEMORY 400

using namespace std;
using namespace htmlcxx;

int main(int argc, const char * argv[]) {
  
  string inputCollectionIndex(argv[1]);
  
  string inputDirectory(argv[2]);
  
  RICPNS::CollectionReader * reader = new RICPNS::CollectionReader(
      inputDirectory,
      inputCollectionIndex);
  
  RICPNS::Document doc;

  doc.clear();
  
  vector<Page> p;
  
  string data;
  string url;

  size_t bytesOccupied=0;
  
  while(reader->getNextDocument(doc)){
    data = doc.getText();
    url = doc.getURL();
    
    if (bytesOccupied+sizeof(data) >= MAXMEMORY){
      
      for (unsigned long i =0 ; i<p.size(); ++i) {
        //TODO: Write tuples to files
      }
      //erase the vector. vector<T>::clear does not guarantee to free memory.
      vector<Page>().swap(p);
      bytesOccupied = 0;
    }
    
    Page *aux = new Page(data, url);
    bytesOccupied+=sizeof(data);
    doc.clear();
    p.push_back(*aux);
    delete aux;
  }
  for (unsigned long i =0 ; i<p.size(); ++i) {
        //TODO: Write tuples to files
  }
  vector<Page>().swap(p);
  
  delete reader;
  return 0;
}
