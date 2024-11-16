#include "Route.hpp"

#include "Airport.hpp"
#include "../game/Camera.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

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

        auto radius = getRelativeRadius(AIRPLANE_SCALE, camera.getZoom());
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

void air::renderRoutePlanes(const Camera& camera, const Route& route, float frameProgress) {
    for(auto& plane: route.planes) {
        auto [proj, angle] = getPointAndAngle(route, std::clamp(plane.t + plane.speed*frameProgress, 0.0f, 1.0f));
        auto p = camera.projToScreen(proj);
        auto scale = getRelativeRadius(AIRPLANE_SCALE, camera.getZoom());
        auto distA = glm::distance(proj, route.points.front()) * camera.getZoom();
        auto distB = glm::distance(proj, route.points.back()) * camera.getZoom();
        auto fillPercentage = std::min(1.0f, float(plane.pass.size()) / PLANE_CAPACITY_PER_LEVEL[route.level]);
        assert(fillPercentage >= 0.0f);
        
        if(distA > scale/2 && distB > scale/2) {
            auto c = FULL_GRADIENT.getColor(fillPercentage);
            auto t = PLANE_TEXTURE_PER_LEVEL[route.level];
            camera.renderExt(t, p.x, p.y, scale, angle + 180.0f * (plane.speed < 0), true, c);
        }
    }
}

Coord air::getIntermediatePoint(Coord c1, Coord c2, float t) {    
    auto lat1 = glm::radians(c1.lat), lon1 = glm::radians(c1.lon);
    auto lat2 = glm::radians(c2.lat), lon2 = glm::radians(c2.lon);
    auto sinlat = glm::sin((lat1-lat2) / 2);
    auto sinlon = glm::sin((lon1-lon2) / 2);
    auto tmp = sinlat*sinlat + glm::cos(lat1) * glm::cos(lat2) * sinlon*sinlon;
    auto d = 2.0 * glm::asin(glm::sqrt(tmp));

    auto A = glm::sin((1-t) * d) / glm::sin(d);
    auto B = glm::sin(t * d) / glm::sin(d);
    auto x = A * glm::cos(lat1) * glm::cos(lon1) + B * glm::cos(lat2) * glm::cos(lon2);
    auto y = A * glm::cos(lat1) * glm::sin(lon1) + B * glm::cos(lat2) * glm::sin(lon2);
    auto z = A * glm::sin(lat1) + B * glm::sin(lat2);
    Coord result;
    result.lat = glm::degrees(glm::atan(z, glm::sqrt(x*x + y*y)));
    result.lon = glm::degrees(glm::atan(y, x));
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

void air::RouteRenderer::start() {
    float points[POINT_COUNT];
    for(int i=0; i<POINT_COUNT; ++i)
        points[i] = i / float(POINT_COUNT-1);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    projectionLoc = glGetUniformLocation(shader.getId(), "projection");
    coordsLoc = glGetUniformLocation(shader.getId(), "coords");
    colorLoc = glGetUniformLocation(shader.getId(), "color");
    dLoc = glGetUniformLocation(shader.getId(), "d");
    widthLoc = glGetUniformLocation(shader.getId(), "width");
    nLoc = glGetUniformLocation(shader.getId(), "n");
}

void air::RouteRenderer::render(const Camera& camera, const Route& r, Coord c1, Coord c2, SDL_Color color) {    
    shader.use();
    
    glUniform4f(colorLoc, color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);

    Coord radianC1 = {glm::radians(c1.lon), glm::radians(c1.lat)};
    Coord radianC2 = {glm::radians(c2.lon), glm::radians(c2.lat)};
    float sinlat = glm::sin((radianC1.lat-radianC2.lat) / 2);
    float sinlon = glm::sin((radianC1.lon-radianC2.lon) / 2);
    float tmp = sinlat*sinlat + glm::cos(radianC1.lat) * glm::cos(radianC2.lat) * sinlon*sinlon;
    float d = 2.0 * glm::asin(glm::sqrt(tmp));
    
    glUniform4f(coordsLoc, radianC1.lon, radianC1.lat, radianC2.lon, radianC2.lat);
    glUniform1f(dLoc, d);
    glUniform1f(widthLoc, camera.getWidth());

    auto projection = camera.projToGLSpaceMatrix();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    float dist = r.lenght / EARTH_RADIUS;
    int n = dist * std::clamp(camera.getZoom(), 2.0f, 20.0f) * 20;
    n += n % 2 + 1;
    glUniform1i(nLoc, n);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, POINT_COUNT);
    glBindVertexArray(0);
}