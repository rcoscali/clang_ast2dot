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
	  // Use a stringbuf over scstr member
	  std::stringbuf sb(_scstr, std::ios_base::in);
	  // And read from standard input until '-' (end of scstr) if encountered
	  // '-' is also read (consummed)
	  if ((c = is->get(sb, '-').get()) != '-')
	    throw std::exception();
	  // Append '-' to the result string
	  std::cerr << "scstr = '" << _scstr << "'\n";
	  _scstr.append(1, '-');
	}
      else
	throw std::exception();
      return _scstr;
    }

  } // ! parser
} // ! clang_ast2dot
