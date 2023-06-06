#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include <stdlib.h>

typedef struct Vec2i {
    int x;
    int y;
} Vec2i_t;

// returns convex hull size
size_t findConvexHull(Vec2i_t *points, size_t pointsCount);

#endif
