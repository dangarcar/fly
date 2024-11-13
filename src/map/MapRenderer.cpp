#include "MapRenderer.hpp"

#include <fstream>
#include <vector>
#include <json/json.hpp>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "../game/Camera.hpp"

void MapRenderer::load(
    const std::vector<float>& projectedVertices, 
    const std::vector<int32_t>& triangles, 
    const std::vector<Polygon>& polys, 
    const std::unordered_map<std::string, Country>& countries
) {
    //LOAD LABELS
    using json = nlohmann::json;

    std::ifstream labelFile(LABELS_DATA_FILE);
    json labelData = json::parse(labelFile);

    for(auto& [k, v]: labelData.items()) {
        Label label;
        label.size = v["size"].template get<float>() * 128.0f; //128 was the default size when this file was made
        label.angle = v["angle"].template get<float>();
        label.name = v["name"].template get<std::string>();
        auto center = v["coord"].template get<std::vector<float>>();
        label.coord = {center[0], center[1]};

        labels[k] = label;
    }


    //LOAD VERTICES
    for(auto& [k, v]: countries) {
        auto [beg, end] = v.meshIndex;

        for(int j=beg; j<end; ++j) {
            auto [begVert, endVert] = polys[j].vertexIndex;        
            std::vector<float> vertices(projectedVertices.begin() + 2*begVert, projectedVertices.begin() + 2*endVert);

            auto [begTri, endTri] = polys[j].triangleIndex;
            std::vector<int32_t> indices(triangles.begin() + 3*begTri, triangles.begin() + 3*endTri);
            for(int i=0; i<int(indices.size()); ++i)
                indices[i] -= begVert;

            CountryRender rend;
            rend.country = k;
            rend.elements = indices.size();
            rend.vertices = vertices.size() / 2;

            glGenVertexArrays(1, &rend.VAO);
            glBindVertexArray(rend.VAO);

            glGenBuffers(1, &rend.VBO);
            glBindBuffer(GL_ARRAY_BUFFER, rend.VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &rend.EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rend.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), &indices[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            renders.push_back(rend);
        }
    }

    //GET UNIFORMS
    projectionLoc = glGetUniformLocation(shader.getId(), "projection");
    colorLoc = glGetUniformLocation(shader.getId(), "color");
}

void MapRenderer::render(const Camera& camera, const std::unordered_map<std::string, SDL_Color>& countryColors) const {
    //RENDER VERTICES
    shader.use();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera.projToGLSpaceMatrix()));
    for(int i=0; i<int(renders.size()); ++i) {
        auto r = renders[i];
        
        auto color = countryColors.at(r.country);
        glUniform4f(colorLoc, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);

        glBindVertexArray(r.VAO);
        glDrawElements(GL_TRIANGLES, r.elements, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //RENDER LINES
    glUniform4f(colorLoc, 0, 0, 0, 1);
    for(int i=0; i<int(renders.size()); ++i) {
        auto r = renders[i];
        glBindVertexArray(r.VAO);
        glDrawArrays(GL_LINE_STRIP, 0, r.vertices);
        glBindVertexArray(0);
    }

    //RENDER LABELS
    glBlendFunc(GL_ONE, GL_ONE); // Additive
    for(auto& [k, label]: labels) {
        auto sz = label.size * camera.getZoom();

        auto v = camera.coordsToScreen(label.coord);
        auto rect = camera.getTextBounds(label.name, sz);
        auto centre = glm::vec2(rect.w/2, rect.h/2);
        auto dv = glm::rotate(centre, glm::radians(label.angle));
        camera.renderTextRotated(label.name, int(v.x - dv.x), int(v.y - dv.y), sz, label.angle, {100, 100, 100, 100});
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Interpolative
}