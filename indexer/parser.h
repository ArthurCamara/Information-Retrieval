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
#include "CollectionReader.h"



using namespace std;
class Page{
  string url_;
  string data_;
  string text_;
  string title_;
  string description_;
  unordered_map<string, int> keywords_;
  string content_type_;
  unordered_map<string, string> links_;
  void generate_keywords();
  
 public:
  Page();
  Page(string data, string url);

  void parseData();
  
  string url() { return url_; }
  string data() { return data_; }
  string text() { return text_; }
  string title() { return title_; }
  string description() { return description_; }
  unordered_map<string, int> keywords() { return keywords_; }
  string content_type() { return content_type_; }
  unordered_map<string, string> getLinks() { return links_; }
  
};


class Parser{
private:
  vector<Page> documents_;
  size_t size_of_documents_;
  FILE* index_file_;
  FILE* vocabulary_file_;
  string input_directory_;
  string input_collection_index_;
  unordered_map<string, unsigned long> vocabulary_;
  
public:
  size_t size_of_documents() { return size_of_documents_; }
  vector<Page> documents() { return documents_; }
  void dumpData();
  void writeVocabulary();
  Parser(string input_directory,string input_collection_index);
  Parser();
  
};
#endif /* defined(____parser__) */
