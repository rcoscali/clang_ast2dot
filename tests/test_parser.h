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
      
      // It's important that PrintTo() is defined in the SAME
      // namespace that defines Bar.  C++'s look-up rules rely on that.
      void PrintTo(const TestParser&, ::std::ostream*);
      
      std::ifstream* _test_parser_testfile;
      std::ifstream* _test_parser2_testfile;
      
    private:
      std::string _TEST_FILE_NAME;
      std::string _TEST_FILE_NAME2;
    };
  }
}

#endif /* ! _TEST_PARSER_H_ */
