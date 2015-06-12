//
//  Indexer.cpp
//  
//
//  Created by Arthur Câmara on 19/3/15.
//
//

#include "Indexer.h"
#include "Page.h"
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>
#include <gumbo.h>
#include <fstream>


using namespace std;

const unsigned kMaxMemory = 40000000;


Indexer::Indexer(string input_directory,
                 string input_collection_index,
                 bool merge){
  
  vocabulary_size_ = 0;
  number_of_documents_ = 0;
  number_of_runs_ = 0;
  number_of_writes_on_merge = 0;
  number_of_anchor_files_ = 0;
  space_occupied_by_vector_ = sizeof(vector<tuple<uint, uint, uint> >);
  space_occupied_by_anchor_texts_ = sizeof(unordered_map<string, vector<string> >);
  
  input_directory_  = input_directory;
  input_collection_index_ = input_collection_index;
  string data, url;
  RICPNS::Document doc;
  
  //Hard-coded for tests.
  if(merge) {
    number_of_runs_ = 70;
    Merge("index.bin", "index", number_of_runs_);
//    Merge("anchor.bin", "anchor", 3);
    return;
  }
  
  //Read all URLS in the index and assign IDs for all of them.
  ifstream indexfile(input_directory_+"/"+input_collection_index_);
  cout<<input_directory_+"/"+input_collection_index_<<endl;
  string u, file;
  int a, b,c;
  unsigned n_pages = 1;
  while(indexfile >> u >> file >> a >> b >>c){
    pagesUri_[u] = n_pages;
    pagesIds_[n_pages++] = u;
  }
  indexfile.close();
  
  RICPNS::CollectionReader * reader = new  RICPNS::CollectionReader(
      input_directory_,
      input_collection_index_);

  doc.clear();
  
  
  //avoiding reallocation when inserting new stuff
  keyword_vector_.reserve(kMaxMemory/sizeof(tuple<uInt, uInt, uInt>));
  while(reader->getNextDocument(doc)) {
    
    number_of_documents_++;
    space_occupied_by_vector_ = keyword_vector_.size()*sizeof(tuple<uInt, uInt, uInt>) + sizeof(vector<tuple<uInt, uInt, uInt> >);
    space_occupied_by_anchor_texts_ = anchor_texts_.size()*sizeof(tuple<uint, uint, uint>) + sizeof(vector<tuple<uInt, uInt, uInt> >);
    if(number_of_documents_%10000 == 0) {
      cout<<number_of_documents_<<" indexed"<<endl;
    }
    
    //Memory is full. Dump and clear everything.
    if (space_occupied_by_vector_ >= kMaxMemory) {
      dumpIndex();
      cout<<"Run "<<number_of_runs_<<" finished. Dumping data."<<endl;
    }
    if(space_occupied_by_anchor_texts_>= kMaxMemory){
      dumpAnchor();
    }
    data = doc.getText();
    url = doc.getURL();
    
    //Parse page
    Page reading_page = Page(data, url, pagesUri_[url]);
    updateVocabulary(reading_page.keywords());
    updateAnchorTextsAndLinks(reading_page.anchor_texts_, reading_page.id());
    addKeywordsToKeywordVector(reading_page.keywords(), reading_page.id());
    doc.clear();
  }
  
  dumpIndex();
  cout<<"Finished reading documents"<<endl;
  doc.clear();
  url.clear();
  delete reader;
  dumpVocabulary();
  dumpPages();
  dumpAnchor();
  dumpLinks();
  Merge("index.bin", "index", number_of_runs_);
  Merge("anchor.bin", "anchor", number_of_anchor_files_);
}



/***********************UPDATERS*****************************/

void Indexer::updateVocabulary(unordered_map<string, uint> words){

  for(auto it = words.begin(); it!= words.end(); ++it) {
    if(vocabulary_.find(it->first)!=vocabulary_.end())
      vocabulary_[it->first].second+= 1;
    else {
      vocabulary_size_++;
      vocabulary_.insert(make_pair(it->first, make_pair(vocabulary_size_, 1)));
    }
  }
}

void Indexer::addKeywordsToKeywordVector(const unordered_map<string, uInt> &k, uInt docid){
  for(auto it = k.begin(); it!=k.end(); ++it){
    uint keywordId = vocabulary_[it->first].first;
    tuple<uint, uint, uint> aux2= make_tuple(keywordId, docid, it->second);
    keyword_vector_.push_back(aux2);
  }
}

void Indexer::updateAnchorTextsAndLinks(const unordered_map<string, unordered_map<string, uint>> & a, uint id) {
  for (auto it : a) {
    if(pagesUri_.find(it.first)==pagesUri_.end())
      continue;
    unsigned to = pagesUri_[it.first];
    //Adding to links set
    page_links_.emplace_back(id, to);
    //it.second is a vector of words for the url in it.first
    for (auto word : it.second) {
      if(vocabulary_.find(word.first)!=vocabulary_.end())
        vocabulary_[word.first].second++;
      else{
        vocabulary_size_++;
        vocabulary_.insert(make_pair(word.first, make_pair(vocabulary_size_, word.second)));
      }
      int wid = vocabulary_[word.first].first;
      tuple<uint, uint, uint> aux = make_tuple(wid, id, word.second);
      anchor_texts_.push_back(aux);
    }
  }
}



/***********************DUMPERS*****************************/

