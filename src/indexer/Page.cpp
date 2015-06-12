//
//  Parser.cpp
//  
//
//  Created by Arthur Câmara on 18/4/15.
//
//

#include "Page.h"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace htmlcxx;

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
  getLinks(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  generate_keywords(cleantext);
  
}


//Clean the page text. Removes CSS, Javascript, etc.
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
  }
  else return "";
}


//Get links and Anchor text from page.
void Page::getLinks(GumboNode *node) {
  if(node->type != GUMBO_NODE_ELEMENT)
    return;
  GumboAttribute* href;
  if(node->v.element.tag == GUMBO_TAG_A &&
     (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
    GumboVector* children = &node->v.element.children;
    string anchor = "";
    
    //Get the anchor text related to the uri
    for (unsigned int i = 0; i < children->length; ++i) {
      const string text = cleanText((GumboNode*)children->data[i]);
      if (i != 0 && !text.empty()) {
        anchor.append(" ");
      }
      anchor.append(text);
    }
//    cout<<"Anchor: "<<anchor<<endl;
    string uri="";

    //Normalize URL
    try {
      uri = HTML::convert_link(href->value, url_);
    } catch (...) {
      cerr<<"Couldn't normalize url "<<href->value<<endl;
    }
    
    generate_anchors(anchor, uri);
    //not the first time this URL appears on this page
//    if(anchor_texts_.find(uri)!=anchor_texts_.end())
//      anchor_texts_[uri].push_back(anchor);
//    //First time this Page sees this URL
//    else {
//      vector<string> v;
//      v.push_back(anchor);
//      anchor_texts_[uri]=v;
//    }
  }
  else if(node->v.element.tag == GUMBO_TAG_TITLE) {
    if (node->v.element.children.length == 1) {
      GumboVector* children = &node->v.element.children;
      for(unsigned i = 0; i<children->length; ++i) {
        string text = cleanText((GumboNode *)children->data[i]);
        if (i != 0 && !text.empty()) {
          title_.append(" ");
        }
        title_.append(text);
      }
    }
  }
  //Recursive call for the other childredn
  GumboVector* children = &node->v.element.children;
  for(int i =0 ; i<children->length; ++i)
    getLinks(static_cast<GumboNode*>(children->data[i]));
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

void Page::generate_anchors(string text, string uri){
  if(anchor_texts_.find(uri)==anchor_texts_.end()){
    unordered_map<string, uint> aux;
    anchor_texts_[uri]=aux;
  }
  
  
  using namespace boost;
  char_separator<char> sep(" , \b!?:;\"'`.()[]{}-_<>~|\\/\n\t*");
  tokenizer<char_separator<char>> tokens(text, sep);
  BOOST_FOREACH(const string&t, tokens){
    string str = t;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    str = trim(str);
    if(str.size()>1) {
      if(anchor_texts_[uri].find(str)==anchor_texts_[uri].end()) {
        anchor_texts_[uri].insert(make_pair(str, 1));
      }
      else {
        anchor_texts_[uri][str]++;
      }
    }
  }
}

void Page::removeHttpHeader(string& http){
  size_t pos = http.find("<!DOC");
  if(!pos || pos == string::npos) return;
  http.erase(0, pos-1);
}

