/**
 * @file clang_ast2dot.cc
 */

/**
 * C System headers
 *
 * string.h was used for memcpy
 */
//#include <string.h>

/**
 * C++ System headers
 *
 * cstdlib 
 * fstream for ifstream, ofstream, getline, etc...
 * iostream for cin, cout, etc...
 */
#include <cstdlib>
#include <fstream>
#include <iostream>

/**
 * Boost tokenizer for parsing ast dump lines
 */
#include <boost/tokenizer.hpp>

// Include our defs
#include "clang_ast_parser.h"

namespace clang_ast2dot
{
    namespace parser
    {
        /**
         * Ast2DotParser Constructor
         */
        Ast2DotParser::Ast2DotParser()
        {
        }
        
        /** 
         * Ast2DotParser Destructor
         */
        Ast2DotParser::~Ast2DotParser()
        {
        }
        
        /** 
         * Read the string that display tree relationships in ASt dump
         *
         * @pre std::cin reads the AST dump. The file pointer is located 
         *      at start of a new line.
         *
         * @post std::cin points on the next AST class name
         *
         * @return the sibling/child string representing the relationship
         *         between AST classes
         */
        std::string&
        Ast2DotParser::read_sibling_child_string(std::istream* is)
        {
            char c = 0;
            _scstr.clear();
      
            if (!is->eof())
                {
                    do
                        {
                            c = is->get();
                            if (c == '|' ||
                                c == ' ' ||
                                c == '`')
                                _scstr.append(1, c);
                            else
                                is->unget();
                        }
                    while(c != '|' && c != ' ' && c != '`');

                    if (_scstr.empty())
                        throw Ast2DotParser::EmptyScStrException();

                    if ((c = is->get()) != '-')
                        throw Ast2DotParser::InvalidScStrException();
                    else
                        _scstr.append(1, '-');
                }

            else
                throw Ast2DotParser::UnexpectedEofException();
            
            return _scstr;
        }
        
    } // ! parser
} // ! clang_ast2dot
