/**
 * @file clang_ast2dot.h
 *
 */

#ifndef _CLANG_AST2DOT_H_
#define _CLANG_AST2DOT_H_

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

/**
 * Boost headers
 */
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/program_options.hpp>

/**
 * Own headers
 */
#include "clang_ast_parser.h"

using namespace boost;
namespace po = boost::program_options;
namespace postyle = po::command_line_style;

namespace clang_ast2dot
{
  class Ast2DotMain
  {
  public:
    Ast2DotMain(int, char *[]);
    virtual ~Ast2DotMain();

    po::variables_map& vm(void);
    int do_main(int);
    void create_dot(std::istream *, std::ostream *, std::string const&, int);
    
  private:
    int _argc;
    std::vector<std::pair<int, std::string> > _argv;
    po::variables_map _vm;
    boost::regex _re;
    boost::smatch _what;
  };
  
} // namespace clang_ast2dot

#endif /* !_CLANG_AST2DOT_H_ */
