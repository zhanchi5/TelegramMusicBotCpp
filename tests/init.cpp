#include <catch.hpp>
#include "functions.hpp"

SCENARIO("test connection", ["getJSON"]){
  getJSON("http://ws.audioscrobbler.com/2.0/?method=chart.gettoptracks&api_key=34cbc3c5bcdc5f4bbabbd1037f67b870&page=1&format=json", "file.json");
  std::ifstream ifile("file.json", std::ifstream::ate | std::ifstream::binary);
  bool answer = true;
  auto a = ifile.tellg();
  if (a <= 0){
    answer = false;
  }
  else{
    answer;
  }
  REQUIRE(answer == true);
}


SCENARIO("test for parsing function", "[toJSON]"){
  toJSON(buf, "file.json", 1);
  std::ifstream ifile("file.json");
  json j = json::parse(ifile);
  bool answer = true;
  for (auto &track : j["tracks"]["track"]) {
   if (track != nullptr){
     answer;
   }
    else{
      answer = false;
    }
  }
  REQUIRE (answer == true);
}
