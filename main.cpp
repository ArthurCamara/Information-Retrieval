//
//  main.cpp
//  indexer
//
//  Created by Arthur Câmara on 9/3/15.
//  Copyright (c) 2015 DCC UFMG. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <fstream>
#include "src/indexer/Indexer.h"
#include "src/QueryProcessing/QueryProcessing.h"
#include "Ranking/page_rank.h"
#include "www/server_http.hpp"
#include "www/client_http.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;



int main(int argc, const char * argv[]) {
  std::ios::sync_with_stdio(false);
  string input_collection_index;
  string input_directory;
  string query;
  bool is_retrieval = false;
  bool is_merge = false;
  bool is_indexer = false;
  
  //Arguments parsing
  for(unsigned i = 1; i < argc; ++i) {
    string param  = argv[i];
    //index file name
    if(param == "--index" || param == "-i") {
      is_indexer = true;
      i++;
      input_collection_index = argv[i];
    }
    //index file path
    else if (param == "--directory" || param == "-d") {
      i++;
      input_directory = argv[i];
    }
    else if (param == "--retrieval" || param == "-r") {
      is_retrieval = true;
    }
    else if (param == "--merge" || param == "-m"){
      is_merge = true;
    }
    else if (param == "--retrieval" || param == "--r"){
      is_retrieval = true;
    }
  }
  if(is_merge){
    Indexer indexer(input_directory, input_collection_index, is_merge);
    return 0;
  }
  else if(is_retrieval){
    QueryProcessing q;
    while (getline(cin, query)){
      cout<<"Lookout for query "<<query<<endl;
      q.retrieve(query);
    }
    return 0;
  }
  else if(is_indexer){
    Indexer indexer(input_directory, input_collection_index);
    return 0;
  }
  //Begin server
  
  cout<<"Initializing Server..."<<endl;
  string indexFileName = "index.bin";
  string anchorFileName = "anchor.bin";
  string linksFileName = "links.txt";
//  QueryProcessing q;
  PageRank pr(linksFileName);
  
  HttpServer server(8080, 4);
  cout<<"Waiting for requests"<<endl;
  server.resource["^/search$"]["POST"]=[](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
    try {
      //Retrieve string from istream (request->content)
      ptree pt;
      read_json(request->content, pt);
      
//      bool vectorModel = pt.get<bool>("vms");
//      bool anchorText = pt.get<bool>("at");
//      bool pageRank = pt.get<bool>("pr");
//      int page = pt.get<int>("page");
      string query = pt.get<string>("query");
      
//      string result = search(query, vectorModel, pageRank, anchorText, page);
//      result = minify(result);
      string result = "bla";
      response << "HTTP/1.1 200 OK\r\nContent-Length: " << result.length() << "\r\nAccess-Control-Allow-Origin: *" << "\r\nContent-Type: \"application/json\"" << "\r\n\r\n" << result;
    }
    catch(std::exception& e) {
      response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
    }
  };
  
  

  return 0;
}
