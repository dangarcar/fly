#include "Player.hpp"

#include <format>
#include <SDL_FontCache.h>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/rotate_vector.hpp>

#include "game/Game.hpp"
#include "game/Camera.hpp"
#include "engine/Gradient.h"

constexpr int TICKS_PER_CLOCK_CYCLE = 250;

bool Player::handleInput(const InputEvent& event) {
    if(auto* clickevent = std::get_if<ClickEvent>(&event)) {
        if(clickevent->button == SDL_BUTTON_LEFT && SDL_PointInRect(&clickevent->clickPoint, &ffButton)) {
            fastForwardMultiplier = fastForwardMultiplier % Game::MAX_FAST_FORWARD + 1;
            return true;
        }
    }

    return false;
}

void renderClockHand(const Camera& camera, SDL_Point center, int w, int h, float angle, SDL_Color color) {
    const int indices[] = {0, 1, 2, 1, 2, 3};
    
    std::vector<SDL_Vertex> minuteVertices = {
        SDL_Vertex { .position = {float(center.x - w/2), float(center.y)}, .color = color, .tex_coord = {0,0}},
        SDL_Vertex { .position = {float(center.x + w/2), float(center.y)}, .color = color, .tex_coord = {0,0}},
        SDL_Vertex { .position = {float(center.x - w/2), float(center.y - h)}, .color = color, .tex_coord = {0,0}},
        SDL_Vertex { .position = {float(center.x + w/2), float(center.y - h)}, .color = color, .tex_coord = {0,0}}
    };

    for(auto& p: minuteVertices) {
        auto vCenter = glm::vec2(float(center.x), float(center.y));
        auto a = glm::vec2(p.position.x, p.position.y) - vCenter;
        auto v = vCenter + glm::rotate(a, angle);
        p.position = SDL_FPoint {v.x, v.y};
    }

    SDL_RenderGeometry(&camera.getSDL(), nullptr, minuteVertices.data(), minuteVertices.size(), indices, 6);
}

void renderFastForward(int n, const Camera& camera, float x, float y, float w, float h, SDL_Color color) {
    SDL_Vertex triVertices[3] = {
        SDL_Vertex { .position = {x - 3*n, y}, .color = color, .tex_coord = {0,0}},
        SDL_Vertex { .position = {x - 3*n, y + h}, .color = color, .tex_coord = {0,0}},
        SDL_Vertex { .position = {x - 3*n + w, y + h/2}, .color = color, .tex_coord = {0,0}},
    };

    for(int i=0; i<=n; ++i) {
        SDL_RenderGeometry(&camera.getSDL(), nullptr, triVertices, 3, nullptr, 3);
        
        for(int j=0; j<3; ++j)
            triVertices[j].position.x += w / n + 6;
    }
}

void Player::render(const Camera& camera, int currentTick) {
    auto screen = camera.getScreenViewportRect();
    auto text = std::format("${}", cash);
    auto bounds = camera.getTextRenderer().getTextBounds(text, 36);
    
    auto rect = SDL_Rect {screen.w - bounds.w - 202, 0, bounds.w + 192, 92};
    SDL_SetRenderDrawColor(&camera.getSDL(), 0xE0, 0xE0, 0xE0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(&camera.getSDL(), &rect);

    camera.renderText(text, screen.w - bounds.w - 192, 28, 36, FC_ALIGN_LEFT, FC_MakeColor(0, 0, 0, SDL_ALPHA_OPAQUE));

    float minuteAngle = 2*M_PI / TICKS_PER_CLOCK_CYCLE * (currentTick % TICKS_PER_CLOCK_CYCLE);
    renderClockHand(camera, SDL_Point(screen.w-56, 46), 2, 30, minuteAngle, SDL_SILVER);

    float hourAngle = 2*M_PI / (24*TICKS_PER_CLOCK_CYCLE) * (currentTick % (24*TICKS_PER_CLOCK_CYCLE));
    renderClockHand(camera, SDL_Point(screen.w-56, 46), 4, 20, hourAngle, SDL_GOLD);

    auto& t = camera.getTextureManager().getTexture("CLOCK");
    rect.w = rect.h = 72;
    t.render(camera.getSDL(), screen.w - 92, 10, &rect);

    ffButton = SDL_Rect { screen.w - 172, 20, 60, 52 };
    renderFastForward(fastForwardMultiplier, camera, screen.w - 172, 20, 30, 52, SDL_BLACK);
}

void Player::update() {
    difficulty += 1e-5; //MORE OR LESS +50% PER HOUR 0,00001 * 15 * 3600 = 0,54
}
