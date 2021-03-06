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
#include "clang_ast2dot.h"
#include "clang_ast_parser.h"

/*
 * Constants definitions
 */
// Version string
#define AST2DOT_VERSION_STRING                  "1.0.0"
// System config file
#define AST2DOT_SYSTEM_CONFIG_FILE_PATH         "/etc/clang_ast2dotrc"
// User config file
#define AST2DOT_HOME_CONFIG_FILE_PATH           std::string(::getenv("HOME")).append("/.clang_ast2dotrc").c_str()
// Local directory config file
#define AST2DOT_LOCAL_CONFIG_FILE_PATH          "./.clang_ast2dotrc"
// Regex for matching ast dump relationship string
#define AST_DUMP_RELATIONSHIP_REGEX             "^((\\| )|( \\|)|(  ))*(\\|-|`-)$"

// Verbose level
static int opt_verbose = 0;

// Use std and boost namespaces
using namespace std;
using namespace boost;

namespace ast2dot = clang_ast2dot::parser;

/*
 * clang_ast2dot namespace
 */
namespace clang_ast2dot
{
  /*
   * Ast2DotMain constructor
   * Instanciate main class for tool
   *
   * @param argc        number of command line options
   * @param argv        array of string containing command line options
   */
  Ast2DotMain::Ast2DotMain(int argc, char *argv[])
  {
    // Init argc
    _argc = argc;

    // copy command line options in class vector member
    for (int i = 0; i < _argc; i++)
      _argv.push_back(std::make_pair(i, std::string(argv[i])));

    // init regular expression for parsing the tree relations
    _re = boost::regex(AST_DUMP_RELATIONSHIP_REGEX);
  }

  /*
   * Virtual destructor
   */
  Ast2DotMain::~Ast2DotMain()
  {
  }

  po::variables_map& Ast2DotMain::vm(void)
  {
    return _vm;
  }
  
  /*
   * Recursively create the .dot file 
   */
  int
  Ast2DotMain::create_dot(std::istream* is, std::ostream* os, std::string const& parent_vertex, int level)
  {
    /* 
     * Cout eventually sends to output file and cin gets from input file
     */
    
    // While not end of file
    while (!is->eof())
      {
        std::string name;
        
        // Parse vertex and return the vertex string for the .dot file
        // Fields in Parser instance are initialized with each part
        std::string *vertex = _parser.read_vertex_props(is, os);

        // Get the name (= Vertex name + vertex address)
        if (!vertex->empty())
          {
            name = _parser.name();
            if (!_parser.is_null())
              name.append(std::string("_").append(_parser.address()));
          }

        // Out the vertex string
        *os << *vertex;

        delete vertex;

        // And if some relationship is needed add the directed edge
        if (!(parent_vertex.empty() || name.empty()))
          *os << "    " << parent_vertex << " -> " << name << " [style=\"solid\",color=black,weight=100,constraint=true];\n";

        // Let's flush for easier debug
        os->flush();

        // Start next vertex relationship string
        std::string scstr;
        
        // Read relationship spec string
        try
          {
            // Returns the full string from beginning of line until the dash '-'
            // ("|-" for a vertex having still sibling or "`-" for a vertex being
            // the last child)
            scstr = _parser.read_sibling_child_string(is);
            // If the string is empty, we could be at end of file
            if (!scstr.empty())
              {
            	// The next vertex level in the tree is hasl the size of the string
                int new_level = scstr.length() / 2;

                // We goes down deeper in tree, so call again with new_level, parent being current vertex
                if (new_level > level)
                  new_level = create_dot(is, os, name, new_level);

                // If we have a sibling vertex, call again with same level and same parent
                if (new_level == level)
                  new_level = create_dot(is, os, parent_vertex, level);

                // If level is less, let's go down (return new_level)
                if (new_level < level)
                  // Get back up in tree
                  return new_level;
              }
          }
        catch (ast2dot::Ast2DotParser::UnexpectedEofException const& ueofe)
          {
              break;
          }
        catch (ast2dot::Ast2DotParser::EmptyScStrException const& esse)
          {
              break;
          }
        catch (ast2dot::Ast2DotParser::InvalidScStrException const& isse)
          {
            break;
          }
      }
    return level;
  }
  
