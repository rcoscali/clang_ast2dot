/**
 * @file clang_ast2dot.h
 *
 */

#ifndef _CLANG_AST_PARSER_H_
#define _CLANG_AST_PARSER_H_

/**
 * C System headers
 *
 */

/**
 * C++ System headers
 *
 * vector for lists
 */

#include <string>
#include <vector>

/**
 * Boost headers
 */
#include <boost/regex.hpp>

namespace clang_ast2dot
{
  namespace parser
  {
    class Ast2DotParser
    {
    public:
      Ast2DotParser();
      ~Ast2DotParser();

      get_vertex()

    private:
      // Line buffer
      std::string _inbuf;

      // Edges
      std::string _scstr;
      // Vertex name
      std::string _name;
      // Vertex address
      std::string _address;
      // Other props (class dependent)
      std::vector<std::string> _props;

      // Vertex is leaf?
      bool _is_leaf;
    }
  } // ! parser
} // ! clang_ast2dot

#endif /* ! _CLANG_AST_PARSER_H_ */


