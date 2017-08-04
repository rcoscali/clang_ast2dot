/**
 * @test_parser.cc
 */

#include <iostream>
#include <fstream>
#include <string>
#include "test_parser.h"
#include "clang_ast_parser.h"

namespace clang_ast2dot
{
    namespace parser
    {
        TestParser::TestParser()
        {
            // Open first test file
            _TEST_FILE_NAME.assign("../tests/test_parser.txt");
            _test_parser_testfile = new std::ifstream();

            // And second test file
            _TEST_FILE_NAME2.assign("../tests/test_parser2.txt");
            _test_parser2_testfile = new std::ifstream();

            // And the valid values file
            _TEST_FILE_NAME3.assign("../tests/test_parser2.res");
            _test_parser3_testfile = new std::ifstream();
        }
    
        TestParser::~TestParser()
        {
            delete _test_parser_testfile;
            delete _test_parser2_testfile;
            delete _test_parser3_testfile;
        }

        void TestParser::SetUp()
        {
            _test_parser_testfile->open(_TEST_FILE_NAME, std::ios_base::in);
            _test_parser2_testfile->open(_TEST_FILE_NAME2, std::ios_base::in);
            _test_parser3_testfile->open(_TEST_FILE_NAME3, std::ios_base::in);
        }
      
        void TestParser::TearDown()
        {
            if (_test_parser_testfile->is_open())
                _test_parser_testfile->close();
            if (_test_parser2_testfile->is_open())
                _test_parser2_testfile->close();
            if (_test_parser3_testfile->is_open())
                _test_parser3_testfile->close();
        }
      
        void TestParser::PrintTo(const TestParser& parser, ::std::ostream* os)
        {
            size_t pos = _test_parser_testfile->tellg();
            *os << "Test file position: " << pos << "\n";
            size_t pos2 = _test_parser2_testfile->tellg();
            *os << "Test file #2 position: " << pos2 << "\n";
            size_t pos3 = _test_parser3_testfile->tellg();
            *os << "Test file #3 position: " << pos3 << "\n";
        }
      
