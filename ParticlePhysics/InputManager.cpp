#include "InputManager.h"
#include <Utility.h>
#include "Particles/SandParticle.h"

void InputManager::handle_input() {
    add_particle();
}

void InputManager::add_particle() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        for (int i = 0; i < particle_multipler; i++) {
            int gridX = mousePos.x / cellSize;
            int gridY = mousePos.y / cellSize;
            gridX += Utility::random_int(-particle_spread, particle_spread);
            gridY += Utility::random_int(-particle_spread, particle_spread);
            if (matrix.get_cell(gridX, gridY) == nullptr) {
                matrix.set_cell(gridX, gridY, new SandParticle());
                matrix.particles++;
                matrix.wake_chunks(gridX, gridY);
            }
        }
    }
}
