#include "RequestManager.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <exception>
#include <iostream>
using nlohmann::json;

RequestManager::RequestManager() {
  std::string telegram_api_key =
      "377205379:AAFdYigWVjta_zFOSxVAYfnUV05yVcrmVik";
  _pBot = new TgBot::Bot(telegram_api_key);

}

RequestManager::~RequestManager() {}

int RequestManager::StartRequestHandler() {

  _pBot->getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
    std::string command, param;

    int ind = message->text.find(' ', 0);
    command = message->text.substr(0, ind);

    if (ind < message->text.length() - 1) {
      param = message->text.substr(ind + 1, message->text.length() - ind - 1);
    }
    // TODO: обработать param на предмет nullptr
      RequestHandle(message, command, param);
  });

  try {
    printf("Bot username: %s\n", _pBot->getApi().getMe()->username.c_str());

    TgBot::TgLongPoll longPoll(*_pBot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (std::exception &e) {
    printf("error: %s\n", e.what());
  }
}

int RequestManager::RequestHandle(TgBot::Message::Ptr pMessage,
                                  const std::string &command,
                                  const std::string &param) {


  int res = 0;

  if (command.empty()) {
    return -1;
  }

  std::cout << command << std::endl << param << std::endl;

    if (command == "/start") {
    res = StartCommand(pMessage);
  } else if (command == "/top") {
    res = TopCommand(pMessage);
  } else if (command == "/artist") {
    res = ArtistCommand(pMessage, param);
  } else if (command == "/video") {
    res = VideoCommand(pMessage, param);
  } else if (command == "/tracks") {
    res = TrackCommand(pMessage, param);
  }

  return res;
}

int RequestManager::StartCommand(TgBot::Message::Ptr pMessage) {
  _pBot->getApi().sendMessage(pMessage->chat->id, "Hey, Lets Rocket!");
  return 0;
}

int RequestManager::TopCommand(TgBot::Message::Ptr pMessage) {
  std::string resp =
      getJSON("http://ws.audioscrobbler.com/2.0/"
              "?method=chart.gettoptracks&api_key="
              "34cbc3c5bcdc5f4bbabbd1037f67b870&page=1&format=json");

  std::cout << "Parsing file" << std::endl;
  json j = json::parse(resp);

  std::cout << "Generating message" << std::endl;
  std::string response;

  for (auto &track : j["tracks"]["track"]) {
    response += track["name"].get<std::string>() + "\n" +
                track["url"].get<std::string>() + "\n";
  }

  _pBot->getApi().sendMessage(pMessage->chat->id, response);
  _pBot->getApi().sendMessage(pMessage->chat->id, "End of command operation");
  return 0;
}

int RequestManager::ArtistCommand(TgBot::Message::Ptr pMessage,
                                  const std::string &param) {

  if (param.empty()) {
    // TODO: обработать
    return -1;
  }

  std::string base_url =
      "http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&artist=";
  std::string api_format =
      "&api_key=34cbc3c5bcdc5f4bbabbd1037f67b870&format=json";
  std::string tmp = base_url + param + api_format;
  std::cout << tmp << std::endl;

  std::string resp = getJSON(tmp);
  json j = json::parse(resp);

  std::string text = j["artist"]["bio"]["summary"].get<std::string>();
  boost::replace_all(text, "<a href=\"", "\n");
  boost::replace_all(text, "\">Read more on Last.fm</a>", "");

  _pBot->getApi().sendMessage(pMessage->chat->id, text);
  _pBot->getApi().sendMessage(pMessage->chat->id, "End of command operation");

  return 0;
}

int RequestManager::VideoCommand(TgBot::Message::Ptr pMessage,
                                 const std::string &param) {
  if (param.empty()) {
    return -1;
  }

  return 0;
}

int RequestManager::TrackCommand(TgBot::Message::Ptr pMessage,
                                 const std::string &param) {
  if (param.empty()) {
    return -1;
  }

  return 0;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string RequestManager::getJSON(const std::string &url) {
  CURL *curl = nullptr;    // указатель на объект cURL
  curl = curl_easy_init(); // запуск "легкого" хэндлера
  CURLcode res;            // объект класса cURLcode
  std::string response;

  if (curl) {
    struct curl_slist *chunk = nullptr; // список на ноль
    chunk = curl_slist_append(
        chunk,
        "User-Agent: Firefox"); // кастомный хэдер USERAGENT в список хэдеров
    res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER,
                           chunk); // список отправляется в хэдер запроса
    curl_easy_setopt(curl, CURLOPT_URL,
                     url.c_str()); // ссылка, по которой обращаемся
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  return response;
}
