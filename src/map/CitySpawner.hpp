#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <optional>
#include <ctime>
#include <random>
#include <filesystem>

#include "../game/Types.h"
#include "../engine/Serializable.h"

class Camera;

struct UnlockableCityData {
    std::string name;
    size_t currentCity;
    int population;
};

struct CitySpawnerSave {
    std::vector<UnlockableCityData> possibleCountries;
};

class CitySpawner: Serializable<CitySpawnerSave> {
private:
    //The inverse of the probability of a city spawn
    static constexpr int SPAWN_FREQUENCY = 30; //TODO: must change it probably
    inline static const std::filesystem::path AIRPORTS_DATA_FILE = "./resources/airports.json";

public:
    //Time is more random in MinGW than the random_device
    CitySpawner(): generator(time(nullptr)) {}

    CitySpawnerSave serialize() const override;
    void deserialize(const CitySpawnerSave& save) override;

    std::vector<std::pair<std::string, int>> getCityIndices(const std::vector<City>& cities) const;
    std::vector<City> getCityVector(const std::vector<std::pair<std::string, int>>& indices) const;

    void load(const Camera& camera);

    std::optional<City> getRandomCity();
    void addCountry(const std::string& country);

private:
    std::unordered_map<std::string, std::vector<City>> cities;
    std::mt19937_64 generator;

    std::queue<City> pendingCities;
    std::vector<UnlockableCityData> possibleCountries;

};