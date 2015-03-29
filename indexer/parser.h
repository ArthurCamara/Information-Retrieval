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

using namespace std;
class Page{
  string url_;
  string data_;
  string text_;
  string title_;
  string description_;
  string keywords_;
  string contenttype_;
  unordered_map<string, string> links_;
  
 public:
  Page();
  Page(string data, string url);

  void parseData();
  
  string getURL() { return url_; }
  string getData() { return data_; }
  string getText() { return text_; }
  string getTitle() {return title_; }
  string getDescription() { return description_; }
  string getKeyWords() { return keywords_; }
  string getContentType() { return contenttype_; }
  unordered_map<string, string> getLinks() { return links_; }
  
};

#endif /* defined(____parser__) */
