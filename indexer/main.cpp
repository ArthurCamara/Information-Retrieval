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

using namespace std;
using namespace htmlcxx;

int main(int argc, const char * argv[]) {
//  // insert code here...
//  string html = "<html><body>hey</body></html>";
//  HTML::ParserDom parser;
//  tree<HTML::Node> dom = parser.parseTree(html);
//  
//  //Print whole DOM tree
//  cout << dom << endl;
//  
//  //Dump all links in the tree
//  tree<HTML::Node>::iterator it = dom.begin();
//  tree<HTML::Node>::iterator end = dom.end();
//  for (; it != end; ++it)
//  {
//    if (strcasecmp(it->tagName().c_str(), "A") == 0)
//    {
//      it->parseAttributes();
//      cout << it->attribute("href").second << endl;
//    }
//  }
//  
//  //Dump all text of the document
//  it = dom.begin();
//  end = dom.end();
//  for (; it != end; ++it)
//  {
//    if ((!it->isTag()) && (!it->isComment()))
//    {
//      cout << it->text();
//    }
//  }
//  cout << endl;

  string inputDirectory("/Users/arthur/Dropbox/UFMG/DCC/Information Retrieval/toyExample");
  string inputCollectionIndex("indexToCompressedColection.txt");
  RICPNS::CollectionReader * reader = new RICPNS::CollectionReader(inputDirectory, inputCollectionIndex);
  RICPNS::Document doc;
  doc.clear();
  while(reader->getNextDocument(doc)){
    cout<<doc.getText()<<endl<<endl<<endl<<endl;
    doc.clear();
  }
  delete reader;
  return 0;
}
