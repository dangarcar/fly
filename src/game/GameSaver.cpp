#include "GameSaver.hpp"

#include <fstream>
#include "../../include/json.hpp"

using nlohmann::json;

std::vector<GameSaver::Save> GameSaver::getSaveNames() {
    std::ifstream saveFile(SAVE_DIR / "worlds.json");
    auto data = json::parse(saveFile);

    std::vector<Save> saves;
    auto a = data["saves"];
    for(auto& v: a) {
        Save save;
        save.progress = v["progress"].template get<float>();
        save.file = v["file"].template get<std::string>();
        
        save.lastPlayed = v["lastPlayed"].template get<std::time_t>();

        saves.push_back(save);
    }

    return saves;
}

GameSave GameSaver::getSave(int saveIndex) { //TODO:
    assert(saveIndex < MAX_SAVES);
    auto saves = getSaveNames();
    Save s = saves[saveIndex];
    
    std::ifstream saveFile(SAVE_DIR / s.file);
    GameSave game;
    json j = json::parse(saveFile);
    game.currentTick = j["currentTick"];
    
    game.camera.zoom = j["zoom"];
    game.camera.pos.x = j["pos"][0];
    game.camera.pos.y = j["pos"][1];
    
    game.player.cash = j["cash"];
    game.player.difficulty = j["difficulty"];
    game.player.fastForwardMultiplier = j["fastForwardMultiplier"];
    game.player.stats.routes = j["stats"]["routes"];
    game.player.stats.planes = j["stats"]["planes"];
    game.player.stats.flights = j["stats"]["flights"];
    game.player.stats.passengersTotal = j["stats"]["passengersTotal"];
    game.player.stats.passengersArrived = j["stats"]["passengersArrived"];
    game.player.stats.countries = j["stats"]["countries"];
    game.player.stats.airports = j["stats"]["airports"];
    game.player.stats.population = j["stats"]["population"];
    game.player.stats.moneySpent = j["stats"]["moneySpent"];
    game.player.stats.moneyEarned = j["stats"]["moneyEarned"];

    for(const auto& k: j["airports"]) {
        air::AirportData a;
        a.level = k["level"];
        a.radius = k["radius"];
        a.waiting = k["waiting"].template get<std::vector<int>>();
        a.routeIndexes = k["routeIndexes"].template get<std::vector<int>>();

        game.airport.airports.push_back(a);
    }

    for(const auto& k: j["routes"]) {
        air::Route r(k["a"], k["b"]);
        r.level = k["level"];
        r.lenght = k["lenght"];
        r.lastTakeoffA = k["lastTakeoffA"];
        r.lastTakeoffB = k["lastTakeoffB"];

        for(const auto& l: k["planes"]) {
            air::Plane p;
            p.t = l["t"];
            p.speed = l["speed"];
            p.pass = l["pass"].template get<std::vector<int>>();
            r.planes.push_back(p);
        }
        game.airport.routes.push_back(r);
    }
    game.citiesIndexes = j["citiesIndexes"];
    game.airport.networkAdjList = j["networkAdjList"];
    game.airport.routeGrids = j["routeGrids"];

    for(const auto& k: j["possibleCountries"]) {
        UnlockableCityData u;
        u.name = k["name"];
        u.currentCity = k["currentCity"];
        u.population = k["population"];
        game.map.possibleCountries.push_back(u);
    }

    return game;
}

void GameSaver::save(const GameSave& game, int number) { //TODO:
    assert(number < MAX_SAVES);
    const int TOTAL_COUNTRIES = 178;

    auto saves = getSaveNames();

    Save save;
    save.lastPlayed = time(nullptr);
    save.progress = float(game.map.possibleCountries.size()) / TOTAL_COUNTRIES;
    if(number < int(saves.size())) {
        save.file = std::format("{}.save", number);
        saves[number] = save;
    } else {
        save.file = std::format("{}.save", saves.size());
        saves.push_back(save);
    }

    json worlds;
    worlds["saves"] = saves;
    std::ofstream worldsFile(SAVE_DIR / "worlds.json");
    worldsFile << worlds.dump(4) << std::endl;
    worldsFile.close();

    std::ofstream saveFile(SAVE_DIR / save.file);
    json j;
    j["currentTick"] = game.currentTick;
    
    j["zoom"] = game.camera.zoom;
    j["pos"][0] = game.camera.pos.x;
    j["pos"][1] = game.camera.pos.y;
    
    j["cash"] = game.player.cash;
    j["difficulty"] = game.player.difficulty;
    j["fastForwardMultiplier"] = game.player.fastForwardMultiplier;
    j["stats"]["routes"] = game.player.stats.routes;
    j["stats"]["planes"] = game.player.stats.planes;
    j["stats"]["flights"] = game.player.stats.flights;
    j["stats"]["passengersTotal"] = game.player.stats.passengersTotal;
    j["stats"]["passengersArrived"] = game.player.stats.passengersArrived;
    j["stats"]["countries"] = game.player.stats.countries;
    j["stats"]["airports"] = game.player.stats.airports;
    j["stats"]["population"] = game.player.stats.population;
    j["stats"]["moneySpent"] = game.player.stats.moneySpent;
    j["stats"]["moneyEarned"] = game.player.stats.moneyEarned;

    for(const auto& a: game.airport.airports) {
        json k;
        k["level"] = a.level;
        k["radius"] = a.radius;
        k["waiting"] = a.waiting;
        k["routeIndexes"] = a.routeIndexes;
        j["airports"].push_back(k);
    }

    for(const auto& r: game.airport.routes) {
        json k;
        k["a"] = r.a;
        k["b"] = r.b;
        k["level"] = r.level;
        k["lenght"] = r.lenght;
        k["lastTakeoffA"] = r.lastTakeoffA;
        k["lastTakeoffB"] = r.lastTakeoffB;

        for(const auto& p: r.planes) {
            json l;
            l["t"] = p.t;
            l["speed"] = p.speed;
            l["pass"] = p.pass;
            k["planes"].push_back(l);
        }
        j["routes"].push_back(k);
    }
    j["citiesIndexes"] = game.citiesIndexes;
    j["networkAdjList"] = game.airport.networkAdjList;
    j["routeGrids"] = game.airport.routeGrids;

    for(const auto& u: game.map.possibleCountries) {
        json k;
        k["name"] = u.name;
        k["currentCity"] = u.currentCity;
        k["population"] = u.population;
        j["possibleCountries"].push_back(k);
    }

    saveFile << j << std::endl;
    saveFile.close(); //Not needed
}
