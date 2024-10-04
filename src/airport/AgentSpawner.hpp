#pragma once

#include <vector>
#include <random>
#include <ctime>

#include "../game/Types.h"

struct Agent {
    int source, target;
};

class AgentSpawner {
    static constexpr int NATIONAL_BOOST = 10;
    static constexpr int CAPITAL_BOOST = 20;

private:
    std::mt19937_64 gen;

public:
    AgentSpawner(): gen(time(nullptr)) {}

    int peopleToSpawn(size_t nCities) const {
        return (nCities + rand()%(DEFAULT_TICKS_PER_SECOND-1)) / DEFAULT_TICKS_PER_SECOND;
    }

    Agent spawn(const std::vector<City>& cities) {
        if(cities.size() < 2)
            return {-1, -1};
    
        std::vector<int> populations(cities.size());
        for(int i=0; i<int(cities.size()); ++i) 
            populations[i] = cities[i].population;
        std::discrete_distribution<int> distribution(populations.begin(), populations.end());

        Agent a;
        a.source = distribution(gen);

        auto country = cities[a.source].country;
        for(int i=0; i<int(cities.size()); ++i) {
            if(cities[i].country == country)
                populations[i] *= NATIONAL_BOOST + cities[i].capital*CAPITAL_BOOST;
        }
        distribution = {populations.begin(), populations.end()};

        do {
            a.target = distribution(gen);
        } while(a.source == a.target);

        return a;
    }
};