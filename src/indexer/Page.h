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
#include <vector>
#include <htmlcxx/html/utils.h>
#include <htmlcxx/html/Uri.h>

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
  void getLinks(GumboNode* node);
  
  void removeHttpHeader(string& http);
  void generate_keywords(string clearText);
  void generate_anchors(string text, string uri);
  void getUrls();
  
  
public:
  unordered_map<string, uint> keywords_;
  unordered_map<string, unordered_map<string, uint> > anchor_texts_;
  Page(string data, string url, uint pid);
  const unordered_map<string, uint> keywords() { return keywords_; };
  uint id() { return id_; }
  void erase_keywords() { keywords_.clear(); }
  
};

#endif /* defined(____Page__) */
