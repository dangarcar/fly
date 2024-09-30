#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <optional>
#include <chrono>
#include <random>

#include "../game/Types.h"

class Camera;

class CitySpawner {
private:
    //The inverse of the probability of a city spawn
    static constexpr int SPAWN_FREQUENCY = 1; //TODO: must change it probably
    static constexpr auto AIRPORTS_DATA_FILE = "./resources/airports.json";

public:
    //Time is more random in MinGW than the random_device
    CitySpawner(): generator(std::chrono::system_clock::now().time_since_epoch().count()) {}

    void load(const Camera& camera);

    std::optional<City> getRandomCity();
    void addCountry(const std::string& country);

private:
    std::unordered_map<std::string, std::vector<City>> cities;
    std::mt19937_64 generator;
    
    struct UnlockableCityData {
        std::string name;
        size_t currentCity;
        int population;
    };

    std::queue<City> pendingCities;
    std::vector<UnlockableCityData> possibleCountries;

};