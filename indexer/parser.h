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
  unsigned long id_;
  //unordered_map<word, freq>
  string content_type_;
  unordered_map<string, string> links_;
  void generate_keywords();
  
 public:
  Page();
  Page(string data, string url, unsigned long pid);
  unordered_map<string, unsigned long> keywords_;
  void parseData();
  string printableForIndex(unordered_map<string, pair<unsigned long, unsigned long> >);
  
  string url() { return url_; }
  string data() { return data_; }
  string text() { return text_; }
  string title() { return title_; }
  unsigned long id() { return id_; }
  string description() { return description_; }
  const unordered_map<string, unsigned long> keywords() { return keywords_; }
  string content_type() { return content_type_; }
  unordered_map<string, string> getLinks() { return links_; }
};


class Parser{
private:
  unsigned long number_of_documents;
  unsigned long vocabulary_size_;
  vector<Page> documents_;
  size_t size_of_documents_;
  FILE* index_file_;
  FILE* vocabulary_file_;
  string input_directory_;
  string input_collection_index_;
  //Vocabulary_ <word,<id, freq>>
  unordered_map<string, pair<unsigned long, unsigned long> > vocabulary_;
  void updateVocabulary(unordered_map<string, unsigned long>);
  void dumpVocabulary();
  void dumpIndex();
  
public:
  size_t size_of_documents() { return size_of_documents_; }
  vector<Page> documents() { return documents_; }
  void dumpData();
  void writeVocabulary();
  Parser(string input_directory,string input_collection_index);
  Parser();
  
};
#endif /* defined(____parser__) */
