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


using namespace htmlcxx;
using namespace std;

const unsigned kMaxMemory = 40000000;


Indexer::Indexer(string input_directory,string input_collection_index){
  vocabulary_size_ = 0;
  number_of_documents = 0;
  number_of_indexed_documents_ = 0;
  number_of_runs_ = 0;
  space_occupied_by_vector_ = sizeof(vector<tuple<uInt, uInt, uInt> >);
  input_directory_  = input_directory;
  input_collection_index_ = input_collection_index;
  index_file_ = fopen("index.bin", "wb");
  RICPNS::CollectionReader * reader = new  RICPNS::CollectionReader(
      input_directory_,
      input_collection_index_);
  
  RICPNS::Document doc;
  
  doc.clear();
  
  string data, url;
  //avoiding reallocation when pushing_bag
  keyword_vector_.reserve(kMaxMemory/sizeof(tuple<uInt, uInt, uInt>));
  while(reader->getNextDocument(doc)) {
    number_of_documents++;
    space_occupied_by_vector_ = keyword_vector_.size()*sizeof(tuple<uInt, uInt, uInt>) + sizeof(vector<tuple<uInt, uInt, uInt>>);
    if(number_of_documents%10000 == 0){
      cout<<number_of_documents<<" indexed"<<endl;
      cout<<"Using "<<space_occupied_by_vector_<<"B of memory"<<endl;
    }
    //Memory is off-limit. Dump and clear everything. 1 extra document for safety
    if (space_occupied_by_vector_ >= kMaxMemory) {
      dumpIndex();
      cout<<"Run "<<number_of_runs_<<" finished. Dumping data."<<endl;
      vector<tuple<unsigned int, unsigned int, unsigned int> >().swap(keyword_vector_);
    }
    
    data = doc.getText();
    url = doc.getURL();
    Page reading_page = Page(data, url, number_of_documents);
    updateVocabulary(reading_page.keywords());
    addKeywordsToArray(reading_page.keywords(), reading_page.id());
    doc.clear();
  }
  dumpIndex();
  vector<tuple<unsigned int, unsigned int, unsigned int> >().swap(keyword_vector_);
  cout<<"Finished reading documents"<<endl;
  doc.clear();
  url.clear();
  delete reader;
  dumpVocabulary();
  Merge();
  
}

void Indexer::updateVocabulary(unordered_map<string, uInt> words){

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
//
//
void Indexer::addKeywordsToArray(const unordered_map<string, uInt> &k, uInt docid){
  for(auto it = k.begin(); it!=k.end(); ++it){
    uint keywordId = vocabulary_[it->first].first;
    keyword_vector_.push_back(make_tuple(keywordId, docid, it->second));
  }
}

//Tuples are written as <word_id, doc_id, frequency>
void Indexer::dumpIndex(){
  number_of_runs_++;
  cout<<"Dumping Index"<<endl;
  stringstream strstream;
  string nrun;
  strstream<<number_of_runs_;
  strstream>>nrun;
  //Sort keywords
  sort(keyword_vector_.begin(), keyword_vector_.end(), mySort());
  //Write keywords to run's file
  string filename = "index"+nrun+".txt";
  FILE* fp;
  fp = fopen(filename.c_str(), "wb");
  for (auto it = keyword_vector_.begin(); it!= keyword_vector_.end(); ++it){
    uInt wordId = get<0>(*it);
    uInt docId = get<1>(*it);
    uInt freq = get<2>(*it);
    fwrite(&wordId, sizeof(uInt), 1, fp);
    fwrite(&docId, sizeof(uInt), 1, fp);
    fwrite(&freq, sizeof(uInt), 1, fp);
  }
  cout<<"Closing file index"<<nrun<<".txt"<<endl;
  fclose(fp);
}

void Indexer::Merge(){
  //size of block in number of elements
  cout<<"External sorting"<<endl;
  uInt size_of_blocks = (kMaxMemory/(number_of_runs_+1))/(sizeof(uInt)+2*sizeof(uInt));
  vector<FILE*> runs;
  tuple<uInt, uInt, uInt, uInt> top_element;
  uInt wordid, freq;
  uInt docid;
  //Initial fill
  for (uInt  i =0; i<number_of_runs_+1; ++i){
    stringstream strstream;
    string nrun;
    strstream<<i;
    strstream>>nrun;
    string name_of_file = "index"+nrun+".txt";
       runs.push_back(fopen(name_of_file.c_str(), "rb"));
    
    //Read a block from this file
    for (uInt j = 0; j<size_of_blocks; ++j) {
      fread(&wordid, sizeof(uInt), 1, runs[i]);
      fread(&docid, sizeof(uInt), 1, runs[i]);
      fread(&freq, sizeof(uInt), 1, runs[i]);
      if(feof(runs[i]))
        break;
      heap_.push(make_tuple(wordid, docid, freq, i));
    }
  }
  //While the heap is not empty, remove the top element, write it to buffer and add one more.
  uInt removedFrom;
  while(!heap_.empty()) {
    top_element = heap_.top();
    removedFrom = get<3>(top_element);
    heap_.pop();
    output_buffer_.push_back(make_tuple(get<0>(top_element),get<1>(top_element), get<2>(top_element)));
    //CHECK IF BUFFER IS FULL
    if((output_buffer_.size()*sizeof(tuple<unsigned, unsigned, unsigned>)) >kMaxMemory){
      dumpBuffer();
    }
    
    //REINSERT ELEMENT INTO HEAP
    fread(&wordid, sizeof(uInt), 1, runs[removedFrom]);
    fread(&docid, sizeof(uInt), 1, runs[removedFrom]);
    fread(&freq, sizeof(uInt), 1, runs[removedFrom]);
  }
  dumpBuffer();
}
//
void Indexer::dumpBuffer() {
  size_t bufferSize = output_buffer_.size()*3*sizeof(unsigned);
  unsigned *writabble = (unsigned*) malloc(bufferSize);
  unsigned controller = 0;
  for(unsigned i = 0; i<output_buffer_.size(); ++i) {
    writabble[controller++] = get<0>(output_buffer_[i]);
    writabble[controller++] = get<1>(output_buffer_[i]);
    writabble[controller++] = get<2>(output_buffer_[i]);
  }
  fwrite(writabble, sizeof(uint), bufferSize , index_file_);
  free(writabble);
}
