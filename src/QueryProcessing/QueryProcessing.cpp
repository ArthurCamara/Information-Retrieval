//
//  QueryProcessing.cpp
//  
//
//  Created by Arthur Câmara on 22/4/15.
//
//

#include "QueryProcessing.h"


using namespace std;

QueryProcessing::QueryProcessing(){
  //First thing to do: Load the Vocabulary on memory
  //Can get better. Slow reading speeds.
  cout<<"Loading vocabulary and pages. Will only be made once"<<endl;
  ifstream vocabulary_file("vocabulary.bin", ios::in|ios::binary);
  string line, term;
  uint id, freq;
  vector<string> tokens;
  while(getline(vocabulary_file, line)) {
    boost::split(tokens, line, boost::is_any_of(","));
    term = tokens[0];
    id = stoi(tokens[1]);
    freq = stoi(tokens[2]);
    vocabulary_[term]=make_pair(id, freq);
  }
  ifstream pages_file("pagesIds.txt", ios::in|ios::binary);
  
  while(getline(pages_file, line)) {
    boost::split(tokens, line, boost::is_any_of(" "));
    id = stoi(tokens[0]);
    pages_[id] = tokens[1];
  }
}

void QueryProcessing::retrieve(string q) {
  //Split string into AND and OR
  vector<string> tokens;
  vector<string> terms;
  vector<string> operations;
  boost::split(tokens, q, boost::is_any_of(" "));
  bool last_item_is_term = false;
  for(uint it = 0; it<tokens.size(); ++it) {
    if(tokens[it]=="OR" || tokens[it]=="AND") {
      last_item_is_term = false;
      operations.push_back(tokens[it]);
    }
    //If there is no connection, assume AND
    else if(last_item_is_term){
      last_item_is_term = false;
      operations.push_back("AND");
    }
    else {
      last_item_is_term = true;
      terms.push_back(tokens[it]);
    }
  }
  set<uint> aux;
  bool use1 = true;
  set<uint> s1;
  set<uint> s2;
  uint to_retrieve = 2;
  if(terms.size() ==1){
    aux = retrieveForKeyword(terms[0]);
  }
  else {
    s1 = retrieveForKeyword(terms[0]);
    s2 = retrieveForKeyword(terms[1]);
    if(operations[0]=="AND")
      set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                       inserter(aux, aux.begin()));
    if(operations[0]=="OR")
      set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                inserter(aux, aux.begin()));
    use1 = false;
    for(unsigned it = 1; it<operations.size(); ++it){
      s1 = aux;
      aux.clear();
      s2 = retrieveForKeyword(terms[to_retrieve]);
      if(operations[it]=="AND")
        set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                         inserter(aux, aux.begin()));
      if(operations[it]=="OR")
        set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                  inserter(aux, aux.begin()));
      to_retrieve++;
    }
  }
  //Print output
  for(auto it = aux.begin(); it!=aux.end(); ++it){
    cout<<pages_[*it]<<endl;
  }
}

set<uint> QueryProcessing::retrieveForKeyword(string q){
  //Read 40 MB of the index.
  set<uint> ret;
//  uint* buff;
  //Word not in my vocabulary
  if(vocabulary_.find(q)==vocabulary_.end()){
    return ret;
  }
  cout<<vocabulary_.find(q)->second.first<<endl;
  uint wid = vocabulary_[q].first;
  vector<uint> buff(sizeof(uint)*9999999);
  FILE* index_file = fopen("index.bin", "rb");
  bool foundBeginning = false;
  bool foundEnding = false;
  uint counter =1;
  while(!foundEnding || !feof(index_file)){
    counter++;
    fread(&buff[0], sizeof(uint), 9999999, index_file);
    //Check if we found the word we want
    for(uint it = 0; it<buff.size(); it+=3){
      if(buff[it]==0){
        break;
      }
      if(buff[it]==wid){
        foundBeginning = true;
        ret.insert(buff[it+1]);
      }
      else if(foundBeginning){
        foundEnding = true;
        break;
      }
    }
  }
  buff.clear();
  return ret;
}