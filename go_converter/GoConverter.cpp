//
//  GoConverter.cpp
//  go_converter
//
//  Created by 王坚 on 17/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//
#include <hdf5.h>
#include <hdf5_hl.h>
#include <iostream>

#include "GoConverter.hpp"
#include "GoLadder.h"
#include "GoEyeUtil.h"
#include "SgBlackWhite.h"
#include "PointConverter.h"
#include "SgfExporter.hpp"

GoConverter::GoConverter() {
    m_featureList = std::vector<std::string>{
        "board",
        "ones",
        "turns_since",
        "liberties",
        "capture_size",
        "self_atari_size",
        "liberties_after",
        "ladder_capture",
        "ladder_escape",
        "sensibleness",
        "zeros"
    };
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

std::vector<shared_ptr<Plane>> GoConverter::BoardState(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(3); //0. current 1. opponent 2.empty
    SgBlackWhite currentPlayer = board->ToPlay();
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            SgPoint pt = FromRowColumn(i, j);
            if(board->IsEmpty(pt)) {
                (vector[0])->board[i][j] = 0;
                (vector[1])->board[i][j] = 0;
                (vector[2])->board[i][j] = 1;
            } else if(board->IsColor(pt, currentPlayer)) {
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

std::vector<shared_ptr<Plane>> GoConverter::TurnsSince(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(STONE_MAXIMUM_AGE);
    int moveNum = board->MoveNumber();
    int age = 0;
    
    for (int i = moveNum - 1; i >= 0; i--)
    {
        GoPlayerMove move = board->Move(i);
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

std::vector<shared_ptr<Plane>> GoConverter::Liberties(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_LIBERTIES);
    
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            SgPoint pt = FromRowColumn(i, j);
            if(!board->IsEmpty(pt)) {
                int liberties = board->NumLiberties(pt);
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

std::vector<shared_ptr<Plane>> GoConverter::CaptureSize(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_CAPTURE_SIZE);
    
    SgPointSet pointSet = board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        int row = SgPointToRow(pt);
        int col = SgPointToCol(pt);
        
        int captured = NumOfCaptured(board, pt);
        if (captured < MAX_CAPTURE_SIZE) {
            vector[captured]->board[row][col] = 1;
        } else {
            vector[MAX_CAPTURE_SIZE - 1]->board[row][col] = 1;
        }
    }
    
    return vector;
}

int GoConverter::NumOfCaptured(shared_ptr<GoBoard> board, SgPoint pt)
{
    SgBlackWhite player = board->ToPlay();
    if(!board->IsLegal(pt, player) || !board->CanCapture(pt, player)) {
        return 0;
    }
    SgPoint pointSet[5] = {SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT};
    board->NeighborBlocks(pt, board->Opponent(), 1, pointSet);
    int captured = 0;
    for (int i=0; i<5; i++) {
        if (pointSet[i] == SG_ENDPOINT) {
            break;
        }
        captured += board->NumStones(pointSet[i]);
    }
    return captured;
}

std::vector<shared_ptr<Plane>> GoConverter::SelfAtariSize(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_SELF_ATARI_SIZE);
    
    SgPointSet pointSet = board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        int numStones = NumOfSelfInAtari(board, pt);
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

std::vector<shared_ptr<Plane>> GoConverter::LibertiesAfter(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(MAX_LIBERTIES_AFTER);
    
    SgPointSet pointSet = board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(!board->IsLegal(pt, board->ToPlay())) {
            continue;
        }
        board->TakeSnapshot();
        board->Play(pt);
        
        int liberties = board->NumLiberties(pt);
        if (liberties > 0) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            if (liberties < MAX_LIBERTIES_AFTER) {
                vector[liberties - 1]->board[row][col] = 1;
            } else {
                vector[MAX_LIBERTIES_AFTER - 1]->board[row][col] = 1;
            }
        }
        
        board->RestoreSnapshot();
    }
    
    return vector;
}

int GoConverter::NumOfSelfInAtari(shared_ptr<GoBoard> board, SgPoint pt)
{
    int count = 0;
    // 找出在pt走子后使己方棋子只有一口气的情况
    if (board->IsLegal(pt) &&
        (board->HasNeighbors(pt, board->ToPlay()) ||
        board->HasNeighbors(pt, board->Opponent()))) {
        board->TakeSnapshot();
        board->Play(pt);
        if (board->NumLiberties(pt) == 1) {
            count = board->NumStones(pt);
        }
        board->RestoreSnapshot();
    }
    return count;
}

std::vector<shared_ptr<Plane>> GoConverter::LadderCapture(shared_ptr<GoBoard> board)
{
    std::vector<shared_ptr<Plane>> vector = Zeros(2);
    
    SgPointSet pointSet = board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    SgBlackWhite currentPlayer = board->ToPlay();
    SgBlackWhite opponentPlayer = SgOppBW(currentPlayer);
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(!board->IsLegal(pt, currentPlayer)) {
            continue;
        }
        // Find neighbors with only one liberty
        SgPoint pointSet[5] = {SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT, SG_ENDPOINT};
        board->NeighborBlocks(pt, currentPlayer, 1, pointSet);
        
        if (pointSet[0] != SG_ENDPOINT) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            board->TakeSnapshot();
            board->Play(pt);
            
            GoLadder ladder;
            int result = ladder.Ladder(*board, pt, opponentPlayer, NULL);
            if (result > 0) { // Good for prey
                vector[1]->board[row][col] = 1;
            } else if (result < 0) { // Good for hunter
                vector[0]->board[row][col] = 1;
            }
            
            board->RestoreSnapshot();
        }
    }
    return vector;
}

