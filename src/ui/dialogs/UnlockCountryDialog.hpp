#pragma once

#include "../Dialog.hpp"

class UnlockCountryDialog: public Dialog {
public:
    UnlockCountryDialog(const std::string& name, const std::string& code, Map& map, Player& player);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    Map& map;
    Player& player;
    std::string countryName, countryCode;

    Button yesButton, noButton;
};