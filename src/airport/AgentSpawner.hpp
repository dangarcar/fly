#pragma once

#include <vector>
#include <random>
#include <ctime>

#include "../game/Types.h"

struct Agent {
    int source, target;
};

class AgentSpawner {
    static constexpr long NATIONAL_BOOST = 10;
    static constexpr long CAPITAL_BOOST = 20;

private:
    std::mt19937_64 gen;
    std::vector<long> populations;

public:
    AgentSpawner(): gen(time(nullptr)) {}

    int peopleToSpawn(size_t nCities) const {
        return (nCities + rand()%(DEFAULT_TICKS_PER_SECOND-1)) / DEFAULT_TICKS_PER_SECOND;
    }

    Agent spawn(const std::vector<City>& cities) {
        if(cities.size() < 2)
            return {-1, -1};

        std::vector<long> populations(cities.size());
        populations[0] = cities[0].population;
        for(int i=1; i<int(cities.size()); ++i) 
            populations[i] = populations[i-1] + glm::sqrt(cities[i].population / 1000.0f) * 1000;
        long x = gen() % populations.back();
        auto it = std::lower_bound(populations.begin(), populations.end(), x);

        Agent a;
        a.source = it - populations.begin();

        auto country = cities[a.source].country;
        for(int i=0; i<int(cities.size()); ++i) {
            if(cities[i].country == country) {
                long prev = i>0? populations[i-1]: 0;
                populations[i] = prev + 1000L * glm::log2(float(cities[i].population)) * long(NATIONAL_BOOST + cities[i].capital*CAPITAL_BOOST);
            }
        }

        do {
            long x = gen() % populations.back();
            auto it = std::lower_bound(populations.begin(), populations.end(), x);
            a.target = it - populations.begin();
        } while(a.source == a.target);

        return a;
    }
};