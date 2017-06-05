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
#include <boost/shared_ptr.hpp>

#include "GoBoard.h"
#include "GoSetup.h"
#include "GoRules.h"
#include "SgNode.h"
#include "SgProp.h"
#include "GoPlayerMove.h"

using boost::shared_ptr;

class SgfExporter {
public:
    SgfExporter(std::string& sfgPath);
    
    void ToPNG(std::string& outputFile);
    void ToText();
    shared_ptr<GoBoard> Board() const;
private:
    shared_ptr<GoBoard> board;
    std::vector<GoPlayerMove> moves;
    int size;
    GoSetup goSetup;
    GoRules goRules;
    
    void parseNode(SgNode* node);
    void inspectProp(SgNode* node, SgProp* prop);
};

#endif /* SgfExporter_hpp */