shared_ptr<Plane> GoConverter::Sensibleness(shared_ptr<GoBoard> board)
{
    shared_ptr<Plane> plane = zero();
    
    SgPointSet pointSet = board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    SgBlackWhite currentPlayer = board->ToPlay();
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(!board->IsLegal(pt, currentPlayer)) {
            continue;
        }
        if (!GoEyeUtil::IsPossibleEye(*board, currentPlayer, pt)) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            plane->board[row][col] = 1;
        }
    }
    
    return plane;
}

shared_ptr<Plane> GoConverter::LegalMoves(shared_ptr<GoBoard> board)
{
    shared_ptr<Plane> plane = zero();
    
    SgPointSet pointSet = board->AllEmpty();
    int size = pointSet.Size();
    SgVector<SgPoint> sgVector;
    pointSet.ToVector(&sgVector);
    
    SgBlackWhite currentPlayer = board->ToPlay();
    
    for (int i = 0; i < size; i++) {
        SgPoint pt = sgVector[i];
        if(board->IsLegal(pt, currentPlayer)) {
            int row = SgPointToRow(pt);
            int col = SgPointToCol(pt);
            plane->board[row][col] = 1;
        }
    }
    
    return plane;
}

//void GoConverter::StateToTensor(shared_ptr<GoBoard> board, Board** tensor[])
//{
//    std::vector<shared_ptr<Plane>> vector = BoardState(board);
//    tensor[1][0] = &(vector[0]->board);
//    tensor[1][1] = &(vector[1]->board);
//    tensor[1][1] = &(vector[2]->board);
//}

void GoConverter::ToHDF5(std::string& sgfFile, std::string &hdfFile)
{
    hid_t file = H5Fcreate(hdfFile.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    shared_ptr<SgfExporter> exporter = shared_ptr<SgfExporter>(new SgfExporter(sgfFile));
    shared_ptr<GoBoard> b = exporter->Board();
    std::vector<GoPlayerMove> moves = exporter->Moves();
    
    // states dataset dimensions
//    hsize_t dimsf[4];
//    dimsf[0] = moves.size();
//    dimsf[1] = 3; // number of planes
//    dimsf[2] = GO_DEFAULT_SIZE;
//    dimsf[3] = GO_DEFAULT_SIZE;
//    hsize_t max_dimsf[4];
//    max_dimsf[0] = H5S_UNLIMITED;
//    max_dimsf[1] = 3; // number of planes
//    max_dimsf[2] = GO_DEFAULT_SIZE;
//    max_dimsf[3] = GO_DEFAULT_SIZE;
//    hid_t s_dataspace = H5Screate_simple(4, dimsf, max_dimsf);
    
    // action dataset dimensions
    hsize_t actionDimsf[2];
    actionDimsf[0] = moves.size();
    actionDimsf[1] = 2;
    hsize_t max_actionDimsf[2];
    max_actionDimsf[0] = H5S_UNLIMITED;
    max_actionDimsf[1] = 2;
    hid_t a_dataspace = H5Screate_simple(2, actionDimsf, max_actionDimsf);

//    hsize_t chunkDimsf[4];
//    chunkDimsf[0] = 64;
//    chunkDimsf[1] = 3; // number of planes
//    chunkDimsf[2] = GO_DEFAULT_SIZE;
//    chunkDimsf[3] = GO_DEFAULT_SIZE;
//    
//    // Dataset creation property list
//    hid_t s_dcpl = H5Pcreate(H5P_DATASET_CREATE);
//    H5Pset_chunk(s_dcpl, 4, chunkDimsf);
//        H5Pset_deflate(dcpl, 1);
    
    hsize_t actionChunkDimsf[2] = {1024, 2};
    hid_t a_dcpl = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(a_dcpl, 2, actionChunkDimsf);
    
    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
//    hid_t stateDS = H5Dcreate(file, "states", H5T_STD_U8LE, s_dataspace, H5P_DEFAULT, s_dcpl, H5P_DEFAULT);
    hid_t actionDS = H5Dcreate(file, "actions", H5T_STD_U8LE, a_dataspace, H5P_DEFAULT, a_dcpl, H5P_DEFAULT);
    
    std::vector<GoPlayerMove>::iterator iter;
    int wdata[moves.size()][2];
    int i=0;
    
    for (iter = moves.begin(); iter != moves.end(); ++iter) {
        // write state
//        Board tensor[1][3];
//        std::vector<shared_ptr<Plane>> vector = BoardState(b);
//        copyBoard(vector[0]->board, tensor[1][0]);
//        copyBoard(vector[1]->board, tensor[1][1]);
//        copyBoard(vector[2]->board, tensor[1][2]);
//        H5Dwrite(stateDS, H5T_NATIVE_UINT8, H5S_ALL, H5S_ALL, H5P_DEFAULT, tensor);
        // write action
        wdata[i][0] = SgPointToCol((*iter).Point());
        wdata[i][1] = SgPointToRow((*iter).Point());
        i++;
        
        b->Play(*iter);
    }
    H5Dwrite(actionDS, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata[0]);

//    H5Dflush(stateDS);
    H5Dflush(actionDS);
    
//    H5Pclose(s_dcpl);
//    H5Sclose(s_dataspace);
//    H5Dclose(stateDS);
    H5Pclose(a_dcpl);
    H5Sclose(a_dataspace);
    H5Dclose(actionDS);
    H5Fclose(file);
}

void GoConverter::copyBoard(Board &from, Board &to)
{
    for (int i = 0; i < GO_DEFAULT_SIZE; i++) { // row
        for (int j = 0; j < GO_DEFAULT_SIZE; j++) { // column
            to[i][j] = from[i][j];
        }
    }
}
