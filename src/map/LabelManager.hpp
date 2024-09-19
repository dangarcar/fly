#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "../game/Types.h"
#include "../engine/Texture.hpp"

#define LABELS_DATA_FILE "./resources/labels.json"

struct Label {
    Texture texture;
    Coord coord;
    float angle;
    float size;
};

class PlayerCamera;
class Camera;

class LabelManager {
private:
    std::unordered_map<std::string, Label> labels;

public:
    void render(const Camera& camera);
    void load(const Camera& camera);
};