//
//  parser.cpp
//  
//
//  Created by Arthur Câmara on 19/3/15.
//
//

#include "parser.h"
#include <string>

using namespace htmlcxx;
using namespace std;

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
            transform(a.begin(), a.end(), a.begin(), ::tolower);
            if (a == "description")
              description_ = it->attribute("content").second;
            else if (a == "keywords")
              keywords_ = it->attribute("content").second;
          }
          string a = it->attribute("http-equiv").second;
          transform(a.begin(), a.end(), a.begin(), ::tolower);
          if (it->attribute("http-equiv").first == true &&
              it->attribute("http-equiv").second == "content-type") {
            contenttype_ = it->attribute("content").second;
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
}