/*! @file test_main.cpp
 * @brief main file to launch googletest
 * @author Max Kontak <Max.Kontak@DLR.de>
 * @date 2019-05-10
 */

#include <gtest/gtest.h>

int main( int argc, char **argv )
{
  testing::InitGoogleTest( &argc, argv );

  return RUN_ALL_TESTS();
}
