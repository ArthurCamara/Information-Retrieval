//
//  parser.cpp
//  
//
//  Created by Arthur Câmara on 19/3/15.
//
//

#include "parser.h"
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/locale.hpp>
#include <codecvt>
#include <locale>



using namespace htmlcxx;
using namespace std;

const unsigned long kMaxMemory = 40000000;

/*
 The following code is from Even Teran. Source is 
 http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 */
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
      std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
      std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}


Page::Page() {}

Page::Page(string data, string url){
  data_ = data;
  url_  = url;
  
  parseData();
}

void Page::parseData() {
  HTML::ParserDom parser;
  tree<HTML::Node> dom = parser.parseTree(data_);
  tree<HTML::Node>::iterator it = dom.begin();
  tree<HTML::Node>::iterator end = dom.end();
  
  text_ += " "+url_;
  
  for(; it != end; ++it) {
    if(it.node != 0 && dom.parent(it) != NULL) {
      //name of the parent tag that this node is in
      string parenttag = dom.parent(it)->tagName();
      transform(parenttag.begin(),
                parenttag.end(),
                parenttag.begin(),
                ::tolower);
      if(parenttag == "script" || parenttag == "noscript" || parenttag == "style") {
        it.skip_children();
        continue;
      }
      
      //main page body text parsing
      if((!it->isTag()) && (!it->isComment()))
        text_ += " " + it->text();
      //metadata parsing
      else {
        string tagname = it->tagName();
        transform(tagname.begin(), tagname.end(), tagname.begin(), ::tolower);
        
        if(tagname == "title") {
          it++;
          if (it == dom.end()) return;
          title_ = it->text();
        }
        
        else if(tagname == "meta") {
          it->parseAttributes();
          if(it->attribute("name").first == true) {
            string a = it->attribute("name").second;
            string content = it->attribute("content").second;
            transform(a.begin(), a.end(), a.begin(), ::tolower);
            if (a == "description")
              description_ = content;
            else if (a == "keywords"){
              if(keywords_.find(content)!=keywords_.end()){
                keywords_.insert(make_pair(content, 0));
              }
              else{
                keywords_[content]++;
              }
            }
          }
          string a = it->attribute("http-equiv").second;
          transform(a.begin(), a.end(), a.begin(), ::tolower);
          if (it->attribute("http-equiv").first == true &&
              it->attribute("http-equiv").second == "content-type") {
            content_type_ = it->attribute("content").second;
          }
        }
        //parse links inside the page. Will be useful for PageRank
        else if(tagname == "a") {
          it->parseAttributes();
          string a = it->attribute("rel").second;
          transform(a.begin(), a.end(), a.begin(), ::tolower);
          if(!(it->attribute("rel").first == true && a == "nofollow")) {
            string anchor;
            for (int i = 0; i < it.number_of_children(); ++i) {
              ++it;
              if(it == dom.end()) return;
              if(!it->isTag()) anchor += it->text();
            }
            links_[HTML::convert_link(it->attribute("href").second,
                                      url_)] = anchor;
            text_ += " " + anchor;
          }
        }
      }
    }
  }
  generate_keywords();
}

void Page::generate_keywords(){
  using namespace boost;
  char_separator<char> sep(", !?:;\"'`.()[]{}-_<>~|\\/\n\t");
  tokenizer<char_separator<char>> tokens(text_, sep);

  BOOST_FOREACH(const string &t, tokens) {
    string str = t;
    str = trim(str);
    if(str.size()>1){
      if(keywords().find(str)==keywords().end()){
        keywords_.insert(make_pair(str, 0));
      }
      else{
        keywords_[str]++;
      }
    }
  }
  return;
}

string Page::writableTriple(){
  return " ";
}

Parser::Parser(string input_directory,string input_collection_index){
  size_of_documents_=0;
  input_directory_  = input_directory;
  input_collection_index_ = input_collection_index;
  RICPNS::CollectionReader * reader = new  RICPNS::CollectionReader(
      input_directory_,
      input_collection_index_);
  
  RICPNS::Document doc;
  
  doc.clear();
  
  string data, url;
  
  long counter = 0;
  
  while(reader->getNextDocument(doc)) {
    counter ++;
//    cout<<counter<<endl;
    data = doc.getText();
    url = doc.getURL();
    
    if (size_of_documents() + sizeof(data) >= kMaxMemory) {
      for (unsigned long i = 0; i<documents_.size(); ++i) {
        //TODO process and print documents
      }
      vector<Page>().swap(documents_);
      size_of_documents_ = 0;
    }
    Page *reading_page = new Page(data, url);
    size_of_documents_ += sizeof(data);
    doc.clear();
    documents_.push_back(*reading_page);
    delete reading_page;
  }
  for (unsigned long i = 0; i<documents_.size(); ++i){
        //TODO process and print documents
  }
  vector<Page>().swap(documents_);
  delete reader;
}

void Parser::updateVocabulary(vector<string> words){
  for(unsigned int i = 0; i<words.size(); ++i) {
    if(vocabulary_.find(words[i])!=vocabulary_.end()){
      vocabulary_[words[i]]++;
    }
    else{
      vocabulary_.insert(make_pair(words[i], 0));
    }
  }
}
