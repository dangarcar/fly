#pragma once

#include <filesystem>

class ShaderProgram {
public:  
    ShaderProgram(std::filesystem::path vertexPath, std::filesystem::path fragmentPath);

    void use() const;

    void setBool(const std::string& name, bool value) const;  
    void setInt(const std::string& name, int value) const;   
    void setFloat(const std::string& name, float value) const;

    unsigned getId() const { return id; }

private:
    unsigned id;
};
