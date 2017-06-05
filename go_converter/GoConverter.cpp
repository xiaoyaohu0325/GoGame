//
//  GoConverter.cpp
//  go_converter
//
//  Created by 王坚 on 17/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//
#include "GoConverter.hpp"
#include "GoLadder.h"
#include "GoEyeUtil.h"
#include "SgBlackWhite.h"
#include "PointConverter.h"

GoConverter::GoConverter(shared_ptr<GoBoard> board) {
    m_board = board;
}

shared_ptr<Plane> GoConverter::zero()
{
    shared_ptr<Plane> p( new Plane() );
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            p->board[i][j] = 0;
        }
    }
    return p;
}

shared_ptr<Plane> GoConverter::one()
{
    shared_ptr<Plane> p( new Plane() );
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            p->board[i][j] = 1;
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

std::vector<shared_ptr<Plane>> GoConverter::Ones(int planes) {
    std::vector<shared_ptr<Plane>> vector;
    
    for (int i = 0; i < planes; i++) {
        vector.push_back(one());
    }
    return vector;
}

std::vector<shared_ptr<Plane>> GoConverter::BoardState()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(3); //0. current 1. opponent 2.empty
    SgBlackWhite currentPlayer = m_board->ToPlay();
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            SgPoint pt = FromRowColumn(i, j);
            if(m_board->IsEmpty(pt)) {
                (vector[0])->board[i][j] = 0;
                (vector[1])->board[i][j] = 0;
                (vector[2])->board[i][j] = 1;
            } else if(m_board->IsColor(pt, currentPlayer)) {
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

std::vector<shared_ptr<Plane>> GoConverter::TurnsSince()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(STONE_MAXIMUM_AGE);
    int moveNum = m_board->MoveNumber();
    int age = 0;
    
    for (int i = moveNum - 1; i >= 0; i--)
    {
        GoPlayerMove move = m_board->Move(i);
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

std::vector<shared_ptr<Plane>> GoConverter::Liberties()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_LIBERTIES);
    
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            SgPoint pt = FromRowColumn(i, j);
            if(!m_board->IsEmpty(pt)) {
                int liberties = m_board->NumLiberties(pt);
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

std::vector<shared_ptr<Plane>> GoConverter::CaptureSize()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_CAPTURE_SIZE);
    
    SgPointSet pointSet = m_board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        int row = SgPointToRow(pt);
        int col = SgPointToCol(pt);
        
        int captured = NumOfCaptured(pt);
        if (captured < MAX_CAPTURE_SIZE) {
            vector[captured]->board[row][col] = 1;
        } else {
            vector[MAX_CAPTURE_SIZE - 1]->board[row][col] = 1;
        }
    }
    
    return vector;
}

int GoConverter::NumOfCaptured(SgPoint pt)
{
    SgBlackWhite player = m_board->ToPlay();
    if(!m_board->IsLegal(pt, player) || !m_board->CanCapture(pt, player)) {
        return 0;
    }
    SgPoint pointSet[5] = {SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT};
    m_board->NeighborBlocks(pt, m_board->Opponent(), 1, pointSet);
    int captured = 0;
    for (int i=0; i<5; i++) {
        if (pointSet[i] == SG_ENDPOINT) {
            break;
        }
        captured += m_board->NumStones(pointSet[i]);
    }
    return captured;
}

std::vector<shared_ptr<Plane>> GoConverter::SelfAtariSize()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_SELF_ATARI_SIZE);
    
    SgPointSet pointSet = m_board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        int numStones = NumOfSelfInAtari(pt);
        if (numStones > 0) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            if (numStones < MAX_SELF_ATARI_SIZE) {
                vector[numStones]->board[row][col] = 1;
            } else {
                vector[MAX_SELF_ATARI_SIZE - 1]->board[row][col] = 1;
            }
        }
    }
    
    return vector;
}

std::vector<shared_ptr<Plane>> GoConverter::LibertiesAfter()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_LIBERTIES_AFTER);
    
    SgPointSet pointSet = m_board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(!m_board->IsLegal(pt, m_board->ToPlay())) {
            continue;
        }
        m_board->TakeSnapshot();
        m_board->Play(pt);
        
        int liberties = m_board->NumLiberties(pt);
        if (liberties > 0) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            if (liberties < MAX_LIBERTIES_AFTER) {
                vector[liberties - 1]->board[row][col] = 1;
            } else {
                vector[MAX_LIBERTIES_AFTER - 1]->board[row][col] = 1;
            }
        }
        
        m_board->RestoreSnapshot();
    }
    
    return vector;
}

int GoConverter::NumOfSelfInAtari(SgPoint pt)
{
    int count = 0;
    // 找出在pt走子后使己方棋子只有一口气的情况
    if (m_board->IsLegal(pt) &&
        (m_board->HasNeighbors(pt, m_board->ToPlay()) ||
        m_board->HasNeighbors(pt, m_board->Opponent()))) {
        m_board->TakeSnapshot();
        m_board->Play(pt);
        if (m_board->NumLiberties(pt) == 1) {
            count = m_board->NumStones(pt);
        }
        m_board->RestoreSnapshot();
    }
    return count;
}

std::vector<shared_ptr<Plane>> GoConverter::LadderCapture()
{
    std::vector<shared_ptr<Plane>> vector = Zeros(2);
    
    SgPointSet pointSet = m_board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    SgBlackWhite currentPlayer = m_board->ToPlay();
    SgBlackWhite opponentPlayer = SgOppBW(currentPlayer);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(!m_board->IsLegal(pt, currentPlayer)) {
            continue;
        }
        // Find neighbors with only one liberty
        SgPoint pointSet[5] = {SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT};
        m_board->NeighborBlocks(pt, currentPlayer, 1, pointSet);
        
        if (pointSet[0] != SG_ENDPOINT) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            m_board->TakeSnapshot();
            m_board->Play(pt);
            
            GoLadder ladder;
            int result = ladder.Ladder(*m_board, pt, opponentPlayer, NULL);
            if (result > 0) { // Good for prey
                vector[1]->board[row][col] = 1;
            } else if (result < 0) { // Good for hunter
                vector[0]->board[row][col] = 1;
            }
            
            m_board->RestoreSnapshot();
        }
    }
    return vector;
}

shared_ptr<Plane> GoConverter::Sensibleness()
{
    shared_ptr<Plane> plane = zero();
    
    SgPointSet pointSet = m_board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    SgBlackWhite currentPlayer = m_board->ToPlay();
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(!m_board->IsLegal(pt, currentPlayer)) {
            continue;
        }
        if (!GoEyeUtil::IsPossibleEye(*m_board, currentPlayer, pt)) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            plane->board[row][col] = 1;
        }
    }
    
    return plane;
}

shared_ptr<Plane> GoConverter::LegalMoves()
{
    shared_ptr<Plane> plane = zero();
    
    SgPointSet pointSet = m_board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    SgBlackWhite currentPlayer = m_board->ToPlay();
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(m_board->IsLegal(pt, currentPlayer)) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            plane->board[row][col] = 1;
        }
    }
    
    return plane;
}
