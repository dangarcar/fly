#include "Map.hpp"

#include <json/json.hpp>
#include <fstream>
#include <unordered_map>

#include "../game/Camera.hpp"
#include "../Player.hpp"
#include "../ui/UIManager.hpp"
#include "../ui/dialogs/UnlockCountryDialog.hpp"

BoundingBox createBoundingBox(Coord c1, Coord c2, const Camera& cam);
SDL_Color getCountryColor(const Map& map, const Country& country);

void Map::load(Camera& camera) {
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
        camera.loadTexture(k, std::format("./assets/countries/{}.png", k));
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

    //LOAD SUBCOMPONENTS
    citySpawner.load(camera);
    mapRenderer.load(projectedVertices, triangles, polygons, countries);

    //FIXME:
    for(auto& [k, v]: countries) {
        if(countries[k].state == CountryState::LOCKED) {
            countries[k].state = CountryState::UNLOCKED;
            citySpawner.addCountry(k);
        }
    }
}

void Map::projectMap(const Camera& camera) {
    projectedVertices.resize(vertices.size());
    
    for(size_t i=0; i<vertices.size(); i += 2) {
        auto proj = camera.coordsToProj({vertices[i], vertices[i + 1]});
        projectedVertices[i] = proj.x;
        projectedVertices[i + 1] = proj.y;
    }
}

void Map::render(const Camera& camera) {
    camera.fillRect(camera.getScreenViewportRect(), seaColor);

    std::unordered_map<std::string, SDL_Color> colors;
    for(auto& [k, c]: countries)
        colors[k] = getCountryColor(*this, c);
    
    mapRenderer.render(camera, colors);
}

void Map::handleInput(const InputEvent& event, Camera& camera, UIManager& uiManager, Player& player) {
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

MapSave Map::serialize() const {
    return citySpawner.serialize();
}

void Map::deserialize(const MapSave& save) {
    for(auto& s: save.possibleCountries) {
        this->countries[s.name].state = CountryState::UNLOCKED;
    }

    citySpawner.deserialize(save);
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
