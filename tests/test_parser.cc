/**
 * @test_parser.cc
 */

#include "test_parser.h"
#include "clang_ast_parser.h"

namespace clang_ast2dot
{
  namespace parser
  {
    TestParser::TestParser()
    {
      _TEST_FILE_NAME.assign("../tests/test_parser.txt");
      _test_parser_testfile = new std::ifstream();
    }
    
    TestParser::~TestParser()
    {
      delete _test_parser_testfile;
    }

      void TestParser::SetUp()
    {
        _test_parser_testfile->open(_TEST_FILE_NAME, std::ios_base::in);
    }
      
      void TestParser::TearDown()
      {
          if (_test_parser_testfile->is_open())
              _test_parser_testfile->close();
      }
      
      void TestParser::PrintTo(const TestParser& parser, ::std::ostream* os)
      {
          size_t pos = _test_parser_testfile->tellg();
          *os << "Test file position: " << pos << "\n";
      }
      
      TEST_F(TestParser, Instanciate)
      {
          Ast2DotParser p;
          std::string str1;

	  // Check test file is open
	  EXPECT_TRUE(_test_parser_testfile->is_open());

	  // Assert all default values of Parser fields
	  EXPECT_STREQ(p.inbuf().c_str(), "");
	  EXPECT_STREQ(p.scstr().c_str(), "");
	  EXPECT_STREQ(p.name().c_str(), "");
	  EXPECT_STREQ(p.address().c_str(), "");
	  EXPECT_EQ(p.props().capacity(), 5);
      }

    TEST_F(TestParser, ParseScStr)
    {
          Ast2DotParser p;

	  // Check test file is open
	  EXPECT_TRUE(_test_parser_testfile->is_open());
	  EXPECT_THROW(p.read_sibling_child_string(_test_parser_testfile), Ast2DotParser::EmptyScStrException());
    }
  }
}

int
main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