void Indexer::dumpVocabulary(){
  cout<<"Dumping Vocabulary"<<endl;
  cout<<"Vocabulary Size: "<<vocabulary_.size()<<endl;
  vocabulary_file_ = fopen("vocabulary.bin", "wb");
  string wordid, frequency;
  stringstream strstream;
  string str="";
  
  for (auto it = vocabulary_.begin(); it!= vocabulary_.end(); ++it) {
    float idf = log2(number_of_documents_/((float)it->second.second));
    strstream<<it->second.first;
    strstream>>wordid;
    strstream.clear();
    strstream<<idf;
    strstream>>frequency;
    strstream.clear();
    str += it->first + "," + wordid +"," + frequency+"\n";
  }
  fwrite(str.c_str(), sizeof(char), str.length(), vocabulary_file_);
  fclose(vocabulary_file_);
}


void Indexer::dumpIndex(){
  stringstream strstream;
  string nrun;
  strstream<<number_of_runs_;
  strstream>>nrun;

  sort(keyword_vector_.begin(), keyword_vector_.end(), mySort());
  
  string filename = "index"+nrun+".txt";
  FILE* fp = fopen(filename.c_str(), "wb");
  uint buffer[3];
  for (auto it : keyword_vector_){
    buffer[0] = get<0>(it);
    buffer[1] = get<1>(it);
    buffer[2] = get<2>(it);
    fwrite(buffer, sizeof(uint), 3, fp);
  }
  vector<tuple<uint, uint, uint>>().swap(keyword_vector_);
  fclose(fp);
  number_of_runs_++;
}

void Indexer::dumpAnchor() {
  string nrum = to_string(number_of_anchor_files_);
  cout<<"Dumping Anchor Text Data Number "<<nrum<<endl;
  sort(anchor_texts_.begin(), anchor_texts_.end(), mySort());
  
  string filename = "anchor"+nrum+".txt";
  FILE* fp = fopen(filename.c_str(), "wb");
  uint buffer[3];
  for (auto it : anchor_texts_){
    buffer[0] = get<0>(it);
    buffer[1] = get<1>(it);
    buffer[2] = get<2>(it);
    fwrite(buffer, sizeof(uint), 3, fp);
  }
  vector<tuple<uint, uint, uint>>().swap(anchor_texts_);
  fclose(fp);
  number_of_anchor_files_++;
}


void Indexer::dumpPages(){
  FILE* fp = fopen("pagesIds.txt", "wb");
  string str="";

  for(auto it = pagesIds_.begin(); it!=pagesIds_.end(); ++it) {
    str += to_string(it->first)+" "+it->second+"\n";
  }
  fwrite(str.data(), str.length(), sizeof(char), fp);
  fclose(fp);
}


void Indexer::dumpLinks(){
  uint buffer[2];
  FILE* fp = fopen("links.txt", "wb");
  for (auto e : page_links_) {
    buffer[0]=e.first;
    buffer[1]=e.second;
    fwrite(buffer, 3, sizeof(uint), fp);
  }
  vector<pair<uint, uint>>().swap(page_links_);
  
  fclose(fp);
}


void Indexer::Merge(string outfile, string inprefix, unsigned nruns){

  cout<<"External sorting with "<<nruns<<" runs"<<endl;
  
  uint midbuff[3];
  memset(midbuff, 3, sizeof(uint));

  tuple<uInt, uInt, uInt, uInt> top_element;
  stringstream strstream;
  string nrun;
  uInt removed_from;
  string name_of_file;

  uint size_of_blocks = kMaxMemory/((nruns+1)*sizeof(uint));
  
  buff_.reserve(9999999);
  index_file_ = fopen(outfile.c_str(), "wb");

  //Initial heap fill
  vector<shared_ptr<ifstream> > runs2;
  vector<uint>mdb(3);
  for (uInt  i = 0; i<nruns; ++i){
    ostringstream filename;
    filename<<inprefix<<i<<".txt";
    unique_ptr<ifstream> file(new ifstream(filename.str(), ios::binary|ios::in));
    runs2.push_back(move(file));
    for(uint it = 0; it<size_of_blocks; it+=3) {
      runs2[i]->read(reinterpret_cast<char*>(midbuff), sizeof(uint)*3);
      heap_.push(make_tuple(midbuff[0],
                midbuff[1],
                midbuff[2], i));
    }
  }
  /*While the heap is not empty, remove the top element, write it to
   buffer and read one From the same file*/
  
  while(!heap_.empty()) {
    top_element = heap_.top();
    midbuff[0]  = get<0>(top_element);
    midbuff[1]  = get<1>(top_element);
    midbuff[2]  = get<2>(top_element);
    removed_from = get<3>(top_element);
    buff_.push_back(midbuff[0]);
    buff_.push_back(midbuff[1]);
    buff_.push_back(midbuff[2]);
    heap_.pop();
    //Check if buffer is full
    if(buff_.size()>= 9999999) {
      dumpBuffer();
      buff_.clear();
    }
    //REINSERT ELEMENT INTO HEAP from the same file
    if(!runs2[removed_from]->eof() && !runs2[removed_from]->bad()) {
      runs2[removed_from]->read(reinterpret_cast<char*>(midbuff), sizeof(uint)*3);
      heap_.push(make_tuple(midbuff[0], midbuff[1], midbuff[2], removed_from));
    }
  }
  dumpBuffer();
  vector<uint>().swap(buff_);
  fclose(index_file_);
}


void Indexer::dumpBuffer() {
  number_of_writes_on_merge++;
  cout<<"Dumping Buffer ["<<number_of_writes_on_merge<<"]"<<endl;
  fwrite(&buff_[0], sizeof(vector<uint>::value_type), buff_.size(), index_file_);
}
