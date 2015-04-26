//
//  Indexer.h
//  
//
//  Created by Arthur Câmara on 19/3/15.
//
//

#include "Indexer.h"
#include "Page.h"
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
  space_occupied_by_vector_ = sizeof(vector<tuple<uint, uint, uint> >);
  input_directory_  = input_directory;
  input_collection_index_ = input_collection_index;
  string data, url;
  RICPNS::Document doc;
  
  if(merge) {
    number_of_runs_ = 69;
    Merge();
    return;
  }
  
  RICPNS::CollectionReader * reader = new  RICPNS::CollectionReader(
      input_directory_,
      input_collection_index_);

  doc.clear();
  
  
  //avoiding reallocation when inserting new stuff
  keyword_vector_.reserve(kMaxMemory/sizeof(tuple<uInt, uInt, uInt>));
  
  while(reader->getNextDocument(doc)) {
    
    number_of_documents_++;
    space_occupied_by_vector_ = keyword_vector_.size()*sizeof(tuple<uInt, uInt, uInt>) + sizeof(vector<tuple<uInt, uInt, uInt> >);
    
    if(number_of_documents_%10000 == 0) {
      cout<<number_of_documents_<<" indexed"<<endl;
      cout<<"Using "<<space_occupied_by_vector_<<"B of memory"<<endl;
    }
    
    //Memory is full. Dump and clear everything.
    if (space_occupied_by_vector_ >= kMaxMemory) {
      dumpIndex();
      cout<<"Run "<<number_of_runs_<<" finished. Dumping data."<<endl;
    }
    data = doc.getText();
    url = doc.getURL();
    
    pagesIds_[number_of_documents_]=url;
    
    Page reading_page = Page(data, url, number_of_documents_);
    updateVocabulary(reading_page.keywords());
    addKeywordsToKeywordVector(reading_page.keywords(), reading_page.id());
    doc.clear();
  }
  
  dumpIndex();
//  vector<tuple<unsigned int, unsigned int, unsigned int> >().swap(keyword_vector_);
  cout<<"Finished reading documents"<<endl;
  doc.clear();
  url.clear();
  delete reader;
  dumpVocabulary();
  dumpPages();
  Merge();
}

void Indexer::updateVocabulary(unordered_map<string, uint> words){

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
void Indexer::dumpVocabulary(){
  cout<<"Dumping Vocabulary"<<endl;
  cout<<"Vocabulary Size: "<<vocabulary_.size()<<endl;
  vocabulary_file_ = fopen("vocabulary.bin", "wb");
  string wordid, frequency;
  stringstream strstream;
  string str="";
  
  for (auto it = vocabulary_.begin(); it!= vocabulary_.end(); ++it) {
    strstream<<it->second.first;
    strstream>>wordid;
    strstream.clear();
    strstream<<it->second.second;
    strstream>>frequency;
    strstream.clear();
    str += it->first + "," + wordid +"," + frequency+"\n";
  }
  fwrite(str.c_str(), sizeof(char), str.length(), vocabulary_file_);
  fclose(vocabulary_file_);
}


void Indexer::addKeywordsToKeywordVector(const unordered_map<string, uInt> &k, uInt docid){
  for(auto it = k.begin(); it!=k.end(); ++it){
    uint keywordId = vocabulary_[it->first].first;
    tuple<uint, uint, uint> aux =  make_tuple(1, 1, 4);
    tuple<uint, uint, uint> aux2= make_tuple(keywordId, docid, it->second);
    keyword_vector_.push_back(aux2);
  }
}

//Tuples are written as <word_id, doc_id, frequency>
void Indexer::dumpIndex(){
  cout<<"Dumping Index"<<endl;
  stringstream strstream;
  string nrun;
  strstream<<number_of_runs_;
  strstream>>nrun;
  uint buffer[3];

  //Sort keywords
  sort(keyword_vector_.begin(), keyword_vector_.end(), mySort());
  
  //Write keywords to run's file
  string filename = "index"+nrun+".txt";
  FILE* fp;
  fp = fopen(filename.c_str(), "wb");
  for (auto it = keyword_vector_.begin(); it!= keyword_vector_.end(); ++it){
    buffer[0] = get<0>(*it);
    buffer[1] = get<1>(*it);
    buffer[2] = get<2>(*it);
    fwrite(buffer, sizeof(uint), 3, fp);
  }
  vector<tuple<uint, uint, uint> >().swap(keyword_vector_);
  fclose(fp);
  number_of_runs_++;
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

void Indexer::Merge(){
  cout<<"External sorting with "<<number_of_runs_<<" runs"<<endl;
  
  uint midbuff[3];
  memset(midbuff, 3, sizeof(uint));

  tuple<uInt, uInt, uInt, uInt> top_element;
  stringstream strstream;
  string nrun;
  uInt removed_from;
  string name_of_file;

  
  //size of each block, given in number of uints to be read
  uint size_of_blocks = kMaxMemory/((number_of_runs_+1)*sizeof(uint));
  
  buff_.reserve(9999999);
  index_file_ = fopen("index.bin", "wb");

  //Initial heap fill
  vector<shared_ptr<ifstream> > runs2;
  vector<uint>mdb(3);
  for (uInt  i = 0; i<number_of_runs_+1; ++i){
    ostringstream filename;
    filename<<"index"<<i<<".txt";
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
//    cout<<get<0>(top_element)<<" "<<get<1>(top_element)<<" "<<get<2>(top_element)<<" "<<get<3>(top_element)<<endl;
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
      if(number_of_writes_on_merge>69){
        cout<<removed_from<<endl;
      }
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
