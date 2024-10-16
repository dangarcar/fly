#include "Route.hpp"

#include "Airport.hpp"
#include "../game/Camera.hpp"

int air::getPointGrid(const Camera& camera, glm::vec2 proj) {
    int x = proj.x * ROUTE_GRID_WIDTH / camera.getWidth();
    int y = proj.y * ROUTE_GRID_HEIGHT / camera.getWidth();
    return std::clamp(y*ROUTE_GRID_WIDTH + x, 0, ROUTE_GRID_WIDTH*ROUTE_GRID_HEIGHT-1);
}

float air::routePrice(const Route& route) {
    return route.lenght * PRICE_PER_METER_ROUTE;
}

bool air::routeClicked(const Camera& camera, const Route& route, SDL_Point mousePos) {
    //FIND IF CLICKED PLANE
    for(auto& plane: route.planes) {
        auto [proj, angle] = getPointAndAngle(route, plane.t);
        auto p = camera.projToScreen(proj);

        auto radius = getRelativeRadius(AIRPLANE_SCALE * 128, camera.getZoom());
        if(SDL_sqrdistance({int(p.x), int(p.y)}, mousePos) < radius*radius) {
            return true;
        }
    }

    //FIND IF CLICKED ROUTE
    for(auto& proj: route.points) {
        auto p = camera.projToScreen(proj);

        if(SDL_sqrdistance({int(p.x), int(p.y)}, mousePos) < 10) {
            return true;
        }
    }

    return false;
}

void air::renderRoutePath(const Camera& camera, const Route& route) {
    float dist = route.lenght / EARTH_RADIUS;
    int n = dist * std::clamp(camera.getZoom(), 2.0f, 20.0f) * 20;
    n += n % 2 + 1;
    auto routeColor = ROUTE_COLOR_BY_LEVEL[route.level];
    SDL_SetRenderDrawColor(&camera.getSDL(), routeColor.r, routeColor.g, routeColor.b, 0xff);
    glm::vec2 lastProj;
    for(int i=0; i<=n; ++i) {
        auto t = float(i) / n;
        auto proj = route.points[ static_cast<size_t>(t * (route.points.size()-1)) ];
        if(i % 2 == 1) {
            auto lastPoint = camera.projToScreen(lastProj);
            auto p = camera.projToScreen(proj);
            if(std::abs(lastPoint.x - p.x) < camera.getWidth()/2)
                SDL_RenderDrawLine(&camera.getSDL(), int(p.x), int(p.y), int(lastPoint.x), int(lastPoint.y));
        }
        lastProj = proj;
    }
}

void air::renderRoutePlanes(const Camera& camera, const Route& route, float frameProgress) {
    for(auto& plane: route.planes) {
        auto [proj, angle] = getPointAndAngle(route, std::clamp(plane.t + plane.speed*frameProgress, 0.0f, 1.0f));
        auto p = camera.projToScreen(proj);
        auto& t = camera.getTextureManager().getTexture(PLANE_TEXTURE_PER_LEVEL[route.level]);
        auto scale = getRelativeRadius(AIRPLANE_SCALE, camera.getZoom());
        auto distA = glm::distance(proj, route.points.front()) * camera.getZoom();
        auto distB = glm::distance(proj, route.points.back()) * camera.getZoom();
        auto fillPercentage = std::min(1.0f, float(plane.pass.size()) / PLANE_CAPACITY_PER_LEVEL[route.level]);
        assert(fillPercentage >= 0.0f);
        if(distA > scale * t.getWidth()/2 && distB > scale * t.getWidth()/2) {
            t.setColorMod(FULL_GRADIENT.getColor(fillPercentage));
            t.renderCenter(camera.getSDL(), p.x, p.y, scale, angle + 180.0f * (plane.speed < 0));
        }
    }
}

Coord air::getIntermediatePoint(Coord c1, Coord c2, float t) {    
    auto lat1 = glm::radians(c1.lat), lon1 = glm::radians(c1.lon);
    auto lat2 = glm::radians(c2.lat), lon2 = glm::radians(c2.lon);
    auto sinlat = std::sin((lat1-lat2) / 2);
    auto sinlon = std::sin((lon1-lon2) / 2);
    auto tmp = sinlat*sinlat + std::cos(lat1) * std::cos(lat2) * sinlon*sinlon;
    auto d = 2.0 * std::asin(std::sqrt(tmp));
    auto A = std::sin((1-t) * d) / std::sin(d);
    auto B = std::sin(t * d) / std::sin(d);
    auto x = A * std::cos(lat1) * std::cos(lon1) + B * std::cos(lat2) * std::cos(lon2);
    auto y = A * std::cos(lat1) * std::sin(lon1) + B * std::cos(lat2) * std::sin(lon2);
    auto z = A * std::sin(lat1) + B * std::sin(lat2);
    Coord result;
    result.lat = glm::degrees(std::atan2(z, std::sqrt(x*x + y*y)));
    result.lon = glm::degrees(std::atan2(y, x));
    return result;
}

std::vector<glm::vec2> air::getPathProjs(const Camera& camera, Coord a, Coord b) {
    int n = mtsDistance(a, b) * Camera::MAX_ZOOM * 100 / EARTH_RADIUS;
    std::vector<glm::vec2> path(n+1);
    #pragma omp parallel for
    for(int i=0; i<=n; ++i) {
        auto c = getIntermediatePoint(a, b, float(i)/n);
        path[i] = camera.coordsToProj(c);
    }
    return path;
}

std::pair<glm::vec2, float> air::getPointAndAngle(const Route& route, float t) {
    assert(route.points.empty() == false);

    float fi = t * (route.points.size() - 1);
    int i1 = std::floor(fi), i2 = std::ceil(fi);
    auto v = glm::normalize(route.points[i1] - route.points[i2]);
    auto angle = glm::degrees( std::acos(glm::dot(glm::vec2(0.0f, 1.0f), v)) );
    if(route.points[i1].x > route.points[i2].x)
        angle = 360 - angle;
    return std::make_pair(glm::mix(route.points[i1], route.points[i2], fi - i1), angle);
}
