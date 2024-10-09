#pragma once

#include "../Dialog.hpp"

namespace air {
    class AirportData;
};
class City;

class AirportDialog: public Dialog {
public:
    AirportDialog(air::AirportData& airport, const City& city, Player& player, const std::vector<City>& cities);

    bool handleInput(const InputEvent& event) override;
    void render(const Camera& camera) override;

private:
    std::vector<std::pair<std::string, int>> getFrequentDestinations() const;
    bool canUpgrade() const;

private:
    air::AirportData& airport;
    const City& city;
    Player& player;

    const std::vector<City>& cities;

    Button upgradeButton;
};