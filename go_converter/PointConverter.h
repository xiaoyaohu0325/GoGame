//
//  PointConverter.h
//  go_converter
//
//  Created by 王坚 on 12/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#ifndef PointConverter_h
#define PointConverter_h
#include "SgSystem.h"
#include "SgPoint.h"
#include "GoBoard.h"

inline SgPoint FromRowColumn(int row, int column) {
    //fuego smart game starts from 1
    //2 col -> col, row -> size - row + 1
    if (row < 0 || row > 18 || column < 0 || column > 18) {
        return SG_PASS;
    }
    return SgPointUtil::Rotate(2, SgPointUtil::Pt(column + 1, row + 1), 19);
}

/**
 * Convert sgf move coordinate to SgPoint
 */
inline SgPoint FromSGFLetter(const std::string& letter) {
    // sgf coordinate starts from 'a', fuego smart game starts from 1
    int column = letter[0] - 'a';
    int row = letter[1] - 'a';
    //2 col -> col, row -> size - row + 1
    return FromRowColumn(row, column);
}

/**
 Convert SgPoint to row number which starts from 0 and uses top-left as origin.
 */
inline int SgPointToRow(const SgPoint pt) {
    SgGrid row = SgPointUtil::Row(pt);
    return GO_DEFAULT_SIZE - row;
}

/**
 Convert SgPoint to column number which starts from 0 and uses top-left as origin.
 */
inline int SgPointToCol(const SgPoint pt) {
    SgGrid col = SgPointUtil::Col(pt);
    return col - 1;
}

#endif /* PointConverter_h */
