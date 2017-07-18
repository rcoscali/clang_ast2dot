/*
 * @file clang_ast2dot.h
 *
 */

#ifndef _CLANG_AST2DOT_H_
#define _CLANG_AST2DOT_H_

#include <string>
#include <vector>

#include <boost/config.hpp>
#include <boost/utility.hpp>                // for boost::tie

#include <boost/program_options.hpp>

using namespace boost;
namespace po = boost::program_options;
namespace postyle = po::command_line_style;

namespace clang_ast2dot
{
  class Ast2DotMain
  {
  public:
    Ast2DotMain(int argc, char *argv[]);
    virtual ~Ast2DotMain();

    po::variables_map& vm(void);
    int do_main(int opt_ind);

  private:
    int _argc;
    std::vector<std::pair<int, std::string> > _argv;
    po::variables_map _vm;
  };
  
} // namespace clang_ast2dot

#endif /* !_CLANG_AST2DOT_H_ */
