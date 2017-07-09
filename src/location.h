#pragma once
#include <cassert>

struct Loc {
    int32_t row, col, end_row, end_col;

    Loc(int32_t row, int32_t col,
             int32_t end_row, int32_t end_col)
        : row(row)
        , col(col)
        , end_row(end_row)
        , end_col(end_col) {}

    Loc(Loc l, Loc r)
        : Loc(l.row, l.col, r.end_row, r.end_col) {}

    Loc(int32_t row, int32_t col)
        : Loc(row, col, row, col) {}

    Loc(): Loc(-1, -1, -1, -1) {}
};
