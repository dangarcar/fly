#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <ctime>

#include "../../include/json.hpp"

#include "Game.hpp"

namespace GameSaver {
    constexpr int MAX_SAVES = 7;
    const std::filesystem::path SAVE_DIR = "./saves";

    struct Save {
        float progress;
        std::time_t lastPlayed;
        std::filesystem::path file;
    };

    std::vector<Save> getSaveNames();
    GameSave getSave(int saveIndex);

    void save(const GameSave& game, int number);

    inline void to_json(nlohmann::json& j, const GameSaver::Save& r) {
        j = nlohmann::json {
            {"progress", r.progress}, 
            {"file", r.file}, 
            {"lastPlayed", r.lastPlayed}
        };
    }

    inline void from_json(const nlohmann::json& j, GameSaver::Save& r) {
        r.progress = j.at("progress").get<float>();
        r.file = j.at("file").get<std::filesystem::path>();
        r.lastPlayed = j.at("lastPlayed").get<std::time_t>();
    }
};
