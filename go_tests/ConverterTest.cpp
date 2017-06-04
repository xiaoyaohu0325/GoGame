//
//  ConverterTest.cpp
//  go_converter
//
//  Created by 王坚 on 03/06/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#include <stdio.h>

#include <gtest/gtest.h>
#include "PointConverter.h"
#include "GoBoard.h"
#include "SgfExporter.hpp"
#include "GoConverter.hpp"

class ConverterTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        std::string sgf("/Users/wangjian/WORK_ROOT/go_converter/go_converter/go_converter/data/2016-01-01-1.sgf");
        exporter = new SgfExporter(sgf);
        board = exporter->Board();
    }
    
    virtual void TearDown() {
        delete exporter;
    }
    
    // virtual void TearDown() {}
    SgfExporter* exporter;
    GoBoard* board;
    GoConverter converter;
};

TEST_F(ConverterTest, CapturedSize) {
    EXPECT_TRUE(board != NULL);
    board->TakeSnapshot();
    
    // try moving
    EXPECT_EQ(board->ToPlay(), SG_BLACK);
    
    EXPECT_EQ(converter.NumOfCaptured(*board, FromRowColumn(1, 13)), 1);
    
    board->Play(FromRowColumn(0, 0));
    
    EXPECT_EQ(board->ToPlay(), SG_WHITE);
    
    EXPECT_EQ(converter.NumOfCaptured(*board, FromRowColumn(6, 3)), 1);
    EXPECT_EQ(converter.NumOfCaptured(*board, FromRowColumn(7, 4)), 4);
    
    board->RestoreSnapshot();
}

TEST_F(ConverterTest, SelfAtari) {
    EXPECT_TRUE(board != NULL);
    board->TakeSnapshot();
    
    // try moving
    EXPECT_EQ(board->ToPlay(), SG_BLACK);
    EXPECT_EQ(converter.NumOfSelfInAtari(*board, FromRowColumn(0, 6)), 2);
    EXPECT_EQ(converter.NumOfSelfInAtari(*board, FromRowColumn(0, 8)), 2);
    
    board->Play(FromRowColumn(0, 0));
    
    // white to play
    EXPECT_EQ(board->ToPlay(), SG_WHITE);
    
    EXPECT_EQ(converter.NumOfSelfInAtari(*board, FromRowColumn(1, 13)), 2);
    
    board->RestoreSnapshot();
}
