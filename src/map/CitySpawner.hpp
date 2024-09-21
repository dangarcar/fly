#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <optional>
#include <random>

#include "../game/Types.h"

class Camera;

class CitySpawner {
private:
    static constexpr int SPAWN_FREQUENCY = 30;
    static constexpr auto AIRPORTS_DATA_FILE = "./resources/airports.json";

public:
    CitySpawner(): generator(std::random_device{}()) {}

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