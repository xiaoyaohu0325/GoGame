//
//  main.cpp
//  go_converter
//
//  Created by 王坚 on 09/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#include <iostream>
#include <typeinfo>

#include <cairo/cairo.h>
#include "SgInit.h"
#include "GoInit.h"

#include "SgfExporter.hpp"


int main(int argc, const char * argv[]) {
    SgInit();
    GoInit();
    
    std::string sgf("/Users/wangjian/WORK_ROOT/go_converter/go_converter/go_converter/data/2016-01-01-1.sgf");
    SgfExporter exporter(sgf);
    exporter.ToText();
    
    GoFini();
    SgFini();
   
    return 0;
}


