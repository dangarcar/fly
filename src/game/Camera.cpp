#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::projToGLSpaceMatrix() const {
    glm::mat4 mat(1.0f);

    mat = glm::translate(mat, glm::vec3(-1, 1, 0));
    mat = glm::scale(mat, glm::vec3(2.0f/width, -2.0f/height, 1));
    mat = glm::scale(mat, glm::vec3(zoom, zoom, 1));
    mat = glm::translate(mat, glm::vec3(-pos.x + width/2.0f, -pos.y + height/2.0f, 0));

    return mat;
}

glm::vec2 Camera::coordsToProj(Coord coords) const {
    glm::vec2 proj;
    const double M_1_2PI = 1.0 / (2.0 * M_PI);
    proj.x = width * M_1_2PI * (M_PI + glm::radians(coords.lon));
    proj.y = width * M_1_2PI * (M_PI - glm::log(glm::tan(glm::quarter_pi<double>() + glm::radians(coords.lat)/2.0)));
    return proj;
}

Coord Camera::projToCoords(glm::vec2 proj) const {
    Coord c;
    c.lon = glm::degrees(2.0 * M_PI * proj.x / width - M_PI); 
    c.lat = glm::degrees(2.0 * (atan(exp(M_PI - 2.0 * M_PI * proj.y / width)) - glm::quarter_pi<double>()));
    return c;
}

SDL_FPoint Camera::projToScreen(glm::vec2 proj) const {
    SDL_FPoint p;
    p.x = (proj.x - pos.x + width/2.0f) * zoom; 
    p.y = (proj.y - pos.y + height/2.0f) * zoom;
    return p;
}

glm::vec2 Camera::screenToProj(SDL_Point p) const {
    glm::vec2 proj;
    proj.x = p.x / zoom + pos.x - width/2.0f; 
    proj.y = p.y / zoom + pos.y - height/2.0f;
    return proj;
}

void Camera::handleInput(const InputEvent& event) {
    if(auto* wheelevent = std::get_if<MouseWheelEvent>(&event)) {
        float newZoom;
        if(wheelevent->direction < 0)
            newZoom = std::max(1.0f, zoom - zoom*ZOOM_SPEED);
        else
            newZoom = std::min(MAX_ZOOM, zoom + zoom*ZOOM_SPEED);
        auto dz = (zoom - newZoom) / (zoom * newZoom);
        zoom = newZoom;
                
        move(glm::vec2(wheelevent->mousePos.x, wheelevent->mousePos.y) * dz);
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        if(moveevent->leftDown) {
            auto p1 = screenToProj(moveevent->oldPos);
            auto p2 = screenToProj(moveevent->newPos);
            move(p2 - p1);
        }
    }

    if(auto* resizedevent = std::get_if<WindowResizedEvent>(&event)) {
        auto normPos = pos / float(width);
        width = resizedevent->width;
        height = resizedevent->height;
        pos = normPos * float(width);
    }
}

void Camera::move(glm::vec2 v) {
    pos = {pos.x - v.x, pos.y - v.y};

    auto top = screenToProj({0, 0});
    auto bottom = screenToProj({width, height});

    if(top.y < 0)
        pos.y += -top.y;
    else if(bottom.y > width)
        pos.y += width - bottom.y;

    if(top.x <= 0)
        pos.x += -top.x;
    else if(bottom.x >= width)
        pos.x += width - bottom.x;
}

CameraSave Camera::serialize() const {
    CameraSave save;

    save.pos = projToCoords(this->pos);
    save.zoom = this->zoom;

    return save;
}

void Camera::deserialize(const CameraSave& save) {
    this->pos = coordsToProj(save.pos);
    this->zoom = save.zoom;

    this->move({0, 0}); //To move camera if outside boundaries
}
