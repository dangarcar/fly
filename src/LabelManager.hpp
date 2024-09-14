#pragma once

#include "../include/json.hpp"

#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "Camera.hpp"
#include <glm/gtx/rotate_vector.hpp>

#define LABELS_DATA_FILE "./resources/labels.json"

struct Label {
    Texture texture;
    Coord coord;
    float angle;
    float size;
};

class LabelManager: public GameObject {
private:
    std::map<std::string, Label> labels;
    const Camera& camera;

public:
    LabelManager(const Camera& cam): camera(cam) {}

    void handleEvents(const Event& event) override {}
    void update() override {}
    void render(const Renderer& renderer) override;
    void load(const Renderer& renderer) override;
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

void LabelManager::render(const Renderer& renderer) {
    for(auto& [k, label]: labels) {
        auto sz = label.size * camera.getZoom();

        //if(label.texture.getHeight() * sz > 12) {    
            auto v = camera.coordsToScreen(label.coord);
            auto dv = glm::rotate(glm::vec2(label.texture.getWidth()/2.f, label.texture.getHeight()/2.f) * sz, glm::radians(label.angle));
            label.texture.render(*renderer.getSDL(), v.x - dv.x, v.y - dv.y, sz, label.angle, SDL_BLENDMODE_ADD);
        //}
    }
}
