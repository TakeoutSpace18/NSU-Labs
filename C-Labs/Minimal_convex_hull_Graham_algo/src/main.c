#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>

#include "error_handling.h"
#include "convex_hull.h"

bool validatePointsCount(size_t pointsCount) {
    return pointsCount <= 100000;
}

int main(void) {
    exitCode_t exitCode = SUCCESS;

    Vec2i_t *points = NULL;
    FILE *input = fopen("in.txt", "r");
    FILE *output = fopen("out.txt", "w");
    if (!input || !output) {
        return_defer(IO_ERROR);
    }

    size_t pointsCount;
    if (fscanf(input, "%zu", &pointsCount) != 1) {
        return_defer(BAD_NUMBER_OF_LINES);
    };
    if (!validatePointsCount(pointsCount)) {
        return_defer(BAD_NUMBER_OF_POINTS);
    }

    points = malloc(sizeof(Vec2i_t) * pointsCount);
    if (!points) {
        log_error("malloc() failed");
        return_defer(ALLOCATION_ERROR);
    }

    for (size_t i = 0; i < pointsCount; ++i) {
        if (fscanf(input, "%i%i", &points[i].x, &points[i].y) != 2) {
            return_defer(BAD_NUMBER_OF_LINES);
        }
    }

    size_t convexHullSize = findConvexHull(points, pointsCount);
    (void)convexHullSize;
    for (size_t i = 0; i < convexHullSize; ++i) {
        fprintf(output, "%i %i\n", points[i].x, points[i].y);
    }

defer:
    printExitCodeDescription(output, exitCode);
    free(points);
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }
    // return exitCode;
    return SUCCESS;
}
