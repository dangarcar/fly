#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "../game/Types.h"
#include "../engine/Texture.hpp"

struct Label {
    static constexpr int NUMBER_TEXTURE = 10;

    std::array<Texture, NUMBER_TEXTURE> texture;
    Coord coord;
    float angle;
    float size;
};

class PlayerCamera;
class Camera;

class LabelManager {
private:
    static constexpr auto LABELS_DATA_FILE = "./resources/labels.json";
    std::unordered_map<std::string, Label> labels;

public:
    void load(Camera& camera);
    
    void render(const Camera& camera);
};