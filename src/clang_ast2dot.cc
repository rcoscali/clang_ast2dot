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

/*
 * Constants definitions
 */
// Version string
#define AST2DOT_VERSION_STRING                  "0.0.1"
// System config file
#define AST2DOT_SYSTEM_CONFIG_FILE_PATH         "/etc/clang_ast2dotrc"
// User config file
#define AST2DOT_HOME_CONFIG_FILE_PATH           std::string(std::getenv("HOME")).append("/.clang_ast2dotrc").c_str()
// Local directory config file
#define AST2DOT_LOCAL_CONFIG_FILE_PATH          "./.clang_ast2dotrc"

// Verbose level
static int opt_verbose = 0;

// Use std and boost namespaces
using namespace std;
using namespace boost;

/*
 * Escape special chars for nice handling in graphviz
 *
 * @param str   string in which to escape characters
 *
 * @return the string with special dot characters escaped
 */
static std::string&
escaped_dot_string(std::string& str)
{
  // Found character position
  size_t pos = 0;
  // Length of escaped sequence
  size_t ilen = 0;
  
  do
    {
      std::cerr << "[escaped_dot_string] str = '" << str << "'\n";
      pos = str.find_first_of("<>\\ ", pos + ilen);
      if (pos != std::string::npos)
        switch (str[pos]) {
        default:
          break;
        case '<':
          str.replace(pos, 1, "&lt;");
          ilen = 4;
          break;
        case '>':
          str.replace(pos, 1, "&gt;");
          ilen = 4;
          break;
        case ' ':
          str.replace(pos, 1, "&nbsp;");
          ilen = 6;
          break;
        case '\\':
          str = str.insert(pos, 1, '\\');
          ilen = 1;
          break;
        }
      
      std::cerr << "[escaped_dot_string] pos = " << pos << "\n";
    }
  while (pos != std::string::npos);

  return str;
}

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
    _re = boost::regex("^((\\| )|( \\|)|(  ))*(\\|-|`-)$");
  }

  /*
   * Virtual destructor
   */
  Ast2DotMain::~Ast2DotMain()
  {
  }

  po::variables_map& Ast2DotMain::vm(void) {
    return _vm;
  }

  /*
   *
   */
  

  /*
   *
   */
  bool
  Ast2DotMain::create_dot()
  {
    bool goes_on = true;
    bool generated = false;

    // While not end of file
    while (!std::cin.eof() && goes_on)
      {
	if (inbuf = read_ast_line())
	  {

	    /*
           * We quote AST special quotes (<<<...>>>, <<...>> and <...>) for
           * having the quoted string in one token
           */
          // Search for some '<<<' to replace
          if ((curpos = inbuf.find("<<<")) != std::string::npos) {
            if (opt_verbose >= 4)
              std::cerr
                << "[do_main] Processing '<<<...>>>' (3x) enclosed fields\n";
            // Insert a dbl quote at start
            inbuf.replace(curpos, 3, "\"<<<");
            // Also insert a dbl quote at end for tokenizing the whole string
            inbuf.replace((lastpos = inbuf.find(">>>", curpos + 4)), 3,
                          ">>>\"");
            if (opt_verbose >= 4)
              std::cerr << "[do_main] Field "
                        << inbuf.substr(curpos, lastpos + 1)
                        << " processed\n";
          }
          // Search for some '<<' to replace
          else if ((curpos = inbuf.find("<<")) != std::string::npos) {
            if (opt_verbose >= 4)
              std::cerr
                << "[do_main] Processing '<<...>>' (2x) enclosed fields\n";
            // Insert a dbl quote at start
            inbuf.replace(curpos, 2, "\"<<");
            // Also insert a dbl quote at end for tokenizing the whole string
            inbuf.replace((lastpos = inbuf.find(">>", curpos + 3)), 2,
                          ">>\"");
            if (opt_verbose >= 4)
              std::cerr << "[do_main] Field "
                        << inbuf.substr(curpos, lastpos + 1)
                        << " processed\n";
          }
          // Or finally, search for some '<' to replace
          else if ((curpos = inbuf.find("<")) != std::string::npos) {
            if (opt_verbose >= 4)
              std::cerr
                << "[do_main] Processing '<...>' (1x) enclosed fields\n";
            // Insert a dbl quote at start
            inbuf.replace(curpos, 1, "\"<");
            // Also insert a dbl quote at end for tokenizing the whole string
            inbuf.replace((lastpos = inbuf.find(">", curpos + 2)), 1,
                          ">\"");
            if (opt_verbose >= 4)
              std::cerr << "[do_main] Field "
                        << inbuf.substr(curpos, lastpos + 1)
                        << " processed\n";
          }

          /*
           * let's start the real tokenizing work
           */
          // Init separator
          boost::escaped_list_separator<char> f("\\", " ", "\\\"");
          // And tokenizer
          boost::tokenizer < boost::escaped_list_separator<char> > tok(inbuf, f);
          // Start iteration
          boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tok.begin();

          int new_level = level;
          std::string vertex_name;        // current vertex name string
          std::string token_scstr;        // sibling/child preprend string

          // If not at root
          if (!(inbuf[0] >= 'A' && inbuf[0] <= 'Z')) {
            // Get the child/Sibling string
            token_scstr = (*it);
            if (opt_verbose >= 4)
              std::cerr << "[do_main] scstr: '" << token_scstr << "'\n";
            // Get next token
            it++;
            new_level = token_scstr.length() / 2;
            if (new_level > level)
              create_graph(token_name, )
                if (boost::regex_match(token_scstr, _what, _re)) {
                  std::cerr << "** Match found **\n   Sub-Expressions:\n";
                  for(int iw = 0; iw < _what.size(); ++iw)
                    std::cout << "      $" << iw << " = '" << _what[iw] << "'\n";
                }
          }
          else
            token_scstr = scstr;

          // If not at end of tokens
          if (it != tok.end())
            do {
              // Read vertex name from AST class name
              std::string token_name = (*it);
              if (opt_verbose >= 4)
                std::cerr << "[do_main] token_name: '" << token_name
                          << "'\n";

              // Forwards to next token
              it++;
              if (it == tok.end())
                break;

              // Read vertex name suffix from AST class instance address
              std::string token_address = (*it);
              if (opt_verbose >= 4)
                std::cerr << "[do_main] token_address: '"
                          << token_address << "'\n";

              // Compute vertex name
              vertex_name = token_name.append(
                                              std::string("_").append(token_address));
              // Create vertex in dot file
              std::cout << "    " << vertex_name
                        << " [shape=record,style=filled,fillcolor=lightgrey,label=\"{ "
                        << token_name << " ";

              // Read all remaining AST instance properties
              do {
                // Forwards to next token
                it++;
                if (it != tok.end()) {
                  std::string token_prop = (*it);
                  if (opt_verbose >= 4)
                    std::cerr << "[do_main] token: '" << token_prop
                              << "'\n";

                  std::cout << "|" << escaped_dot_string(token_prop)
                            << " ";
                }
              } while (it != tok.end());

              // Create vertex in dot file
              std::cout << "}\"];\n";

              // At root, recurs if needed (while sibling == "|-")
              if (token_scstr.empty()) {
                goes_on = create_graph(vertex_name, token_scstr,
                                       level + 1);
                parent_name = vertex_name;
              }

              // Not at root, recurs if "|-"
              else {
                // Create edge in dot file
                std::cout << "    " << parent_name << " -> "
                          << vertex_name
                          << " [style=\"solid\",color=black,weight=100,constraint=true];\n";
                previous_scstr = token_scstr;
                                        
              } while (0);
            }
        }
      }

    /*
     *
     */
    int Ast2DotMain::do_main(int opt_ind)
    {
      /*
        for (std::vector<std::pair<int, std::string> >::iterator argit = _argv.begin();
        argit != _argv.end();
        argit++)
        {
        std::pair<int, std::string> the_arg = *argit;
        if (the_arg.first > 0 && the_arg.first >= opt_ind)
        {
        if (opt_verbose >= 3)
        std::cerr << "Processing arg#" << the_arg.first << " = '" << the_arg.second << "'" << std::endl;
        }
        }
      */
      if (opt_verbose >= 2) {
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

      do {
        // Setup input file or cin
        if (_vm["input"].as<std::string>().compare("-") != 0) {
          // If input from file, open it
          ifs = new std::ifstream(_vm["input"].as<std::string>().c_str(),
                                  std::ifstream::in);
          if (!ifs->is_open()
              || (ifs->rdstate() & std::ifstream::failbit) != 0) {
            std::cerr << "[do_main] ** Error! failed to open input file '"
                      << _vm["input"].as<std::string>() << "'!\n";
            break;
          }

          // Replace cin buf stream with file's one and keep it for restoring
          cin_rdbuf = std::cin.rdbuf(ifs->rdbuf());
          // tie to stdout (for auto flushing)
          std::cin.tie(0);
        }
        // Setup output file or cout
        if (_vm["output"].as<std::string>().compare("-") != 0) {
          // If output to file, open it
          ofs = new std::ofstream(_vm["output"].as<std::string>().c_str(),
                                  std::ofstream::out);
          if (!ofs->is_open()
              || (ofs->rdstate() & std::ofstream::failbit) != 0) {
            std::cerr << "[do_main] ** Error! failed to open output file '"
                      << _vm["output"].as<std::string>() << "'!\n";
            break;
          }

          // Replace cout buf stream with file's one and keep it for restoring
          cout_rdbuf = std::cout.rdbuf(ofs->rdbuf());
        }

        // Now cout eventually sends to output file and cin gets from input file
        // Start directed graph
        std::cout << "digraph {\n";

        // First call for root with all empty/level 0
        std::string inbuf;
        std::string parent_name;
        std::string scstr;
        int level = 0;
        (void) create_graph(inbuf, parent_name, scstr, level);

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
    }
  } // namespace clang_ast2dot

  static std::string var2option_mapper(std::string var_name) {
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

  static void compute_verbose(std::vector<std::string> v) {
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

  int main(int argc, char *argv[]) {
    int ret = 1;
    clang_ast2dot::Ast2DotMain the_main(argc, argv);

    try {
      po::options_description desc("Clang_AST2Dot options");

      std::vector < std::string > implicit_verbose(1);
      implicit_verbose.push_back(std::string("1"));

      std::vector < std::string > default_verbose(1);
      default_verbose.push_back(std::string("0"));

      // Add spec for options
      desc.add_options()("help,h", "CPAD help message")("version,V",
                                                        "Print version information")("verbose,v:",
                                                                                     po::value<std::vector<std::string> >()->implicit_value(
                                                                                                                                            implicit_verbose, std::string("1"))->default_value(
                                                                                                                                                                                               default_verbose, std::string("1"))->multitoken()->notifier(
                                                                                                                                                                                                                                                          compute_verbose), "Verbosity level")("output,o",
                                                                                                                                                                                                                                                                                               po::value<std::string>()->default_value(std::string("-")),
                                                                                                                                                                                                                                                                                               "Output dot file name: defaults to '-' that is stdout")(
                                                                                                                                                                                                                                                                                                                                                       "input,i",
                                                                                                                                                                                                                                                                                                                                                       po::value<std::string>()->default_value(std::string("-")),
                                                                                                                                                                                                                                                                                                                                                       "Input dot file name: defaults to '-' that is stdin")("param",
                                                                                                                                                                                                                                                                                                                                                                                                             "Extra parameters");

      po::positional_options_description params;
      params.add("param", -1);

      po::store(
                po::command_line_parser(argc, argv).style(postyle::unix_style).options(
                                                                                       desc).positional(params).run(), the_main.vm());
      po::notify(the_main.vm());

      // Process system config file: /etc/clang_ast2dotrc
      try {
        std::cerr << "[main] trying to read system config\n";
        std::ifstream ifss(AST2DOT_SYSTEM_CONFIG_FILE_PATH,
                           std::ifstream::in);
        po::store(
                  po::parse_config_file(ifss, desc,
                                        true /* allow_unregistered */), the_main.vm());
        po::notify(the_main.vm());
      } catch (std::ios_base::failure iosf) {
        std::cerr
          << "Error: Exception while opening system config file " AST2DOT_SYSTEM_CONFIG_FILE_PATH "\n";
        std::cerr << "Error: " << iosf.what() << "\n";
      } catch (std::exception stde) {
        std::cerr
          << "Error: Exception while opening system config file " AST2DOT_SYSTEM_CONFIG_FILE_PATH "\n";
        std::cerr << "): " << stde.what() << "\n";
      }

      // Process home config file: $HOME/.clang_ast2dotrc
      try {
        std::cerr << "[main] trying to read home config\n";
        std::ifstream ifsh(AST2DOT_HOME_CONFIG_FILE_PATH, std::ifstream::in);
        po::store(
                  po::parse_config_file(ifsh, desc,
                                        true /* allow_unregistered */), the_main.vm());
        po::notify(the_main.vm());
      } catch (std::ios_base::failure iosf) {
        std::cerr << "Error: Exception while opening home config file ";
        std::cerr << AST2DOT_HOME_CONFIG_FILE_PATH<< "\n";
        std::cerr << "Error: " << iosf.what() << "\n";
      } catch (std::exception stde) {
        std::cerr << "Error: Exception while opening system config file ";
        std::cerr << AST2DOT_HOME_CONFIG_FILE_PATH<< "\n";
        std::cerr << "): " << stde.what() << "\n";
      }

      try {
        std::cerr << "[main] trying to read local config\n";
        std::ifstream ifsl(AST2DOT_LOCAL_CONFIG_FILE_PATH,
                           std::ifstream::in);
        po::store(
                  po::parse_config_file(ifsl, desc,
                                        true /* allow_unregistered */), the_main.vm());
        po::notify(the_main.vm());
      } catch (std::ios_base::failure iosf) {
        std::cerr
          << "Error: Exception while opening local config file " AST2DOT_LOCAL_CONFIG_FILE_PATH "\n";
        std::cerr << "Error: " << iosf.what() << "\n";
      } catch (std::exception stde) {
        std::cerr
          << "Error: Exception while opening system config file " AST2DOT_LOCAL_CONFIG_FILE_PATH "\n";
        std::cerr << "): " << stde.what() << "\n";
      }

      // Parse env vars
      po::store(po::parse_environment(desc, var2option_mapper),
                the_main.vm());
      po::notify(the_main.vm());

      // If help requested, display it and terminate
      if (the_main.vm().count("help"))
        std::cerr << desc << "\n";

      else {
        // If version & verbose requested, display version
        if (the_main.vm().count("version") && opt_verbose)
          std::cerr << "[main] version is " AST2DOT_VERSION_STRING "\n";

        // If version requested, display it and terminate
        if (the_main.vm().count("version"))
          std::cout << argv[0] << "version " AST2DOT_VERSION_STRING "\n";

        else
          ret = the_main.do_main(0);
      }
    } catch (boost::program_options::error poe) {
      std::cerr << "Error: Exception while processing command line options\n";
      std::cerr << "Error: " << poe.what() << "\n";
    }

    return ret;
  }
