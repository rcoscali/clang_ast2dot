/**
 * @test_parser.h
 */

#ifndef _TEST_PARSER_H_
#define _TEST_PARSER_H

#include <string>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>

#include "gtest/gtest.h"

namespace clang_ast2dot
{
  namespace parser
  {
    class TestParser : public ::testing::Test
    {
    public:
      TestParser();
      virtual ~TestParser();
      
      virtual void SetUp();
      virtual void TearDown();

      std::ifstream* _test_parser_testfile;
      
    private:
      std::string _TEST_FILE_NAME;
    };
  }
}

#endif /* ! _TEST_PARSER_H_ */
