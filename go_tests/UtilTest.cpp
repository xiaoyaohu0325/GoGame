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

//#define BOOST_TEST_MAIN
//#include <boost/test/unit_test.hpp>
//
//BOOST_AUTO_TEST_CASE(PointConverterTest)
//{
//    BOOST_CHECK_EQUAL(FromSGFLetter("pd"), 336);
//}
