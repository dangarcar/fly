#pragma once

#include <vector>
#include <unordered_map>
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
    std::vector<long> populations;
    std::unordered_map<std::string, std::vector<long>> boostedPopsByCountry;
    std::unordered_map<std::string, int> capitals;

public:
    AgentSpawner(): gen(time(nullptr)) {}

    int peopleToSpawn(size_t nCities) const {
        return (nCities + rand()%(DEFAULT_TICKS_PER_SECOND-1)) / DEFAULT_TICKS_PER_SECOND;
    }

    Agent spawn(const std::vector<City>& cities) {
        if(cities.size() < 2)
            return {-1, -1};
    
        //UPDATE POPULATIONS CACHE
        if(cities.size() != populations.size()) { 
            for(auto i=populations.size(); i<cities.size(); ++i) {
                auto cnt = cities[i].country;

                if(cities[i].capital) {
                    capitals[cnt] = i;
                }

                long prev = populations.empty()? 0 : populations.back();
                populations.push_back(prev + long(std::sqrt(cities[i].population / 1000.0f) * 1000.0f));

                if(!boostedPopsByCountry.contains(cnt))
                    boostedPopsByCountry[cnt] = {};

                for(auto& [k, v]: boostedPopsByCountry) {
                    for(auto j=v.size(); j<cities.size(); ++j) {
                        prev = v.empty()? 0 : v.back();
                        float boost = 1 + cities[j].capital * CAPITAL_BOOST;

                        if(cnt == k)
                            boost += NATIONAL_BOOST;

                        long a = boost * std::log2f(cities[j].population / 1000.0f) * 1000.0f;
                        v.push_back(prev + a);
                    }
                }
            }
        }

        long x = gen() % populations.back();
        auto it = std::lower_bound(populations.begin(), populations.end(), x);
        
        Agent a;
        a.source = it - populations.begin();

        auto country = cities[a.source].country;

        do {
            long x = gen() % boostedPopsByCountry[country].back();
            auto it = std::lower_bound(boostedPopsByCountry[country].begin(), boostedPopsByCountry[country].end(), x);
            assert(it != boostedPopsByCountry[country].end());

            a.target = it - boostedPopsByCountry[country].begin();
        } while(a.source == a.target);

        return a;
    }
};