  /*
   *
   */
  int
  Ast2DotMain::do_main(int opt_ind)
  {
    if (opt_verbose >= 2)
      {
        std::cerr << "[do_main] Option input = '"
                  << _vm["input"].as<std::string>() << "'\n";
        std::cerr << "[do_main] Option output = '"
                  << _vm["output"].as<std::string>() << "'\n";
      }

    // Opening input/output files with cin/cout
    std::streambuf *cin_rdbuf = (std::streambuf *) NULL;
    std::streambuf *cout_rdbuf = (std::streambuf *) NULL;
    std::istream *is = (std::istream *) NULL;
    std::ostream *os = (std::ostream *) NULL;
    std::ifstream *ifs = (std::ifstream *) NULL;
    std::ofstream *ofs = (std::ofstream *) NULL;
      
    do
      {
        // Setup input file or cin if file name is '-'
        if (_vm["input"].as<std::string>().compare("-") != 0)
          {
            // If input from file, open it
            ifs = new std::ifstream(_vm["input"].as<std::string>().c_str(),
                                    std::ifstream::in);
            if (!ifs->is_open() ||
                (ifs->rdstate() & std::ifstream::failbit) != 0)
              {
                std::cerr << "[do_main] ** Error! failed to open input file '"
                          << _vm["input"].as<std::string>() << "'!\n";
                break;
              }
            
            // Replace cin buf stream with file's one and keep it for restoring
            cin_rdbuf = std::cin.rdbuf(ifs->rdbuf());
            // tie to stdout (for auto flushing)
            std::cin.tie(0);
          }
          
        // Same for output file or cout
        if (_vm["output"].as<std::string>().compare("-") != 0)
          {
            // If output to file, open it
            ofs = new std::ofstream(_vm["output"].as<std::string>().c_str(),
                                    std::ofstream::out);
            if (!ofs->is_open() ||
                (ofs->rdstate() & std::ofstream::failbit) != 0)
              {
                std::cerr << "[do_main] ** Error! failed to open output file '"
                          << _vm["output"].as<std::string>() << "'!\n";
                break;
              }

            // Replace cout buf stream with file's one and keep it for restoring
            cout_rdbuf = std::cout.rdbuf(ofs->rdbuf());
          }

        // Start a directed graph
        std::cout << "digraph {\n";

        // First call for root with all empty/level 0
        create_dot(&std::cin, &std::cout, "", 0);

        // Create vertex in dot file
        std::cout << "}\n";

      } while (0);

    // Restore cin stream buffer if necessary
    if (cin_rdbuf) {
      (void) std::cin.rdbuf(cin_rdbuf);
      // re-tie
      std::cin.tie(0);
    }

    // Restore cout stream buffer if necessary
    if (cout_rdbuf)
      (void) std::cout.rdbuf(cin_rdbuf);

    // Close output file if necessary
    if (ofs)
      ofs->close();

    // Close input file if necessary
    if (ifs)
      ifs->close();

    return 0;
  }

} // namespace clang_ast2dot

static std::string
var2option_mapper(std::string var_name)
{
  // Only process env var starting with CLANG
  if (var_name.substr(0, 5).compare("CLANG") == 0) {
    if (opt_verbose >= 4)
      std::cerr << "[var2option_mapper] checking env var " << var_name
                << "\n";
    if (var_name.compare("CLANG_AST2DOT_INPUT_FILENAME") == 0) {
      if (opt_verbose >= 4)
        std::cerr
          << "[var2option_mapper] env var CLANG_AST2DOT_INPUT_FILENAME  mapped to 'input'\n";
      return (std::string("input"));
    } else if (var_name.compare("CLANG_AST2DOT_OUTPUT_FILENAME") == 0) {
      if (opt_verbose >= 4)
        std::cerr
          << "[var2option_mapper] env var CLANG_AST2DOT_OUTPUT_FILENAME  mapped to 'output'\n";
      return (std::string("output"));
    } else if (var_name.compare("CLANG_AST2DOT_VERBOSE_LEVEL") == 0) {
      if (opt_verbose >= 4)
        std::cerr
          << "[var2option_mapper] env var CLANG_AST2DOT_VERBOSE_LEVEL  mapped to 'verbose'\n";
      return (std::string("verbose"));
    }
    if (opt_verbose >= 4)
      std::cerr << "[var2option_mapper] env var " << var_name
                << " not mapped\n";
  } else if (opt_verbose >= 4)
    std::cerr << "[var2option_mapper] env var " << var_name
              << " discarded\n";

  // Return empty string
  return (std::string(""));
}

static void
compute_verbose(std::vector<std::string> v)
{
  for (std::vector<std::string>::iterator it = v.begin(); it != v.end();
       it++) {
    if (opt_verbose >= 2)
      std::cerr << "[compute_verbose] value '" << *it << "'\n";
    if ((*it).empty())
      opt_verbose = 0;
    else
      opt_verbose += atoi((*it).c_str());
  }
  if (opt_verbose >= 1)
    std::cerr << "[compute_verbose] verbosity set to " << opt_verbose
              << "\n";
}

