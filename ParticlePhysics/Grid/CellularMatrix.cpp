#include "CellularMatrix.h"
#include "Utility.h"
#include <thread>
#include <future>
#include <ThreadPool.h>

#define CHUNK_SIZE 32
#define NBR_OF_CHUNKS 25  // pow2

CellularMatrix::CellularMatrix(int width, int height)
	: width(width), height(height), threadPool(std::thread::hardware_concurrency()) {

	cells.resize(height);
	for (int row = 0; row < height; ++row) {
		cells[row].resize(width, nullptr);
	}

    int chunk_pixels = 32;
    int chunks_x = 25;
    int chunks_y = 25;   // 25 x 25 chunks of 32 pixels = 800x800 windows

    chunks.resize(chunks_y);
    for (int cy = 0; cy < chunks_y; ++cy) {
        chunks[cy].reserve(chunks_x);
        for (int cx = 0; cx < chunks_x; ++cx) {
            int chunk_x_pos = cx * chunk_pixels;
            int chunk_y_pos = cy * chunk_pixels;
            chunks[cy].emplace_back(chunk_x_pos, chunk_y_pos, CHUNK_SIZE);
        }
    }
}

void CellularMatrix::display_matrix(sf::RenderWindow& window, int CELL_SIZE) {
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);

    // Reserve max vertices (6 per particle)
    vertices.resize(width * height * 6);

    int vertexCount = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Particle* p = get_cell(x, y);
            if (p != nullptr) {
                float xPos = static_cast<float>(x * CELL_SIZE);
                float yPos = static_cast<float>(y * CELL_SIZE);
                sf::Color color = p->color;

                // Triangle 1
                vertices[vertexCount + 0].position = sf::Vector2f(xPos, yPos);
                vertices[vertexCount + 0].color = color;

                vertices[vertexCount + 1].position = sf::Vector2f(xPos + CELL_SIZE, yPos);
                vertices[vertexCount + 1].color = color;

                vertices[vertexCount + 2].position = sf::Vector2f(xPos + CELL_SIZE, yPos + CELL_SIZE);
                vertices[vertexCount + 2].color = color;

                // Triangle 2
                vertices[vertexCount + 3].position = sf::Vector2f(xPos, yPos);
                vertices[vertexCount + 3].color = color;

                vertices[vertexCount + 4].position = sf::Vector2f(xPos + CELL_SIZE, yPos + CELL_SIZE);
                vertices[vertexCount + 4].color = color;

                vertices[vertexCount + 5].position = sf::Vector2f(xPos, yPos + CELL_SIZE);
                vertices[vertexCount + 5].color = color;

                vertexCount += 6;
            }
        }
    }

    vertices.resize(vertexCount);
    window.draw(vertices);
}

void CellularMatrix::set_cell(int x, int y, Particle* particle) {
	if(x >= 0 && x < width && y >= 0 && y < height)
		cells[y][x] = particle;
}

Particle* CellularMatrix::get_cell(int x, int y) {
	if (x >= 0 && x < width && y >= 0 && y < height)
		return cells[y][x];
	return nullptr;
}

void reset_particles(CellularMatrix& matrix) {
	int height = matrix.height;
	int width = matrix.width;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Particle* current = matrix.cells[x][y];
			if (current != nullptr) {
				current->already_processed = false;
			}
		}
	}
}

void CellularMatrix::update_all_cells() {
    reset_particles(*this);

    // Step 1: Process the bottom row alone to ensure all particles are handled
    bool leftToRight = Utility::random_bool();
    if (leftToRight) {
        for (int x = 0; x < width; ++x) {
            update_cell(x, height - 1);
        }
    }
    else {
        for (int x = width - 1; x >= 0; --x) {
            update_cell(x, height - 1);
        }
    }

    // Step 2: Multithreaded processing using chunks
    const int chunk_rows = static_cast<int>(chunks.size());
    const int chunk_cols = static_cast<int>(chunks[0].size());

    auto update_chunk_region = [&](int chunk_y, int chunk_x) {
        Chunk& chunk = chunks[chunk_y][chunk_x];
        if (!chunk.step) return;

        int start_y = chunk_y * CHUNK_SIZE;
        int end_y = std::min(height, start_y + CHUNK_SIZE);

        int start_x = chunk_x * CHUNK_SIZE;
        int end_x = std::min(width, start_x + CHUNK_SIZE);

        for (int y = end_y - 1; y >= start_y; --y) {
            bool leftToRight = Utility::random_bool();
            if (leftToRight) {
                for (int x = start_x; x < end_x; ++x) {
                    update_cell(x, y);
                }
            }
            else {
                for (int x = end_x - 1; x >= start_x; --x) {
                    update_cell(x, y);
                }
            }
        }
        };

    for (int cy = 0; cy < chunk_rows; ++cy) {
        for (int cx = 0; cx < chunk_cols; ++cx) {
            threadPool.enqueue([=]() {
                update_chunk_region(cy, cx);
                });
        }
    }
    threadPool.wait_all();
    cycle_chunks();
}

Chunk* CellularMatrix::get_chunk(int x, int y) {
    int chunk_x = x / CHUNK_SIZE;
    int chunk_y = y / CHUNK_SIZE;
    if (chunk_y < 0 || chunk_y >= (int)chunks.size() || chunk_x < 0 || chunk_x >= (int)chunks[0].size()) {
        return nullptr;
    }
    return &chunks[chunk_y][chunk_x];
}

