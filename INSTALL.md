# Installation guide
## Step 1: `Download release of this repository`
  ```
  here will be link to latest release
  ```
## Step 2: `Preparation`
  ```
    1st - check this software to be installed:
      g++
      libcurl4-openssl-dev
      make
      binutils
      cmake
      libssl-dev
      libboost-system-dev
      libboost-iostreams-dev
  ```
## Step 3: `Compilation (Linux)`
  ```
    g++ main.cpp -o telegram_bot --std=c++11 -I/usr/local/include -lTgBot -lboost_system -lboost_iostreams -lssl -lcrypto -lpthread
  ```
## Step 4: `Starting bot (Linux)`
  ```
    ./telegram_bot
  ```