int main(int argc, char *argv[])
{
  int ret = 1;
  clang_ast2dot::Ast2DotMain the_main(argc, argv);
    
  try
    {
      po::options_description desc("Clang_AST2Dot options");

      std::vector < std::string > implicit_verbose(1);
      implicit_verbose.push_back(std::string("1"));

      std::vector < std::string > default_verbose(1);
      default_verbose.push_back(std::string("0"));

      // Add spec for options
      desc.add_options()
        ("help,h", "CPAD help message")
        ("version,V", "Print version information")
        ("verbose,v:", po::value<std::vector<std::string> >()->implicit_value(implicit_verbose, std::string("1"))->
         default_value(default_verbose, std::string("1"))->
         multitoken()->notifier(compute_verbose), "Verbosity level")
        ("output,o", po::value<std::string>()->default_value(std::string("-")), "Output dot file name: defaults to '-' that is stdout")
        ("input,i", po::value<std::string>()->default_value(std::string("-")), "Input dot file name: defaults to '-' that is stdin")
        ("param", "Extra parameters");

      po::positional_options_description params;
      params.add("param", -1);

      po::store(po::command_line_parser(argc, argv).style(postyle::unix_style).options(desc).positional(params).run(), the_main.vm());
      po::notify(the_main.vm());

      // Process system config file: /etc/clang_ast2dotrc
      try
        {
          std::cerr << "[main] trying to read system config\n";
          std::ifstream ifss(AST2DOT_SYSTEM_CONFIG_FILE_PATH,
                             std::ifstream::in);
          if (!ifss.is_open())
            {
              po::store(po::parse_config_file(ifss,
                                              desc,
                                              true /* allow_unregistered */),
                        the_main.vm());
              po::notify(the_main.vm());
            }
        }
      catch (std::ios_base::failure iosf)
        {
          std::cerr << "Error: Exception while opening system config file ";
          std::cerr << AST2DOT_SYSTEM_CONFIG_FILE_PATH "\n";
          std::cerr << "Error: (" << iosf.code() << "): " << iosf.what() << "\n";
        }

      // Process home config file: $HOME/.clang_ast2dotrc
      try
        {
          std::cerr << "[main] trying to read home config\n";
          std::ifstream ifsh(AST2DOT_HOME_CONFIG_FILE_PATH, std::ifstream::in);
          if (ifsh.is_open())
            {
              po::store(po::parse_config_file(ifsh, desc, true /* allow_unregistered */),
                        the_main.vm());
              po::notify(the_main.vm());
            }
        }
      catch (std::ios_base::failure iosf)
        {
          std::cerr << "Error: Exception while opening home config file ";
          std::cerr << AST2DOT_HOME_CONFIG_FILE_PATH<< "\n";
          std::cerr << "Error: (" << iosf.code() << "): " << iosf.what() << "\n";
        }

      try
        {
          std::cerr << "[main] trying to read local config\n";
          std::ifstream ifsl(AST2DOT_LOCAL_CONFIG_FILE_PATH, std::ifstream::in);
          if (ifsl.is_open())
            {
              po::store(po::parse_config_file(ifsl, desc, true),
                        the_main.vm());
              po::notify(the_main.vm());
            }
        }
      catch (std::ios_base::failure iosf)
        {
          std::cerr << "Error: Exception while opening local config file " AST2DOT_LOCAL_CONFIG_FILE_PATH "\n";
          std::cerr << "Error: (" << iosf.code() << "): " << iosf.what() << "\n";
        }

      // Parse env vars
      po::store(po::parse_environment(desc, var2option_mapper),
                the_main.vm());
      po::notify(the_main.vm());

      // If help requested, display it and terminate
      if (the_main.vm().count("help"))
        std::cerr << desc << "\n";

      else
        {
          // If version & verbose requested, display version
          if (the_main.vm().count("version") && opt_verbose)
            std::cerr << "[main] version is " AST2DOT_VERSION_STRING "\n";
          
          // If version requested, display it and terminate
          if (the_main.vm().count("version"))
            std::cout << argv[0] << "version " AST2DOT_VERSION_STRING "\n";
          
          else
            ret = the_main.do_main(0);
        }
    }
  catch (boost::program_options::error poe)
    {
      std::cerr << "Error: Exception while processing command line options\n";
      std::cerr << "Error: " << poe.what() << "\n";
    }

  return ret;
}
