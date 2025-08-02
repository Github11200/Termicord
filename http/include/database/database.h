#ifndef DATABASE_H
#define DATABASE_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <spdlog/spdlog.h>

class Database {
private:
  nlohmann::json currentJsonData;
  std::string PATH = "databases/";
  std::string NAME;

public:
  Database(std::string name);
  ~Database();

  void createDatabase();
  bool openDatabase();
  bool databaseExists();
  std::optional<std::string> searchDatabase(std::string key);
  std::string getAllJsonData();
  bool addEntry(std::string key, nlohmann::json jsonData);
  bool updateEntry(std::string key, nlohmann::json newData);
};

#endif
