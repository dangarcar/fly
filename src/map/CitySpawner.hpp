#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "../game/Types.h"

#define AIRPORTS_DATA_FILE "./resources/airports.json"

class Camera;

class CitySpawner {
public:
    void load(const Camera& camera);

    std::optional<City> getRandomCity();
    void addCountry(std::string country) { possibleCountries.push_back(country); }

private:
    std::unordered_map<std::string, std::vector<City>> cities;
    std::unordered_map<std::string, int> currentCities;
    std::vector<std::string> possibleCountries;

};