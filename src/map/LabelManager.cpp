#include "LabelManager.hpp"

#include "../../include/json.hpp"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/rotate_vector.hpp>

#include <fstream>
#include "../game/Camera.hpp"
#include "../Player.hpp"

void LabelManager::load(const Camera& camera) {
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
        label.texture = std::move(camera.getTextRenderer().renderToTexture(*camera.getSDL(), name));
        label.texture.setColorMod(FC_MakeColor(0x60, 0x60, 0x60, 0xFF));

        labels[k] = std::move(label);
    }
}

void LabelManager::render(const Camera& camera) {
    for(auto& [k, label]: labels) {
        auto sz = label.size * camera.getZoom(); 
        auto v = camera.coordsToScreen(label.coord);
        auto dv = glm::rotate(glm::vec2(label.texture.getWidth()/2.f, label.texture.getHeight()/2.f) * sz, glm::radians(label.angle));
        label.texture.render(*camera.getSDL(), v.x - dv.x, v.y - dv.y, sz, label.angle, SDL_BLENDMODE_ADD);
    }
}
