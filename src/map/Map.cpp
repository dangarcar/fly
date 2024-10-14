#include "Map.hpp"

#include "../../include/json.hpp"
#include <fstream>
#include <format>

#include "../engine/Gradient.h"
#include "../game/Camera.hpp"
#include "../Player.hpp"
#include "../ui/UIManager.hpp"
#include "../ui/dialogs/UnlockCountryDialog.hpp"

SDL_Color getCountryColor(const Map& map, const Country& country);
BoundingBox createBoundingBox(Coord c1, Coord c2, const Camera& cam);
Texture createFlag(Camera& camera, const std::string& svg);

void Map::load(Camera& camera) {
    labelManager.load(camera);
    citySpawner.load(camera);

    //COUNTRY MESH
    using json = nlohmann::json;
    std::ifstream countryFile(COUNTRIES_DATA_FILE);
    json countryData = json::parse(countryFile);

    for(auto& [k, v]: countryData.items()) {
        Country c;
        c.name = v["name"].template get<std::string>();
        auto banned = v["banned"].template get<bool>();
        c.state = banned? CountryState::BANNED : CountryState::LOCKED;

        std::vector<Polygon> polys;
        for(auto& e: v["mesh"]) {
            Polygon p;
            auto box = e["box"].template get<std::vector<std::vector<float>>>(); //It was not compatible with Bounding box type
            p.vertexIndex = e["vertexIndex"].template get<std::pair<int, int>>();
            p.triangleIndex = e["triangleIndex"].template get<std::pair<int, int>>();
            p.boundingBox = createBoundingBox({box[0][0], box[0][1]}, {box[1][0], box[1][1]}, camera);

            polys.emplace_back(std::move(p));
        }

        std::sort(polys.begin(), polys.end(), [](Polygon& a, Polygon& b){ return a.boundingBox.area() > b.boundingBox.area(); });
        polygons.insert(polygons.end(), polys.begin(), polys.end());
        c.meshIndex = std::make_pair(polygons.size() - polys.size(), polygons.size());

        countries[k] = std::move(c);
    }

    //MESH LOADING
    std::ifstream meshFile(MESH_DATA_FILE, std::ios::binary);
    int32_t vertexSize;
    meshFile.read(reinterpret_cast<char*>(&vertexSize), sizeof(int32_t));

    int32_t triSize;
    meshFile.read(reinterpret_cast<char*>(&triSize), sizeof(int32_t));

    vertices.clear();
    vertices.resize(vertexSize * 2);
    meshFile.read(reinterpret_cast<char*>(&vertices[0]), vertexSize * 2 * sizeof(float));

    triangles.clear();
    triangles.resize(triSize * 3);
    meshFile.read(reinterpret_cast<char*>(&triangles[0]), triSize * 3 * sizeof(int32_t));

    this->projectMap(camera);

    //FLAG LOADING
    std::ifstream flagFile(FLAGS_DATA_FILE);
    json flagData = json::parse(flagFile);
    for(auto& [k, v]: flagData.items()) {
        auto svg = v.template get<std::string>();
        camera.getTextureManager().loadTexture(k, createFlag(camera, svg));
    }

    /*//FIXME: stress test
    for(auto& [k, c]: countries) {
        if(c.state == CountryState::BANNED)
            continue;

        c.state = CountryState::UNLOCKED;
        citySpawner.addCountry(k);
    }*/
}

void Map::projectMap(const Camera& camera) {
    projectedVertices.resize(vertices.size());
    
    for(size_t i=0; i<vertices.size(); i += 2) {
        auto proj = camera.coordsToProj({vertices[i], vertices[i + 1]});
        projectedVertices[i] = proj.x;
        projectedVertices[i + 1] = proj.y;
    }

    vertexArray = std::vector<SDL_Vertex>(vertices.size() / 2);
    lineArray.clear();
    for(auto& p: polygons) {
        auto [begV, endV] = p.vertexIndex;            
        lineArray.emplace_back(endV - begV);
    }
}

