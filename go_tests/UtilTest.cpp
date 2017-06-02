//
//  UtilTest.cpp
//  go_converter
//
//  Created by 王坚 on 12/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#include <gtest/gtest.h>
#include "PointConverter.h"
#include "SgPoint.h"

TEST(PointConverterTest, convert)
{
    EXPECT_EQ( FromSGFLetter("pd"), 336 );
    int row = SgPointToRow(336); // row and col starts from 1, left-bottom as origin
    int col = SgPointToCol(336);
    EXPECT_EQ(col, 'p' - 'a'); // 15
    EXPECT_EQ(row, 'd' - 'a'); // 3
}
