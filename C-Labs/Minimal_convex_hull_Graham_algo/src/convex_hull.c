#define _GNU_SOURCE // for qsort_r()
#include <stdlib.h>

#include "convex_hull.h"

static size_t findStartingPoint(Vec2i_t *points, size_t pointsCount) {
    size_t starting = 0;
    for (size_t i = 0; i < pointsCount; ++i) {
        if (points[i].y < points[starting].y) {
            starting = i;
        }
        else if (points[i].y == points[starting].y) {
            if (points[i].x < points[starting].x) {
                starting = i;
            }
        }
    }
    return starting;
}

static void swapVec2i(Vec2i_t *a, Vec2i_t *b) {
    Vec2i_t temp = *a;
    *a = *b;
    *b = temp;
}

static int cmp(const void *a, const void *b, void *origin) {
    Vec2i_t *a_vec = (Vec2i_t*)a;
    Vec2i_t *b_vec = (Vec2i_t*)b;
    Vec2i_t *origin_vec = (Vec2i_t*)origin;

    double oa_x = (double)a_vec->x - origin_vec->x;
    double oa_y = (double)a_vec->y - origin_vec->y;
    double ob_x = (double)b_vec->x - origin_vec->x;
    double ob_y = (double)b_vec->y - origin_vec->y;

    double prod = oa_x * ob_y - oa_y * ob_x;
    if (prod > 0) {
        return -1;
    }
    else if (prod < 0) {
        return 1;
    }
    else {
        double oa_len = oa_x * oa_x + oa_y * oa_y;
        double ob_len = ob_x * ob_x + ob_y * ob_y;
        if (oa_len < ob_len) {
            return -1;
        }
        else if (oa_len > ob_len) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

static double checkRotation(Vec2i_t o, Vec2i_t a, Vec2i_t b) {
    double oa_x = (double)a.x - o.x;
    double oa_y = (double)a.y - o.y;
    double ab_x = (double)b.x - a.x;
    double ab_y = (double)b.y - a.y;

    return oa_x * ab_y - oa_y * ab_x;
}

size_t findConvexHull(Vec2i_t *points, size_t pointsCount) {
    if (pointsCount < 2) {
        return pointsCount;
    }

    size_t starting = findStartingPoint(points, pointsCount);
    // move starting point to the beginning
    swapVec2i(&points[0], &points[starting]);

    // sort points by angle relatively to starting point
    qsort_r(&points[1], pointsCount - 1, sizeof(Vec2i_t), &cmp, &points[0]);

    size_t stackTop = 1;
    for (size_t i = 2; i < pointsCount; ++i) {
        Vec2i_t a = points[stackTop - 1];
        Vec2i_t b = points[stackTop];
        Vec2i_t c = points[i];
        while (stackTop > 0 && checkRotation(a, b, c) <= 0) {
            --stackTop;
            if (stackTop == 0) {
                a = points[stackTop];
                b = points[i];
            }
            else {
                a = points[stackTop - 1];
                b = points[stackTop];
            }
        }
        ++stackTop;
        points[stackTop] = points[i];
    }

    return stackTop + 1;
}
