#include "Airport.hpp"

#include <algorithm>

#include "../game/Camera.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>
#include <vector>

float air::getRelativeRadius(float radius, float zoom) {
    return radius * std::clamp(zoom, 2.0f, 12.0f);
}

void air::AirportRenderer::start(const std::unordered_map<std::string, Country>& countries) {
    //OPENGL CREATION
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    projectionLoc = glGetUniformLocation(shader.getId(), "projection");


    //SPRITE SHEET LOADING
    glGenTextures(1, &sheet.texture);
    glBindTexture(GL_TEXTURE_2D, sheet.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char *data = stbi_load(SPRITESHEET_SRC.string().c_str(), &sheet.width, &sheet.height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sheet.width, sheet.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        writeError("Failed to load texture\n");
    }
    stbi_image_free(data);


    //TEXTURE MAPPING
    std::vector<std::string> names;
    for(auto& [k, _]: countries) {
        names.push_back(k);
    }
    std::sort(names.begin(), names.end());

    const int tileSize = 512;
    float x=0, y=0;
    for(auto& name: names) {
        textureCoords[name] = glm::vec2(x/sheet.width, y/sheet.height);

        x += tileSize;
        if(x >= sheet.width) {
            x = 0;
            y += tileSize;
        }
    }

    this->blackCoords = glm::vec2(x/sheet.width, y/sheet.height);
}

void air::AirportRenderer::render(const Camera& camera, const std::vector<AirportData>& airports, const std::vector<City>& cities) const {
    if(airports.empty())
        return;
    
    const glm::vec2 tile(512.0f/sheet.width, 512.0f/sheet.height);

    const int mulVertices[] = {
        1,1,1,1,
        1,-1,1,0,
        -1,-1,0,0,
        -1,1,0,1
    };

    const unsigned mulIndices[] = {0, 1, 3, 1, 2, 3 };

    std::vector<float> vertices(cities.size() * 32);
    std::vector<unsigned> indices(cities.size() * 12);

    for(int i=0; i<int(cities.size()); ++i) {
        renderInfo(camera, airports[i], cities[i]);

        auto tex = textureCoords.at(cities[i].country);
        auto pos = cities[i].proj;

        auto rad = getRelativeRadius(airports[i].radius, camera.getZoom()) / camera.getZoom();
        auto biggerRad = rad + 2/camera.getZoom();

        for(int j=0; j<4; ++j) {
            vertices[32*i + 16 + 4*j] = mulVertices[4*j]*rad + pos.x;
            vertices[32*i + 16 + 4*j+1] = mulVertices[4*j+1]*rad + pos.y;
            vertices[32*i + 16 + 4*j+2] = mulVertices[4*j+2]*tile.x + tex.x;
            vertices[32*i + 16 + 4*j+3] = mulVertices[4*j+3]*tile.y + tex.y;

            vertices[32*i + 4*j] = mulVertices[4*j]*biggerRad + pos.x;
            vertices[32*i + 4*j+1] = mulVertices[4*j+1]*biggerRad + pos.y;
            vertices[32*i + 4*j+2] = mulVertices[4*j+2]*tile.x + blackCoords.x;
            vertices[32*i + 4*j+3] = mulVertices[4*j+3]*tile.y + blackCoords.y;
        }

        for(int j=0; j<6; ++j) {
            indices[12*i + j] = mulIndices[j] + 8*i;
            indices[12*i + 6 + j] = mulIndices[j] + 8*i + 4;
        }
    }
    
    shader.use();
    
    auto projection = camera.projToGLSpaceMatrix();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindTexture(GL_TEXTURE_2D, sheet.texture);
    glBindVertexArray(VAO);
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}

void air::AirportRenderer::renderInfo(const Camera& camera, const AirportData& airport, const City& city) const {
    float fillPercentage = std::min(1.0f, float(airport.waiting.size()) / AIRPORT_CAPACITY_PER_LEVEL[airport.level]);
    auto color = FULL_GRADIENT.getColor(fillPercentage);

    auto pos = camera.projToScreen(city.proj);
    if(pos.x < 0 || pos.y < 0 || pos.x > camera.getWidth() || pos.y > camera.getHeight())
        return;

    auto rad = getRelativeRadius(airport.radius, camera.getZoom());
    SDL_FRect clip = {pos.x - rad, pos.y - rad, 2.0f*rad, 2.0f*rad};

    if((camera.getZoom() > 4 && airport.radius >= 20)
    || (camera.getZoom() > 6 && airport.radius >= 16)
    || camera.getZoom() > 8) {
        //DRAW NAME
        camera.renderText(city.name, clip.x + rad, clip.y - rad*1.3, rad*1.3, Aligment::CENTER, color);

        //DRAW BAR
        auto bar = SDL_FRect { clip.x, clip.y + clip.h + 4, clip.w, clip.h/5 };
        camera.getRenderer().fillRect(bar, SDL_BLACK, 0.0f);

        bar = SDL_FRect { bar.x + 1, bar.y + 1, (bar.w - 2) * fillPercentage, bar.h - 2 };
        camera.getRenderer().fillRect(bar, color, 0.0f);
    }
}
