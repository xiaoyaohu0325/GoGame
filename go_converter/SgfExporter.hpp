//
//  SgfExporter.hpp
//  go_converter
//
//  Created by 王坚 on 02/06/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#ifndef SgfExporter_hpp
#define SgfExporter_hpp

#include <stdio.h>
#include <iostream>
#include <vector>

#include "GoBoard.h"
#include "GoSetup.h"
#include "GoRules.h"
#include "SgNode.h"
#include "SgProp.h"
#include "GoPlayerMove.h"

class SgfExporter {
public:
    SgfExporter(std::string& sfgPath);
    ~SgfExporter();
    
    void ToPNG(std::string& outputFile);
    void ToText();
    GoBoard* Board() const;
private:
    GoBoard* board;
    std::vector<GoPlayerMove> moves;
    int size;
    GoSetup goSetup;
    GoRules goRules;
    
    void parseNode(SgNode* node);
    void inspectProp(SgNode* node, SgProp* prop);
};

#endif /* SgfExporter_hpp */
