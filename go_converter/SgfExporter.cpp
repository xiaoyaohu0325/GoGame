//
//  SgfExporter.cpp
//  go_converter
//
//  Created by 王坚 on 02/06/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#include "SgfExporter.hpp"

#include <fstream>
#include <cairo/cairo.h>

#include "SgGameReader.h"
#include "SgPoint.h"

#include "PointConverter.h"

SgfExporter::SgfExporter(std::string& sfgPath) {
    std::ifstream sgfFile(sfgPath, std::ifstream::in);
    board = NULL;
    if (sgfFile.is_open()) {
        SgGameReader reader(sgfFile);
        SgNode* node = reader.ReadGame();
    
        while(node) {
            parseNode(node);
            node = node->LeftMostSon();
        }
        sgfFile.close();
        board = new GoBoard(size, goSetup, goRules);
        std::vector<GoPlayerMove>::iterator iter;
        
        for (iter = moves.begin(); iter != moves.end(); ++iter) {
            board->Play(*iter);
        }
    }
}

SgfExporter::~SgfExporter() {
    if (board != NULL){
        delete board;
    }
}

void SgfExporter::ToPNG(std::string &outputFile) {
    
}

void SgfExporter::ToText() {
    if (board == NULL) {
        return;
    }
    for (int i=0;i<size;i++) {
        std::string line;
        for (int j=0;j<size;j++) {
            SgPoint pt = FromRowColumn(i, j);
            SgBlackWhite color = board->GetStone(pt);
            if(color == SG_BLACK) {
                line.append("B");
            } else if (color == SG_WHITE) {
                line.append("W");
            } else if (color == SG_EMPTY) {
                line.append("O");
            }
        }
        std::cout << line << std::endl;
    }
}

void SgfExporter::parseNode(SgNode* node) {
    for (SgPropListIterator it(node->Props()); it; ++it) {
        inspectProp(node, (*it));
    }
}

void SgfExporter::inspectProp(SgNode* node, SgProp* prop)
{
    std::string propLabel = prop->Label();
    std::vector<std::string> values;
    prop->ToString(values, 19, SG_PROPPOINTFMT_GO, 0);
    for(std::vector<std::string>::iterator it = values.begin();it != values.end(); ++it) {
//        std::cout << propLabel << "(" << typeid(*prop).name() << "):" << *it << std::endl;
        if (propLabel.compare("PB") == 0) {
            // Black player
        } else if(propLabel.compare("PW") == 0) {
            // White player
        } else if(propLabel.compare("BR") == 0) {
            // Black rank
        } else if(propLabel.compare("WR") == 0) {
            // White rank
        } else if(propLabel.compare("SZ") == 0) {
            // board size
            size = std::stoi(*it);
        } else if(propLabel.compare("KM") == 0) {
            // Komi 贴目
            goRules.SetKomi(std::stof(*it));
        } else if(propLabel.compare("HA") == 0) {
            // Handicap 让子数
            goRules.SetHandicap(std::stoi(*it));
        } else if(propLabel.compare("AB") == 0) {
            // Add Black
            goSetup.AddBlack(FromSGFLetter(*it));
        } else if(propLabel.compare("AW") == 0) {
            // Add White
            goSetup.AddWhite(FromSGFLetter(*it));
        } else if(propLabel.compare("B") == 0) {
            // Black move
            SgPoint pt = FromSGFLetter(*it);
            GoPlayerMove move(SG_BLACK, pt);
            moves.push_back(move);
        } else if(propLabel.compare("W") == 0) {
            // White move
            SgPoint pt = FromSGFLetter(*it);
            GoPlayerMove move(SG_WHITE, pt);
            moves.push_back(move);
        }
    }
}

