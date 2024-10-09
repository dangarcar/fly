#include "LabelManager.hpp"

#include "../../include/json.hpp"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/rotate_vector.hpp>

#include <fstream>
#include "../game/Camera.hpp"
#include "../Player.hpp"
#include "../engine/Window.hpp"

void LabelManager::load(Camera& camera) {    
    using json = nlohmann::json;

    std::ifstream labelFile(LABELS_DATA_FILE);
    json labelData = json::parse(labelFile);

    for(auto& [k, v]: labelData.items()) {
        Label label;
        label.size = v["size"].template get<float>() * 128.0f * (float(camera.getHeight()) / Window::DEFAULT_SCREEN_HEIGHT); //128 was the default size when this file was made
        label.angle = v["angle"].template get<float>();
        auto center = v["coord"].template get<std::vector<float>>();
        label.coord = {center[0], center[1]};
        auto name = v["name"].template get<std::string>();

        for(int i=0; i<Label::NUMBER_TEXTURE; ++i) {
            auto size = 2 << i;
            if(size > 2 * Camera::MAX_ZOOM * label.size)
                break;
            
            label.texture[i] = std::move(camera.getTextRenderer().renderToTexture(*camera.getSDL(), name, size));
            label.texture[i].setColorMod(FC_MakeColor(0x60, 0x60, 0x60, 0xFF));
        }


        labels[k] = std::move(label);
    }
}

void LabelManager::render(const Camera& camera) {
    for(auto& [k, label]: labels) {
        auto sz = label.size * camera.getZoom();

        auto textureIndex =  std::clamp(0, Label::NUMBER_TEXTURE - 1, int(std::log2(sz)));
        sz /= label.texture[textureIndex].getHeight();

        auto v = camera.coordsToScreen(label.coord);
        auto centre = glm::vec2(label.texture[textureIndex].getWidth()/2.f, label.texture[textureIndex].getHeight()/2.f);
        auto dv = glm::rotate(centre * sz, glm::radians(label.angle));
        label.texture[textureIndex].render(*camera.getSDL(), int(v.x - dv.x), int(v.y - dv.y), sz, label.angle, SDL_BLENDMODE_ADD);
    }
}
