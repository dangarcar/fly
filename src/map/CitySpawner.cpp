#include "CitySpawner.hpp"

#include "../../include/json.hpp"
#include <fstream>

#include "../game/Camera.hpp"

void CitySpawner::load(const Camera& camera) {
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
