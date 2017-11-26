#include <signal.h>
#include <stdio.h>
#include <exception>
#include <tgbot/tgbot.h>
#include <iostream>
#include <curl/curl.h>
#include "json.hpp"
#include <cstring>
#include <fstream>
#include <vector>


using nlohmann::json;

int fileSize;

static void getJSON(char* url, char* filename) {
        CURL  *curl;// указатель на объект cURL
        curl = curl_easy_init(); // запуск "легкого" хэндлера
        CURLcode res; // объект класса cURLcode

        if (curl) {
                struct curl_slist *chunk = NULL; // список на ноль
                chunk = curl_slist_append(chunk, "User-Agent: David"); // кастомный хэдер USERAGENT в список хэдеров
                res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk); // список отправляется в хэдер запроса
                curl_easy_setopt(curl, CURLOPT_URL, url); // ссылка, по которой обращаемся
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                FILE *pF;
                pF = fopen(filename, "wb");
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, pF); // команда на копирование данных в файл

                res = curl_easy_perform(curl); // выполнение запросов

                if (res != CURLE_OK) // проверка
                        fprintf(stderr, "Could not download file: %s\n",
                                curl_easy_strerror(res));
                else printf("SUCCESS: File has been dowloaded\n");

                rewind(pF);
                fseek(pF, 0, SEEK_END);
                fileSize = ftell(pF); // получение размера файла
                curl_easy_cleanup(curl);
                fclose(pF);
        }
}

std::string formattedJson(char *json, unsigned int files_or_commits)
{
        std::string pretty;

        if (json == NULL || strlen(json) == 0)
        {
                return pretty;
        }

        std::string str = std::string(json);
        bool quoted = false;
        bool escaped = false;
        std::string INDENT = "    ";
        int indent = 0;
        int length = (int)str.length();
        int i;

        for (i = 0; i < length; i++)
        {
                char ch = str[i];

                switch (ch)
                {
                case '{':
                case '[':
                        pretty += ch;

                        if (!quoted)
                        {
                                pretty += "\n";

                                if (!(str[i + 1] == '}' || str[i + 1] == ']'))
                                {
                                        ++indent;

                                        for (int j = 0; j < indent; j++)
                                        {
                                                pretty += INDENT;
                                        }
                                }
                        }

                        break;

                case '}':
                case ']':
                        if (!quoted)
                        {
                                if ((i > 0) && (!(str[i - 1] == '{' || str[i - 1] == '[')))
                                {
                                        pretty += "\n";

                                        --indent;

                                        for (int j = 0; j < indent; j++)
                                        {
                                                pretty += INDENT;
                                        }
                                }
                                else if ((i > 0) && ((str[i - 1] == '[' && ch == ']') || (str[i - 1] == '{' && ch == '}')))
                                {
                                        for (int j = 0; j < indent; j++)
                                        {
                                                pretty += INDENT;
                                        }
                                }
                        }

                        pretty += ch;

                        break;

                case '"':
                        pretty += ch;
                        escaped = false;

                        if (i > 0 && str[i - 1] == '\\')
                        {
                                escaped = !escaped;
                        }

                        if (!escaped)
                        {
                                quoted = !quoted;
                        }

                        break;

                case ',':
                        pretty += ch;

                        if (!quoted)
                        {
                                pretty += "\n";

                                for (int j = 0; j < indent; j++)
                                {
                                        pretty += INDENT;
                                }
                        }

                        break;

                case ':':
                        pretty += ch;

                        if (!quoted)
                        {
                                pretty += " ";
                        }

                        break;
                default:
                        pretty += ch;

                        break;
                }

        }
        return pretty;
}

void toJSON(json& j, char* filename, unsigned int files_or_commits) {
        std::ifstream fromFile(filename);
        char* line = new char[fileSize];
        while (!fromFile.eof()) {
                for (unsigned int i = 0; i <= fileSize; ++i) {
                        fromFile >> line[i];
                }
        }
        fromFile.close();

        std::ofstream toFile(filename);
        toFile << formattedJson(line, files_or_commits);
        toFile.close();

        std::ifstream finFile(filename);
        j << finFile;

        finFile.close();
}

using namespace std;
using namespace TgBot;

bool sigintGot = false;




int main() {
        Bot bot("377205379:AAEmGRAfTEBASf1tE7pevGok0oYLJENUa8Y");
        /*bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
           bot.getApi().sendMessage(message->chat->id, "Hi!");
           });
           bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
           printf("User wrote %s\n", message->text.c_str());
           if (StringTools::startsWith(message->text, "/start")) {
            return;
           }
           bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
           });*/
        bot.getEvents().onCommand("random", [&bot](Message::Ptr message){
          /**/
        } );
        bot.getEvents().onCommand("top", [&bot](Message::Ptr message){
                CURL* curl = curl_easy_init();
                json buf;
                CURLcode res;
                if (curl) {
                        struct curl_slist *chunk = NULL;
                        chunk = curl_slist_append(chunk, "User-Agent: Chrome");
                        std::vector<std::string> v;
                        res = curl_easy_perform(curl);
                        curl_easy_cleanup(curl);
                        getJSON("http://ws.audioscrobbler.com/2.0/?method=chart.gettoptracks&api_key=34cbc3c5bcdc5f4bbabbd1037f67b870&page=1&format=json", "file.json");
                        toJSON(buf, "file.json", 1);
                        std::ifstream ifile("file.json");
                        json j = json::parse(ifile);
                        unsigned int i = 0;
                        for (auto &track : j["tracks"]["track"]) {
                          v.push_back(track["name"]);
                          v[i] += " ";
                          v[i] += track["url"];
                          //std::cout << v[i] << std::endl;
                          i++;
                        }
                        std::string la = v[0];
                        v[0].clear();
                        for (auto &i : v){
                          v[0] += i;
                          v[0] += "\n";
                        }
                        v[0] += la;
                        v.resize(1);
                        bot.getApi().sendMessage(message->chat->id, v[0]);
                      }


        });
        /*
        bot.getEvents().onCommand("genre", [&bot](Message::Ptr message){

        })*/


        signal(SIGINT, [](int s) {
                printf("SIGINT got");
                sigintGot = true;
        });
        try {
                printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());

                TgLongPoll longPoll(bot);
                while (!sigintGot) {
                        printf("Long poll started\n");
                        longPoll.start();
                }
        } catch (exception& e) {
                printf("error: %s\n", e.what());
        }
        return 0;
}
