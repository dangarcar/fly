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
            c.coord = {coord[0], coord[1]};
            c.proj = camera.coordsToProj(c.coord);
            c.capital = e["capital"].template get<bool>();
            c.country = k;

            cities[k].emplace_back(std::move(c));
        }
    }
}

std::optional<City> CitySpawner::getRandomCity() {
    std::optional<City> city;

    if(!pendingCities.empty()) {
        auto c = pendingCities.front();
        pendingCities.pop();
        return c;
    }

    if(possibleCountries.empty() || rand() % SPAWN_FREQUENCY != 0)
        return city;
    
    std::vector<int> populations(possibleCountries.size());
    for(size_t i=0; i<populations.size(); ++i)
        populations[i] = possibleCountries[i].population;
    std::discrete_distribution<int> distribution(populations.begin(), populations.end());

    auto countryIndex = distribution(generator);
    auto i = possibleCountries[countryIndex].currentCity;
    auto country = possibleCountries[countryIndex].name;
    if(i < cities[country].size()) {  
        city = cities[country][i];

        possibleCountries[countryIndex].population = city.value().population;
        possibleCountries[countryIndex].currentCity++;
    }

    return city;
}

void CitySpawner::addCountry(const std::string& country) { 
    possibleCountries.emplace_back(country, 1, cities[country][0].population);
    pendingCities.push(cities[country][0]);
}
