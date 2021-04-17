#ifndef ALLEGRO_HPP
#define ALLEGRO_HPP
#include "backend.hpp"

class Allegro : public Backend {
public:
    void run(Game &game) override;
};
#endif
