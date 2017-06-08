//
//  HDF5Test.cpp
//  go_converter
//
//  Created by 王坚 on 07/06/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

#include <gtest/gtest.h>
#include "GoConverter.hpp"

TEST(PointConverterTest, hdf5)
{
    GoConverter converter;
    std::string sgf("/Users/wangjian/WORK_ROOT/go_converter/go_converter/go_converter/data/2016-01-01-1.sgf");
    std::string hdf("/Users/wangjian/WORK_ROOT/go_converter/go_converter/temp/out.hdf");
    converter.ToHDF5(sgf, hdf);
}