void CellularMatrix::wake_chunks(int x, int y) {
    Chunk* chunk = get_chunk(x, y);  // x,y in cells

    // Compute chunk coords once
    int chunk_x = x / CHUNK_SIZE;
    int chunk_y = y / CHUNK_SIZE;

    chunk->step_next_frame = true;

    Chunk* chunk_left = (chunk_x > 0) ? &chunks[chunk_y][chunk_x - 1] : nullptr;
    Chunk* chunk_right = (chunk_x < 25 - 1) ? &chunks[chunk_y][chunk_x + 1] : nullptr;
    Chunk* chunk_top = (chunk_y > 0) ? &chunks[chunk_y - 1][chunk_x] : nullptr;
    Chunk* chunk_bottom = (chunk_y < 25 - 1) ? &chunks[chunk_y + 1][chunk_x] : nullptr;

    if (chunk_left) chunk_left->step_next_frame = true;
    if (chunk_right) chunk_right->step_next_frame = true;
    if (chunk_top) chunk_top->step_next_frame = true;
    if (chunk_bottom) chunk_bottom->step_next_frame = true;
}

void CellularMatrix::update_cell(int x, int y) {
	Particle* p = get_cell(x, y);
	if (p != nullptr && !p->already_processed) {
		switch (p->type) {
		case ParticleType::Solid:
			update_solid_cell(x, y);
			break;
		}
	}
}

bool CellularMatrix::is_cell_empty(int x, int y) {
	return x >= 0 && x < width && y >= 0 && y < height && cells[y][x] == nullptr;
}

void CellularMatrix::update_solid_cell(int x, int y) {
    Particle* current = cells[y][x];
    Chunk* chunk = get_chunk(x, y);
    if (current != nullptr && !current->already_processed) {
        current->velocity.y += 0.1f;

        int fallDistance = static_cast<int>(std::floor(current->velocity.y));
        int currentY = y;
        int currentX = x;

        for (int i = 0; i < fallDistance; ++i) {
            int nextY = currentY + 1;
            if (nextY >= height) {
                current->velocity.y = 1;
                break;
            }

            bool canMoveDown = (cells[nextY][currentX] == nullptr);
            if (canMoveDown) {
                cells[nextY][currentX] = current;
                cells[currentY][currentX] = nullptr;
                currentY = nextY;
                current->already_processed = true;
                current->immobile_frames = 0;
                //current->color = current->base_color;
                wake_chunks(currentX, currentY);
            }
            else {
                Particle* blocking_particle = cells[nextY][currentX];
                if (blocking_particle != nullptr && blocking_particle->velocity.y < current->velocity.y) {
                    float avg_velocity = (blocking_particle->velocity.y + current->velocity.y) / 2;
                    blocking_particle->velocity.y = avg_velocity;
                    current->velocity.y = avg_velocity;
                }

                const int maxSlide = 2;
                bool movedSideways = false;

                for (int slideDist = 1; slideDist <= maxSlide && !movedSideways; ++slideDist) {
                    int tryX = currentX - slideDist;
                    if (tryX >= 0 && cells[nextY][tryX] == nullptr) {
                        cells[nextY][tryX] = current;
                        cells[currentY][currentX] = nullptr;
                        currentX = tryX;
                        currentY = nextY;
                        current->already_processed = true;
                        current->immobile_frames = 0;
                        //current->color = current->base_color;
                        movedSideways = true;
                        wake_chunks(currentX, currentY);
                        break;
                    }
                    tryX = currentX + slideDist;
                    if (tryX < width && cells[nextY][tryX] == nullptr) {
                        cells[nextY][tryX] = current;
                        cells[currentY][currentX] = nullptr;
                        currentX = tryX;
                        currentY = nextY;
                        current->already_processed = true;
                        current->immobile_frames = 0;
                        //current->color = current->base_color;
                        movedSideways = true;
                        wake_chunks(currentX, currentY);
                        break;
                    }
                }

                if (!movedSideways) {
                    current->velocity.y = 1;
                    break;
                }
            }
        }
	}
}

void CellularMatrix::cycle_chunks() {
    for (int i = 0; i < NBR_OF_CHUNKS; i++) {
        for (int j = 0; j < NBR_OF_CHUNKS; j++) {
            chunks[i][j].step = chunks[i][j].step_next_frame;
            chunks[i][j].step_next_frame = false;
        }
    }
}

void CellularMatrix::display_chunk_debug(sf::RenderWindow& window, int CELL_SIZE) {
    for (int cy = 0; cy < chunks.size(); ++cy) {
        for (int cx = 0; cx < chunks[cy].size(); ++cx) {
            if (chunks[cy][cx].step) {
                float x = cx * CHUNK_SIZE * CELL_SIZE;
                float y = cy * CHUNK_SIZE * CELL_SIZE;

                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(CHUNK_SIZE * CELL_SIZE, CHUNK_SIZE * CELL_SIZE));
                sf::Vector2f pos{ x, y };
                rect.setPosition(pos);
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::Green);
                rect.setOutlineThickness(1.0f);

                window.draw(rect);
            }
        }
    }
}
