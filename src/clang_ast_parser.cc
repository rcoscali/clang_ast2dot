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
     *
     */
    Ast2DotParser::Ast2DotParser()
    {
    }

    /**
     *
     */
    Ast2DotParser::~Ast2DotParser()
    {
    }

  } // ! parser
} // ! clang_ast2dot
