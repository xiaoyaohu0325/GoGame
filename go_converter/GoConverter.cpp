//
//  GoConverter.cpp
//  go_converter
//
//  Created by 王坚 on 17/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#include "GoConverter.hpp"
#include "SgBlackWhite.h"
#include "PointConverter.h"

GoConverter::GoConverter() {
    
}

shared_ptr<Plane> GoConverter::zero()
{
    shared_ptr<Plane> p( new Plane() );
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            (*p).board[i][j] = 0;
        }
    }
    return p;
}

std::vector<shared_ptr<Plane>> GoConverter::Zeros(int planes) {
    std::vector<shared_ptr<Plane>> vector;
    
    for (int i = 0; i < planes; i++) {
        vector.push_back(zero());
    }
    return vector;
}

std::vector<shared_ptr<Plane>> GoConverter::BoardState(GoBoard& board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(3); //0. current 1. opponent 2.empty
    SgBlackWhite currentPlayer = board.ToPlay();
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            SgPoint pt = FromRowColumn(i, j);
            if(board.IsEmpty(pt)) {
                (vector[0])->board[i][j] = 0;
                (vector[1])->board[i][j] = 0;
                (vector[2])->board[i][j] = 1;
            } else if(board.IsColor(pt, currentPlayer)) {
                (vector[0])->board[i][j] = 1;
                (vector[1])->board[i][j] = 0;
                (vector[2])->board[i][j] = 0;
            } else {
                (vector[0])->board[i][j] = 0;
                (vector[1])->board[i][j] = 1;
                (vector[2])->board[i][j] = 0;
            }
        }
    }
    
    return vector;
}

std::vector<shared_ptr<Plane>> GoConverter::TurnsSince(GoBoard &board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(STONE_MAXIMUM_AGE);
    int moveNum = board.MoveNumber();
    int age = 0;
    
    for (int i = moveNum - 1; i >= 0; i--)
    {
        GoPlayerMove move = board.Move(i);
        SgPoint pt = move.Point();
        int row = SgPointToRow(pt);
        int col = SgPointToCol(pt);
        
        if (age < STONE_MAXIMUM_AGE) {
            vector[age]->board[row][col] = 1;
        } else {
            vector[STONE_MAXIMUM_AGE - 1]->board[row][col] = 1;
        }
        age++;
    }
    return vector;
}

std::vector<shared_ptr<Plane>> GoConverter::Liberties(GoBoard &board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_LIBERTIES);
    
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            SgPoint pt = FromRowColumn(i, j);
            if(!board.IsEmpty(pt)) {
                int liberties = board.NumLiberties(pt);
                if (liberties >= MAX_LIBERTIES) {
                    vector[MAX_LIBERTIES - 1]->board[i][j] = 1;
                } else {
                    vector[liberties - 1]->board[i][j] = 1;
                }
            }
        }
    }

    return vector;
}

std::vector<shared_ptr<Plane>> GoConverter::CaptureSize(GoBoard &board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_CAPTURE_SIZE);
    
    SgPointSet pointSet = board.AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        int captured = NumOfCaptured(board, pt);
        if (captured >= MAX_CAPTURE_SIZE - 1) {
            captured = MAX_CAPTURE_SIZE - 1;
        }
        int row = SgPointToRow(pt);
        int col = SgPointToCol(pt);
        vector[captured]->board[row][col] = 1;
    }
    
    return vector;
}

int GoConverter::NumOfCaptured(GoBoard& board, SgPoint pt)
{
    SgBlackWhite player = board.ToPlay();
    if(!board.IsLegal(pt, player) || !board.CanCapture(pt, player)) {
        return 0;
    }
    SgPoint pointSet[5] = {SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT};
    board.NeighborBlocks(pt, board.Opponent(), 1, pointSet);
    int captured = 0;
    for (int i=0; i<5; i++) {
        if (pointSet[i] == SG_ENDPOINT) {
            break;
        }
        captured += board.NumStones(pointSet[i]);
    }
    return captured;
}
