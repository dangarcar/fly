#pragma once

#include "../Dialog.hpp"

class AirportData;
class City;

class AirportDialog: public Dialog {
public:
    AirportDialog(AirportData& airport, const City& city, Player& player, const std::vector<City>& cities);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    std::vector<std::pair<std::string, int>> getFrequentDestinations() const;
    bool canUpgrade() const;

private:
    AirportData& airport;
    const City& city;
    Player& player;

    const std::vector<City>& cities;

    Button upgradeButton;
};