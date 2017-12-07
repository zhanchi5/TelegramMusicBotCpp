#ifndef _REQUEST_MANAGER_HPP_
#define _REQUEST_MANAGER_HPP_

#include "json.hpp"
#include <curl/curl.h>
#include <string>
#include <tgbot/tgbot.h>

class RequestManager {

private:
  TgBot::Bot *_pBot;
  

  int RequestHandle(TgBot::Message::Ptr pMessage,
                    const std::string &command, const std::string &param);

  int TopCommand(TgBot::Message::Ptr pMessage);
  int ArtistCommand(TgBot::Message::Ptr pMessage,
                    const std::string &param);
  int VideoCommand(TgBot::Message::Ptr pMessage,
                   const std::string &param);
  int TrackCommand(TgBot::Message::Ptr pMessage,
                   const std::string &param);

  std::string getJSON(const std::string &url);

public:
  RequestManager();
  ~RequestManager();

  int StartRequestHandler();
};

#endif
