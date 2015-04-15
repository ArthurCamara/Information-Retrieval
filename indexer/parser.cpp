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
#include <sstream>


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

Page::Page(string data, string url, unsigned long pid){
  data_ = data;
  url_  = url;
  id_  = pid;
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
            transform(content.begin(), content.end(), content.begin(), ::tolower);
            transform(a.begin(), a.end(), a.begin(), ::tolower);
            if (a == "description")
              description_ = content;
            else if (a == "keywords"){
              if(keywords_.find(content)!=keywords_.end()){
                keywords_.insert(make_pair(content, 1));
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
        //parse links inside the page. Will be useful for PageRank in the future
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
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    str = trim(str);
    if(str.size()>1){
      if(keywords().find(str)==keywords().end()){
        keywords_.insert(make_pair(str, 1));
      }
      else{
        keywords_[str]++;
      }
    }
  }
  return;
}

string Page::printableForIndex(unordered_map<string,
                               pair<unsigned long, unsigned long> > vocabulary){
  string ret ="";
  bool should_add_divisor = false;
  for (auto it = keywords_.begin(); it!=keywords_.end(); ++it) {
    if(should_add_divisor)
      ret+="|";
    else
      should_add_divisor = true;
    string number, frequency, pageid;
    stringstream strstream;
    strstream<<vocabulary[it->first].first;
    strstream>>number;
    strstream.clear();
    strstream<<keywords_[it->first];
    strstream>>frequency;
    strstream.clear();
    strstream<<id_;
    strstream>>pageid;
    strstream.clear();
    ret += number + "," + pageid + "," + frequency;
  }
  return ret;
}

Parser::Parser(string input_directory,string input_collection_index){
  size_of_documents_= 0;
  vocabulary_size_ = 0;
  number_of_documents = 0;
  index_file_ = fopen("index.txt", "w");
  input_directory_  = input_directory;
  input_collection_index_ = input_collection_index;
  RICPNS::CollectionReader * reader = new  RICPNS::CollectionReader(
      input_directory_,
      input_collection_index_);
  
  RICPNS::Document doc;
  
  doc.clear();
  
  string data, url;
  
  while(reader->getNextDocument(doc)) {
    number_of_documents++;
//    if(counter%1000==0){
//      cout<<counter<<endl;
//    }
    data = doc.getText();
    url = doc.getURL();
    
    if (size_of_documents() + sizeof(data) >= kMaxMemory) {
      dumpIndex();
      vector<Page>().swap(documents_);
      size_of_documents_ = 0;
    }
    Page *reading_page = new Page(data, url, number_of_documents);
    updateVocabulary(reading_page->keywords());
    size_of_documents_ += sizeof(data);
    doc.clear();
    documents_.push_back(*reading_page);
    delete reading_page;
  }
  dumpIndex();
  fclose(index_file_);
  vector<Page>().swap(documents_);
  delete reader;
  dumpVocabulary();
  
}

void Parser::updateVocabulary(unordered_map<string, unsigned long> words){
  for(auto it = words.begin(); it!= words.end(); ++it) {
    if(vocabulary_.find(it->first)!=vocabulary_.end())
      vocabulary_[it->first].second+= it->second;
    else {
      vocabulary_size_++;
      vocabulary_.insert(make_pair(it->first, make_pair(vocabulary_size_, it->second)));
    }
  }
}

//Iterate over every word on vocabulary and write it as <word, id, frequency>
void Parser::dumpVocabulary(){
  vocabulary_file_ = fopen("vocabulary.bin", "wb");
  string wordid, frequency;
  stringstream strstream;
  string str;
  for (auto it = vocabulary_.begin(); it!= vocabulary_.end(); ++it) {
    strstream<<it->second.first;
    strstream>>wordid;
    strstream.clear();
    strstream<<it->second.second;
    strstream>>frequency;
    strstream.clear();
    str = it->first + "," + wordid +"," + frequency+"\n";
    fwrite(str.c_str(), 1, str.length(), vocabulary_file_);
  }
  fclose(vocabulary_file_);
}


void Parser::dumpIndex(){
  stringstream strstream;
  string frequency, wordid, docid;
  string str;
  for (auto doc = documents_.begin(); doc!= documents_.end(); ++doc) {
    str = "";
    strstream<<doc->id();
    strstream>>docid;
    strstream.clear();
    for (auto word = doc->keywords_.begin(); word != doc->keywords_.end(); ++word){
      strstream<<word->second;
      strstream>>frequency;
      strstream.clear();
      strstream<<vocabulary_[word->first].second;
      strstream>>wordid;
      strstream.clear();

      str += wordid+","+docid+","+frequency+"\n";
    }
    fwrite(str.c_str(), 1, str.length(), index_file_);
    str = "";
  }
  return;
}