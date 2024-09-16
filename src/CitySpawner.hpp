#pragma once

#include "../include/json.hpp"

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "PlayerCamera.hpp"

#define AIRPORTS_DATA_FILE "./resources/airports.json"

struct City {
    std::string name;
    int population;
    glm::vec2 proj;
    bool capital;
};

class CitySpawner {
public:
    CitySpawner() = default;

    void load(const PlayerCamera& camera);

    std::optional<City> getRandomCity();
    void addCountry(std::string country) { possibleCountries.push_back(country); }

private:
    std::unordered_map<std::string, std::vector<City>> cities;
    std::unordered_map<std::string, int> currentCities;
    std::vector<std::string> possibleCountries;

};

void CitySpawner::load(const PlayerCamera& camera) {
    using json = nlohmann::json;

    std::ifstream airportFile(AIRPORTS_DATA_FILE);
    json airportData = json::parse(airportFile);

    for(auto& [k, v]: airportData.items()) {
        cities[k] = {};
        for(auto& e: v) {
            City c;
            c.name = e["name"].template get<std::string>();
            c.population = e["population"].template get<int>();
            auto coord = e["coords"].template get<std::vector<float>>();
            c.proj = camera.coordsToProj({coord[0], coord[1]});
            c.capital = e["capital"].template get<bool>();

            cities[k].push_back(c);
        }
    }
}

std::optional<City> CitySpawner::getRandomCity() {
    std::optional<City> city;

    if(possibleCountries.empty())
        return city;
    
    int tries = 0;
    do {
        int i = rand() % possibleCountries.size();
        auto country = possibleCountries[i];

        if(currentCities[country] < cities[country].size()) {
            city = cities[country][currentCities[country]];
            currentCities[country]++;
        }
    } while(!city.has_value() && tries++ < 10);

    return city;
}
