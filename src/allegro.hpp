#ifndef ALLEGRO_HPP
#define ALLEGRO_HPP
#include "backend.hpp"

class Allegro : public dam::Backend {
public:
    void run(dam::Game &game) override;
};
#endif
