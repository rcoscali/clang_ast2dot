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
      _TEST_FILE_NAME.assign("./test_parser.txt");
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
      
      void TestParser::PrintTo(const Ast2DotParser& parser, ::std::ostream* os) {
          size_t pos = _test_parser_testfile->tellg();
          *os << "Test file position: " << pos << "\n";
      }
      
      TEST_F(TestParser, Instanciate)
      {
          Ast2DotParser p;
          std::string str1;
          
          ASSERT_THROW((void)p.read_sibling_child_string(_test_parser_testfile), Ast2DotParser::EmptyScStrException);
          
      }
  }
}

int
main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