void Map::render(const Camera& camera) {
    SDL_SetRenderDrawColor(camera.getSDL(), seaColor.r, seaColor.g, seaColor.b, seaColor.a);
    SDL_Rect rect = camera.getScreenViewportRect();
    SDL_RenderFillRect(camera.getSDL(), &rect);

    //Projection calculation in parallel
    #pragma omp parallel for
    for(int i=0; i<int(polygons.size()); ++i) {
        auto [beg, end] = polygons[i].vertexIndex;
        for(int j=beg; j<end; ++j) {
            SDL_Vertex v;
            v.color = SDL_GOLD;
            v.position = camera.projToScreen({projectedVertices[2*j], projectedVertices[2*j + 1]});
            vertexArray[j] = v;
            lineArray[i][j - beg] = SDL_Point{int(v.position.x), int(v.position.y)};
        }
    }

    //Color without parallelization
    for(auto& [name, country]: countries) {
        auto countryColor = getCountryColor(*this, country);
        auto [beg, end] = country.meshIndex;
        for(int i=beg; i<end; ++i) {
            auto [begV, endV] = polygons[i].vertexIndex;
            for(auto j=begV; j<endV; ++j) {
                vertexArray[j].color = countryColor;
            }
        }
    }

    SDL_RenderGeometry(camera.getSDL(), nullptr, vertexArray.data(), vertexArray.size(), triangles.data(), triangles.size());

    labelManager.render(camera);

    SDL_SetRenderDrawColor(camera.getSDL(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    for(auto& l: lineArray) {
        SDL_RenderDrawLines(camera.getSDL(), l.data(), l.size());
    }

    //BOX RENDER
    if(renderBoxes) {
        for(auto [k, c]: countries) {
            auto& pol = polygons[c.meshIndex.first];
            auto x1 = pol.boundingBox.topLeft.x, y1 = pol.boundingBox.topLeft.y;
            auto x2 = pol.boundingBox.bottomRight.x, y2 = pol.boundingBox.bottomRight.y;
            std::vector box = { camera.projToScreen({x1, y1}), camera.projToScreen({x1, y2}), camera.projToScreen({x2, y2}), camera.projToScreen({x2, y1}), camera.projToScreen({x1, y1}) };
            SDL_SetRenderDrawColor(camera.getSDL(), 0, 0xFF, 0xFF, 0xFF);
            SDL_RenderDrawLinesF(camera.getSDL(), box.data(), box.size());
        }
    }
}

void Map::handleInput(const InputEvent& event, Camera& camera, UIManager& uiManager, Player& player) {
    if(auto* keyevent = std::get_if<KeyPressedEvent>(&event)) {
        if(keyevent->keycode == SDLK_q)
            renderBoxes = ! renderBoxes;
    }

    if(auto* clickEvent = std::get_if<ClickEvent>(&event)) {
        if(clickEvent->button == SDL_BUTTON_LEFT) {
            if(!targetCountry.empty() && countries[targetCountry].state != CountryState::BANNED)
                if(countries[targetCountry].state != CountryState::UNLOCKED) {
                    moveToCountry(countries[targetCountry], camera);

                    uiManager.addDialog<UnlockCountryDialog>(countries[targetCountry].name, targetCountry, *this, player);
                }
        }
    }

    if([[maybe_unused]] auto* resizedevent = std::get_if<WindowResizedEvent>(&event)) {
        this->projectMap(camera);
    }

    if(auto* moveevent = std::get_if<MouseMoveEvent>(&event)) {
        mousePos = moveevent->newPos;
    }
}

void Map::update(const Camera& camera) {
    auto mouseProj = camera.screenToProj(mousePos);
    targetCountry.clear();

    Polygon* hoveredPolygon = nullptr; //There can be more than one at a time (Thanks Lesotho)
    for(auto& [name, country]: countries) {
        if(country.state == CountryState::HOVERED)
            country.state = CountryState::LOCKED;

        auto [beg, end] = country.meshIndex;
        for(int i=beg; i<end; ++i) {
            if(isIntersecting(polygons[i], mouseProj)) {
                if(hoveredPolygon && hoveredPolygon->boundingBox.area() < polygons[i].boundingBox.area())
                    continue;
                targetCountry = name;
                hoveredPolygon = &polygons[i];
                break;
            }
        }
    }

    if(!targetCountry.empty() && countries[targetCountry].state == CountryState::LOCKED)
        countries[targetCountry].state = CountryState::HOVERED;
}

bool Map::isIntersecting(const Polygon& p, const glm::vec2& v) const {
    if(v.x < p.boundingBox.topLeft.x || v.y < p.boundingBox.topLeft.y
    || v.x > p.boundingBox.bottomRight.x || v.y > p.boundingBox.bottomRight.y)
        return false;

    bool inside = false;
    auto [beg, end] = p.vertexIndex;
    int numVertices = end - beg;

    glm::vec2 p1 = { projectedVertices[2*beg], projectedVertices[2*beg + 1] }, p2;
    for(int i=1; i<=numVertices; ++i) {
        int j = beg + (i % numVertices);
        p2 = {projectedVertices[j*2], projectedVertices[j*2 + 1]};

        if(v.y > std::min(p1.y, p2.y) && v.y <= std::max(p1.y, p2.y)) {
            if(v.x <= std::max(p1.x, p2.x)) {
                auto xInter = (v.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;

                if(p1.x == p2.x || v.x <= xInter)
                    inside = !inside;
            }
        }

        p1 = p2;
    }

    return inside;
}

void Map::unlockCountry(const std::string& country, Player& player) {
    countries[country].state = CountryState::UNLOCKED;
    citySpawner.addCountry(country);

    long price = long( double(DEFAULT_CITY_PRICE) * player.getDifficulty() );
    player.spend(price);
    player.stats.countries++;
}

void Map::moveToCountry(const Country& country, Camera& camera) {
    auto bb = polygons[country.meshIndex.first].boundingBox; //Biggest polygon bb
    camera.setZoom(20.0f);

    auto countryProj = (bb.topLeft + bb.bottomRight) * 0.5f;
    auto rect = camera.getScreenViewportRect();
    auto centerProj = camera.screenToProj({rect.w/2, rect.h/2});
    camera.move(centerProj - countryProj);
    
    centerProj = camera.screenToProj({rect.w/2, rect.h/2}); //It's not the same anymore
    auto dv = centerProj - bb.topLeft;
    auto zoom = std::min((rect.w/2.0f) / dv.x, (rect.h/2.0f) / dv.y);
    zoom = SDL_clamp(zoom * 0.7f, 1.0f, 12.0f);
    camera.setZoom(zoom);
    camera.move({0.0f, 0.0f}); //To correct out of bounds errors
}

SDL_Color getCountryColor(const Map& map, const Country& country) {
    switch (country.state) {
        case CountryState::UNLOCKED: return map.unlockedColor;
        case CountryState::LOCKED: return map.lockedColor;
        case CountryState::HOVERED: return map.hoveredColor;
        default: return map.bannedColor;
    }
}

BoundingBox createBoundingBox(Coord c1, Coord c2, const Camera& cam) {
    auto v1 = cam.coordsToProj(c1);
    auto v2 = cam.coordsToProj(c2);

    return BoundingBox { glm::min(v1, v2), glm::max(v1, v2) };
}

Texture createFlag(Camera& camera, const std::string& svg) {
    auto& circle = camera.getTextureManager().getTexture("CIRCLE");
    Texture country;
    country.loadSVG(*camera.getSDL(), svg);
    country.applyMask(*camera.getSDL(), circle);

    return country;
}
