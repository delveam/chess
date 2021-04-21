#ifndef ALLEGRO_HPP
#define ALLEGRO_HPP
#include "dam/backend.hpp"

class Allegro : public dam::Backend {
public:
    void run(dam::App& app) override;
};
#endif
