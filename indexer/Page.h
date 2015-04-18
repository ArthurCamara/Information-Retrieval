//
//  Page.h
//  
//
//  Created by Arthur Câmara on 18/4/15.
//
//

#ifndef ____Page__
#define ____Page__


#include <string>
#include <iostream>
#include <unordered_map>
#include <gumbo.h>

typedef unsigned uint;

using namespace std;
class Page{
  string url_;
  string data_;
  string text_;
  string title_;
  string description_;
  uint id_;
  string content_type_;

  void parseData (string data);
  string cleanText(GumboNode* node);
  void removeHttpHeader(string& http);
  void generate_keywords(string clearText);
  
public:
  unordered_map<string, uint> keywords_;
  Page(string data, string url, uint pid);
  const unordered_map<string, uint> keywords() { return keywords_; };
  uint id() { return id_; }
  void erase_keywords() { keywords_.clear(); }
  
};

#endif /* defined(____Page__) */
