//
//  main.cpp
//  go_tests
//
//  Created by 王坚 on 12/05/2017.
//  Copyright © 2017 王坚. All rights reserved.
//

//#include <boost/test/unit_test.hpp>
#include <gtest/gtest.h>
#include "SgInit.h"
#include "GoInit.h"

namespace {
    
    void Init()
    {
        SgInit();
        GoInit();
    }
    
    void Fini()
    {
        GoFini();
        SgFini();
    }
    
} // namespace



bool init_unit_test()
{
    try
    {
        Init();
    }
    catch (const std::exception& e)
    {
        return false;
    }
    if (std::atexit(Fini) != 0)
        return false;
    return true;
}

//boost::unit_test::test_suite* init_unit_test_suite( int argc, char* argv[] )
//{
//    boost::unit_test::test_suite* test = BOOST_TEST_SUITE( "Master test suite" );
//    
//    test->add( BOOST_TEST_CASE( &init_unit_test ) );
//    
//    return test;
//}

GTEST_API_ int main(int argc, char **argv) {
    init_unit_test();
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
