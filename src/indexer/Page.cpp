//
//  Parser.cpp
//  
//
//  Created by Arthur Câmara on 18/4/15.
//
//

#include "Page.h"
#include <gumbo.h>
#include <boost/tokenizer.hpp>
//#include <boost/locale/util.hpp>
//#include <boost/locale.hpp>
#include <boost/foreach.hpp>

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


Page::Page(string data, string url, uint pid) {
  url_ = url;
  id_ = pid;
  parseData(data);
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

void Page::parseData(string data) {
  removeHttpHeader(data);
  GumboOutput* output = gumbo_parse(data.c_str());
  string cleantext = cleanText(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  generate_keywords(cleantext);
  
}

string Page::cleanText(GumboNode *node) {
  if(node->type == GUMBO_NODE_TEXT)
    return string(node->v.text.text);
  if (node->type == GUMBO_NODE_ELEMENT &&
      node->v.element.tag != GUMBO_TAG_SCRIPT &&
      node->v.element.tag != GUMBO_TAG_STYLE) {
    string contents = "";
    GumboVector* children = &node->v.element.children;
    for (uint i = 0; i< children->length; ++i) {
      const string text = cleanText((GumboNode*)children->data[i]);
      if(i != 0 && !text.empty()) {
        contents.append(" ");
      }
      contents.append(text);
    }
    return contents;
  }else return "";
}
void Page::generate_keywords(string clearText){
  using namespace boost;
  char_separator<char> sep(" , \b!?:;\"'`.()[]{}-_<>~|\\/\n\t*");
  tokenizer<char_separator<char>> tokens(clearText, sep);
  BOOST_FOREACH(const string&t, tokens){
    string str = t;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    str = trim(str);
    if(str.size()>1) {
      if(keywords_.find(str)==keywords_.end()) {
        keywords_.insert(make_pair(str, 1));
      }
      else {
        keywords_[str]++;
      }
    }
  }
}

void Page::removeHttpHeader(string& http){
  size_t pos = http.find("<!DOC");
  if(!pos || pos == string::npos) return;
  http.erase(0, pos-1);
}

