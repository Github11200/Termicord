#include "database/database.h"

using namespace std;
using json = nlohmann::json;

Database::Database(string name) {
  this->NAME = name;

  // Try opening the database, if you can't, then create it
  if (!this->openDatabase())
    this->createDatabase();
}

void Database::createDatabase() {
  fstream database(this->PATH + this->NAME + ".json",
                   ios::out | ios::in | ios::trunc);

  if (!database.is_open()) {
    spdlog::error("Could not create a new database");
    return;
  }

  database << "{}";
  this->currentJsonData = json::parse("{}");
  database.close();

  spdlog::info("Successfully created the database \"{}\"", this->NAME);
}

bool Database::openDatabase() {
  fstream database(this->PATH + this->NAME + ".json");

  if (!database.is_open()) {
    spdlog::error("Could not open the database");
    return false;
  }

  database >> this->currentJsonData;
  spdlog::info("Successfully opened and loaded the database");
  return true;
}

bool Database::databaseExists() {
  ifstream file(this->PATH + this->NAME + ".json");
  if (file)
    return true;
  else
    return false;
}

optional<string> Database::searchDatabase(string key) {
  if (this->currentJsonData.contains(key))
    return this->currentJsonData[key].dump();
  return nullopt;
}

string Database::getAllJsonData() { return this->currentJsonData.dump(); }

bool Database::addEntry(string key, json jsonData) {
  // Only add the entry if it doesn't already exist
  if (!this->searchDatabase(key)) {
    this->currentJsonData[key] = jsonData;
    return true;
  }
  return false;
}

bool Database::updateEntry(string key, json newData) {
  // If the key doesn't exist then just return false since we can't update
  // anything
  optional<string> searchResult = this->searchDatabase(key);
  if (!searchResult)
    return false;
  this->currentJsonData[key] = newData;
  return true;
}

Database::~Database() {
  fstream database(this->PATH + this->NAME + ".json");

  if (!database.is_open()) {
    spdlog::error("Could not open the database so the latest data has not been "
                  "written to it");
    return;
  }

  // Write the latest data to the database
  database << this->currentJsonData.dump(2);
  spdlog::info("Successfully closed the database");
}
