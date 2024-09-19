#pragma once

namespace Event { class EventManager; };
class Camera;

class Player {
private:
    //static constexpr long INITIAL_CASH = 1000;
    static constexpr long INITIAL_CASH = 1000000000;

public:
    void registerEvents(Event::EventManager& manager);
    void render(const Camera& camera);

    long getCash() const { return cash; }
    void spend(long amount) { cash -= amount; }
    void earn(long amount) { cash += amount; }

private:
    long cash = INITIAL_CASH;
};
