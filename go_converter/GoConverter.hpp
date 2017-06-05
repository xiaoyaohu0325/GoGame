//
//  GoConverter.hpp
//  go_converter
//
//  Created by 王坚 on 17/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#ifndef GoConverter_hpp
#define GoConverter_hpp

#include <boost/shared_ptr.hpp>
#include "GoBoard.h"

struct Plane {
    float board[GO_DEFAULT_SIZE][GO_DEFAULT_SIZE];
};

using boost::shared_ptr;

const int STONE_MAXIMUM_AGE = 8;
const int MAX_LIBERTIES = 8;
const int MAX_CAPTURE_SIZE = 8;
const int MAX_SELF_ATARI_SIZE = 8;
const int MAX_LIBERTIES_AFTER = 8;

class GoConverter
{
public:
    GoConverter(shared_ptr<GoBoard> board);
    
    void convert(std::string sfgFile);
    
    std::vector<shared_ptr<Plane>> Ones(int planes);
    
    std::vector<shared_ptr<Plane>> Zeros(int planes);
    /**
     * A feature encoding WHITE BLACK and EMPTY on separate planes, but plane 0
     * always refers to the current player and plane 1 to the opponent
     */
    std::vector<shared_ptr<Plane>> BoardState();
    /**
     A feature encoding the age of the stone at each location up to 'maximum'
     Note:
     - the [maximum-1] plane is used for any stone with age greater than or equal to maximum
     - EMPTY locations are all-zero features
     */
    std::vector<shared_ptr<Plane>> TurnsSince();
    /**
     A feature encoding the number of liberties of the group connected to the stone at
     each location
     
     Note:
     - there is no zero-liberties plane; the 0th plane indicates groups in atari(打吃状态)
     - the [maximum-1] plane is used for any stone with liberties greater than or equal to maximum
     - EMPTY locations are all-zero features
     */
    std::vector<shared_ptr<Plane>> Liberties();
    /**
     A feature encoding the number of opponent stones that would be captured by
     playing at each location, up to 'maximum'
     
     Note:
     - we currently *do* treat the 0th plane as "capturing zero stones"
     - the [maximum-1] plane is used for any capturable group of size
     greater than or equal to maximum-1
     - the 0th plane is used for legal moves that would not result in capture
     - illegal move locations are all-zero features
     */
    std::vector<shared_ptr<Plane>> CaptureSize();
    /**
     A feature encoding the size of the own-stone group that is put into atari by
     playing at a location
     */
    std::vector<shared_ptr<Plane>> SelfAtariSize();
    /**
     A feature encoding what the number of liberties *would be* of the group connected to
     the stone *if* played at a location
     
     Note:
     - there is no zero-liberties plane; the 0th plane indicates groups in atari
     - the [maximum-1] plane is used for any stone with liberties greater than or equal to maximum
     - illegal move locations are all-zero features
     */
    std::vector<shared_ptr<Plane>> LibertiesAfter();
    /**
     A feature wrapping is_ladder_capture
     first result is for GOOD_FOR_HUNTER, ladder capture
     second result is for GOOD_FOR_PREY, ladder escape
     */
    std::vector<shared_ptr<Plane>> LadderCapture();
    /**
     A move is 'sensible' if it is legal and if it does not fill the current_player's own eye
     */
    shared_ptr<Plane> Sensibleness();
    /**
     Zero at all illegal moves, one at all legal moves. Unlike sensibleness, no eye check is done
     */
    shared_ptr<Plane> LegalMoves();
    
    int NumOfCaptured(SgPoint pt);
    
    int NumOfSelfInAtari(SgPoint pt);
private:
    shared_ptr<GoBoard> m_board;
    
    shared_ptr<Plane> zero();
    shared_ptr<Plane> one();
};

#endif /* GoConverter_hpp */
