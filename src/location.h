#ifndef LOCATION_H
#define LOCATION_H

#include <cassert>

struct Location {
    int32_t row, col, end_row, end_col;

    Location(int32_t row, int32_t col,
             int32_t end_row, int32_t end_col)
        : row(row)
        , col(col)
        , end_row(end_row)
        , end_col(end_col) {}

    Location(Location l, Location r)
        : Location(l.row, l.col, r.end_row, r.end_col) {}

    Location(int32_t row, int32_t col)
        : Location(row, col, row, col) {}

    Location(): Location(-1, -1, -1, -1) {}
};

#endif // LOCATION_H
