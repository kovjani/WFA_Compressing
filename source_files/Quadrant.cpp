#include "../header_files/Quadrant.h"

#include "../header_files/NondeterministicCoding.h"

Quadrant::Quadrant(int level, int quadtree_index, double brightness) {
    this->level = level;
    this->quadtree_index = quadtree_index;
    this->brightness = brightness;
}