        TEST_F(TestParser, Instanciate)
        {
            Ast2DotParser p;
            std::string str1;

            // Check test file is open
            EXPECT_TRUE(_test_parser_testfile->is_open());

            // Set pointer at start of the file
            _test_parser_testfile->seekg(0, std::ios_base::beg);

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
            std::string str1;
      
            // Check test file is open
            EXPECT_TRUE(_test_parser_testfile->is_open());
      
            // Set pointer at start of the file
            _test_parser_testfile->seekg(0, std::ios_base::beg);

            // Check 1st line is empty scstr
            EXPECT_THROW(str1 = p.read_sibling_child_string(_test_parser_testfile), Ast2DotParser::EmptyScStrException);
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line0");
      
            // Check 2nd line is invalid scstr
            EXPECT_THROW(str1 = p.read_sibling_child_string(_test_parser_testfile), Ast2DotParser::InvalidScStrException);
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
      
            // Check 3nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "| |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "-Line-2");

            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line1");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "| |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line2");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "| `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line3");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line4");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   | |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line5");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   | `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line6");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |   `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line7");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line8");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   | |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line9");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   | `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line10");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |   | `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line11");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |   `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line12");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |     |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line13");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |     | `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line14");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |     `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line15");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |       `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line16");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |         |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line17");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |         | `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line18");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |         `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line19");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   |           `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line20");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|   `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line21");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|     `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line22");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "|       `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line23");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "`-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line24");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "  |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line25");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "  |-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line26");
      
            // Check 4nd line is invalid scstr
            str1 = p.read_sibling_child_string(_test_parser_testfile);
            EXPECT_STREQ(p.scstr().c_str(), "  `-");
      
            // Discard to next line
            std::getline(*_test_parser_testfile, str1);
            EXPECT_STREQ(str1.c_str(), "Line27");
      
            // Check next line is invalid scstr
            EXPECT_THROW(str1 = p.read_sibling_child_string(_test_parser_testfile), Ast2DotParser::InvalidScStrException);

            // Discard until end of file (2 lines)
            std::getline(*_test_parser_testfile, str1);
            std::getline(*_test_parser_testfile, str1);
            // Check next line is unexpected eof scstr
            EXPECT_THROW(str1 = p.read_sibling_child_string(_test_parser_testfile), Ast2DotParser::UnexpectedEofException);
        }

        TEST_F(TestParser, SpecialQuoting)
        {
            Ast2DotParser p;
            std::string in;
            std::string out;
            std::string* outptr;

            in = "a simple <test for simple quote with less/greater than> special chars";
            out = in;
            out = p.quote_special_quotes(out, "<", ">", outptr);
            EXPECT_STREQ(out.c_str(), "a simple \"<test for simple quote with less/greater than>\" special chars");
            EXPECT_FALSE(outptr == (std::string*)NULL);
            if (outptr) delete (outptr);

            out = p.quote_special_quotes(in, ">", "<", outptr);
            EXPECT_STREQ(out.c_str(), in.c_str());
            EXPECT_TRUE(outptr == (std::string*)NULL);
            if (outptr) delete (outptr);

            in = "another <<one with special>> quotes";
            out = in;
            out = p.quote_special_quotes(out, "<<", ">>", outptr);
            EXPECT_STREQ(out.c_str(), "another \"<<one with special>>\" quotes");
            EXPECT_FALSE(outptr == (std::string*)NULL);
            if (outptr) delete (outptr);
            
            out = p.quote_special_quotes(in, ">>", "<<", outptr);
            EXPECT_STREQ(out.c_str(), in.c_str());
            EXPECT_TRUE(outptr == (std::string*)NULL);
            if (outptr) delete (outptr);
            
            in = "and <<finally another>> one <<<the very last>>> quotes";
            out = in;
            out = p.quote_special_quotes(out, "<<", ">>", outptr);
            out = p.quote_special_quotes(out, "<<<", ">>>", outptr);
            EXPECT_STREQ(out.c_str(), "and \"<<finally another>>\" one \"<<<the very last>>>\" quotes");
            EXPECT_FALSE(outptr == (std::string*)NULL);
            if (outptr) delete (outptr);
            
            out = p.quote_special_quotes(in, ">>>", "<<<", outptr);
            EXPECT_STREQ(out.c_str(), in.c_str());
            EXPECT_TRUE(outptr == (std::string*)NULL);
            if (outptr) delete (outptr);            
        }

        TEST_F(TestParser, Null2Name)
        {
            Ast2DotParser p;

            EXPECT_STREQ(p.null_to_name(0).c_str(), "NULL_0");
            EXPECT_STREQ(p.null_to_label("NULL_0").c_str(), "&lt;&lt;&lt;NULL_0&gt;&gt;&gt;");
            EXPECT_STREQ(p.null_to_name(-1).c_str(), "NULL_1");
            EXPECT_STREQ(p.null_to_name(2).c_str(), "NULL_2");
            EXPECT_STREQ(p.null_to_name(0).c_str(), "NULL_0");
            EXPECT_STREQ(p.null_to_name(-1).c_str(), "NULL_3");
            EXPECT_STREQ(p.null_to_name(2).c_str(), "NULL_2");
            EXPECT_STREQ(p.null_to_label("NULL_1").c_str(), "&lt;&lt;&lt;NULL_1&gt;&gt;&gt;");
            EXPECT_STREQ(p.null_to_label("NULL_2").c_str(), "&lt;&lt;&lt;NULL_2&gt;&gt;&gt;");
            EXPECT_STREQ(p.null_to_label("NULL_3").c_str(), "&lt;&lt;&lt;NULL_3&gt;&gt;&gt;");
            EXPECT_STREQ(p.null_to_label("NULL_4").c_str(), "");
        }

        TEST_F(TestParser, ParseVertexProps)
        {
            Ast2DotParser p;
      
            // Check test file is open
            EXPECT_TRUE(_test_parser2_testfile->is_open());

            // Set pointer at start of the file
            _test_parser2_testfile->seekg(0, std::ios_base::beg);      

            // Check result file is open
            EXPECT_TRUE(_test_parser3_testfile->is_open());

            // Set pointer at start of the file
            _test_parser3_testfile->seekg(0, std::ios_base::beg);      

            while (!_test_parser2_testfile->eof())
                {
                    std::string *vertex_str = p.read_vertex_props(_test_parser2_testfile, &std::cerr);
                    if (!vertex_str->empty())
                        {
                            std::string result;
                            std::getline(*_test_parser3_testfile, result);
                            result.append("\n");
                            EXPECT_TRUE(vertex_str->compare(result) == 0);
                        }
                    delete vertex_str;
                }
        }
    }
}

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
