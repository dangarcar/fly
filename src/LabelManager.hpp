#pragma once

#include "../include/json.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

#include "PlayerCamera.hpp"
#include <glm/gtx/rotate_vector.hpp>

#define LABELS_DATA_FILE "./resources/labels.json"

struct Label {
    Texture texture;
    Coord coord;
    float angle;
    float size;
};

class LabelManager {
private:
    std::unordered_map<std::string, Label> labels;

public:
    LabelManager() = default;

    void registerEvents(Event::EventManager& manager) {}
    void update() {}
    void render(const Renderer& renderer, const PlayerCamera& camera);
    void load(const Renderer& renderer);
};

void LabelManager::load(const Renderer& renderer) {
    using json = nlohmann::json;

    std::ifstream labelFile(LABELS_DATA_FILE);
    json labelData = json::parse(labelFile);

    for(auto& [k, v]: labelData.items()) {
        Label label;
        label.size = v["size"].template get<float>();
        label.angle = v["angle"].template get<float>();
        auto center = v["coord"].template get<std::vector<float>>();
        label.coord = {center[0], center[1]};
        auto name = v["name"].template get<std::string>();
        label.texture = std::move(renderer.getTextRenderer().renderToTexture(*renderer.getSDL(), name));
        label.texture.setColorMod(FC_MakeColor(0x60, 0x60, 0x60, 0xFF));

        labels[k] = std::move(label);
    }
}

void LabelManager::render(const Renderer& renderer, const PlayerCamera& camera) {
    for(auto& [k, label]: labels) {
        auto sz = label.size * camera.getZoom(); 
        auto v = camera.coordsToScreen(label.coord);
        auto dv = glm::rotate(glm::vec2(label.texture.getWidth()/2.f, label.texture.getHeight()/2.f) * sz, glm::radians(label.angle));
        label.texture.render(*renderer.getSDL(), v.x - dv.x, v.y - dv.y, sz, label.angle, SDL_BLENDMODE_ADD);
    }
}
