/*
 * @file clang_ast2dot.cc
 */

#include <string.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include<boost/tokenizer.hpp>

#include "clang_ast2dot.h"

#define AST2DOT_VERSION_STRING			"0.0.1"
#define AST2DOT_SYSTEM_CONFIG_FILE_PATH		"/etc/clang_ast2dotrc"
#define AST2DOT_HOME_CONFIG_FILE_PATH		std::string(std::getenv("HOME")).append("/.clang_ast2dotrc").c_str()
#define AST2DOT_LOCAL_CONFIG_FILE_PATH		"./.clang_ast2dotrc"

static int opt_verbose = 0;

using namespace std;
using namespace boost;

namespace clang_ast2dot
{
  Ast2DotMain::Ast2DotMain(int argc, char *argv[])
  {
    _argc = argc;
    for (int i = 0; i < _argc; i++)
      _argv.push_back(std::make_pair(i, std::string(argv[i])));
  }
  
  Ast2DotMain::~Ast2DotMain()
  {
    /* ... */
  }

  po::variables_map& Ast2DotMain::vm(void)
  {
    return _vm;
  }
  
  int Ast2DotMain::do_main(int opt_ind)
  {
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
    if (opt_verbose >= 2)
      {
	std::cerr << "[do_main] Option input = '" << _vm["input"].as<std::string>() << "'\n";
	std::cerr << "[do_main] Option output = '" << _vm["output"].as<std::string>() << "'\n";
      }

    // Opening input/output files with cin/cout
    std::streambuf *cin_rdbuf = (std::streambuf *)NULL;
    std::streambuf *cout_rdbuf = (std::streambuf *)NULL;
    std::istream *is = (std::istream *)NULL;
    std::ostream *os = (std::ostream *)NULL;
    std::ifstream *ifs = (std::ifstream *)NULL;
    std::ofstream *ofs = (std::ofstream *)NULL;
    
    do
      {
	if (!_vm["input"].as<std::string>().compare("-"))
	  {
	    ifs = new std::ifstream(_vm["input"].as<std::string>().c_str(), std::ifstream::in);
	    if ((ifs->rdstate() & std::ifstream::failbit) != 0)
	      {
		std::cerr << "[do_main] ** Error! failed to open input file '" << _vm["input"].as<std::string>() << "'!\n";
		break;
	      }
	    
	    // Replace cin buf stream with file's one and keep it for restoring
	    cin_rdbuf = std::cin.rdbuf(ifs->rdbuf());
	    // tie to stdout (for auto flushing)
	    std::cin.tie(0);
	  }
	if (!_vm["output"].as<std::string>().compare("-"))
	  {
	    ofs = new std::ofstream(_vm["output"].as<std::string>().c_str(), std::ofstream::out);

	    if ((ofs->rdstate() & std::ofstream::failbit) != 0)
	      {
		std::cerr << "[do_main] ** Error! failed to open output file '" << _vm["output"].as<std::string>() << "'!\n";
		break;
	      }

	    // Replace cout buf stream with file's one and keep it for restoring
	    cout_rdbuf = std::cout.rdbuf(ofs->rdbuf());
	  }

	// Now cout eventually sends to output file and cin gets from input file
	std::cout << "digraph {\n";

	std::string inbuf;	// input buffer
        while (!std::cin.eof())
          {
	    std::getline(std::cin, inbuf);

            std::ios_base::iostate rdstate = std::cin.rdstate();
	    if (opt_verbose >= 4)
	      std::cerr << "[do_main] line (" << std::cin.gcount() << "): " << inbuf << "\n";

	    size_t curpos =0;
	    size_t lastpos = 0;
	    
	    if ((curpos = inbuf.find_first_of("<<<")) != std::string::npos)
	      {
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] Processing '<<<...>>>' (3x) enclosed fields\n";
		inbuf.replace(curpos, 3, "\"<<<");
		inbuf.replace((lastpos = inbuf.find_first_of(">>>", curpos+4)), 3, ">>>\"");
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] Field " << inbuf.substr(curpos, lastpos+5) << " processed\n";
	      }
	    else if ((curpos = inbuf.find_first_of("<<")) != std::string::npos)
	      {
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] Processing '<<...>>' (2x) enclosed fields\n";
		inbuf.replace(curpos, 2, "\"<<");
		inbuf.replace((lastpos = inbuf.find_first_of(">>", curpos+3)), 2, ">>\"");
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] Field " << inbuf.substr(curpos, lastpos+4) << " processed\n";
	      }
	    else if ((curpos = inbuf.find_first_of("<")) != std::string::npos)
	      {
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] Processing '<...>' (1x) enclosed fields\n";
		inbuf.replace(curpos, 1, "\"<1");
		inbuf.replace((lastpos = inbuf.find_first_of(">", curpos+2)), 1, ">\"");
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] Field " << inbuf.substr(curpos, lastpos+3) << " processed\n";
	      }

	    boost::escaped_list_separator<char> f("\\", "", "\\\"");
            boost::tokenizer<boost::escaped_list_separator<char> > tok(inbuf, f);
            for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tok.begin();
                it != tok.end();
                ++it)
              {
		if (opt_verbose >= 4)
		  std::cerr << "[do_main] token: '" << *it << "'\n";
              }
          }
      }
    while (0);

    // Restore cin stream buffer if necessary
    if (cin_rdbuf)
      {
	(void)std::cin.rdbuf(cin_rdbuf);
	// re-tie
	std::cin.tie(0);
      }

    // Restore cout stream buffer if necessary
    if (cout_rdbuf)
      (void)std::cout.rdbuf(cin_rdbuf);

    // Close output file if necessary
    if (ofs)
      ofs->close();

    // Close input file if necessary
    if (ifs)
      ifs->close();
  }
} // namespace clang_ast2dot

std::string
var2option_mapper(std::string var_name)
{
  // Only process env var starting with CLANG
  if (var_name.substr(0, 5).compare("CLANG") == 0)
    {
      if (opt_verbose >= 4)
	std::cerr << "[var2option_mapper] checking env var " << var_name << "\n";
      if (var_name.compare("CLANG_AST2DOT_INPUT_FILENAME") == 0)
        {
	  if (opt_verbose >= 4)
	    std::cerr << "[var2option_mapper] env var CLANG_AST2DOT_INPUT_FILENAME  mapped to 'input'\n";
          return (std::string("input"));
        }
      else if (var_name.compare("CLANG_AST2DOT_OUTPUT_FILENAME") == 0)
        {
	  if (opt_verbose >= 4)
	    std::cerr << "[var2option_mapper] env var CLANG_AST2DOT_OUTPUT_FILENAME  mapped to 'output'\n";
          return (std::string("output"));
        }
      else if (var_name.compare("CLANG_AST2DOT_VERBOSE_LEVEL") == 0)
        {
	  if (opt_verbose >= 4)
	    std::cerr << "[var2option_mapper] env var CLANG_AST2DOT_VERBOSE_LEVEL  mapped to 'verbose'\n";
          return (std::string("verbose"));
        }
      if (opt_verbose >= 4)
	std::cerr << "[var2option_mapper] env var " << var_name << " not mapped\n";
    }
  else if (opt_verbose >= 4)
    std::cerr << "[var2option_mapper] env var " << var_name << " discarded\n";

  // Return empty string
  return (std::string(""));
}

void
compute_verbose(std::vector<std::string> v)
{
  for (std::vector<std::string>::iterator it = v.begin();
       it != v.end();
       it++)
    {
      if (opt_verbose >= 2)
	std::cerr << "[compute_verbose] value '" << *it << "'\n";
      if ((*it).empty())
        opt_verbose = 0;
      else
        opt_verbose += atoi((*it).c_str());
    }
  if (opt_verbose >= 1)
    std::cerr << "[compute_verbose] verbosity set to " << opt_verbose << "\n";
}
  
int
main(int argc, char *argv[])
{
  int ret = 1;
  clang_ast2dot::Ast2DotMain the_main(argc, argv);

  try
    {
      po::options_description desc("Clang_AST2Dot options");

      std::vector<std::string> implicit_verbose(1);
      implicit_verbose.push_back(std::string("1"));
      
      std::vector<std::string> default_verbose(1);
      default_verbose.push_back(std::string("0"));
      
      // Add spec for options
      desc.add_options()
	("help,h",
	 "CPAD help message")
	("version,V",
	 "Print version information")
	("verbose,v:",
	 po::value<std::vector<std::string> >()
	     ->implicit_value(implicit_verbose, std::string("1"))
	     ->default_value(default_verbose, std::string("1"))
	     ->multitoken()
	     ->notifier(compute_verbose),
	 "Verbosity level")
	("output,o",
	 po::value<std::string>()->default_value(std::string("-")),
	 "Output dot file name: defaults to '-' that is stdout")
	("input,i",
	 po::value<std::string>()->default_value(std::string("-")),
	 "Input dot file name: defaults to '-' that is stdin")
	("param",
	 "Extra parameters"
	 )
	;
      
      po::positional_options_description params;
      params.add("param", -1);
      
      po::store(po::command_line_parser(argc, argv).
		  style(postyle::unix_style).
		  options(desc).
		  positional(params).
		  run(),
		the_main.vm());
      po::notify(the_main.vm());

      // Process system config file: /etc/clang_ast2dotrc
      try
	{
          std::cerr << "[main] trying to read system config\n";
	  std::ifstream ifss (AST2DOT_SYSTEM_CONFIG_FILE_PATH, std::ifstream::in);
	  po::store(po::parse_config_file(ifss,
					  desc,
					  true /* allow_unregistered */),
		    the_main.vm());
          po::notify(the_main.vm());
        }
      catch (std::ios_base::failure iosf)
	{
	  std::cerr << "Error: Exception while opening system config file "AST2DOT_SYSTEM_CONFIG_FILE_PATH"\n";
	  std::cerr << "Error: " << iosf.what() << "\n";
	}
      catch (std::exception stde)
	{
	  std::cerr << "Error: Exception while opening system config file "AST2DOT_SYSTEM_CONFIG_FILE_PATH"\n";
	  std::cerr << "): " << stde.what() << "\n";
	}

      // Process home config file: $HOME/.clang_ast2dotrc
      try
	{
          std::cerr << "[main] trying to read home config\n";
	  std::ifstream ifsh (AST2DOT_HOME_CONFIG_FILE_PATH, std::ifstream::in);
	  po::store(po::parse_config_file(ifsh,
					  desc,
					  true /* allow_unregistered */),
		    the_main.vm());
          po::notify(the_main.vm());
        }
      catch (std::ios_base::failure iosf)
	{
	  std::cerr << "Error: Exception while opening home config file ";
	  std::cerr << AST2DOT_HOME_CONFIG_FILE_PATH << "\n";
	  std::cerr << "Error: " << iosf.what() << "\n";
	}
      catch (std::exception stde)
	{
	  std::cerr << "Error: Exception while opening system config file ";
	  std::cerr << AST2DOT_HOME_CONFIG_FILE_PATH << "\n";
	  std::cerr << "): " << stde.what() << "\n";
	}

      try
	{
          std::cerr << "[main] trying to read local config\n";
	  std::ifstream ifsl (AST2DOT_LOCAL_CONFIG_FILE_PATH, std::ifstream::in);
	  po::store(po::parse_config_file(ifsl,
					  desc,
					  true /* allow_unregistered */),
		    the_main.vm());
          po::notify(the_main.vm());
        }
      catch (std::ios_base::failure iosf)
	{
	  std::cerr << "Error: Exception while opening local config file "AST2DOT_LOCAL_CONFIG_FILE_PATH"\n";
	  std::cerr << "Error: " << iosf.what() << "\n";
	}
      catch (std::exception stde)
	{
	  std::cerr << "Error: Exception while opening system config file "AST2DOT_LOCAL_CONFIG_FILE_PATH"\n";
	  std::cerr << "): " << stde.what() << "\n";
	}

      // Parse env vars
      po::store(po::parse_environment(desc,
                                      var2option_mapper),
		the_main.vm());
      po::notify(the_main.vm());

      // If help requested, display it and terminate
      if (the_main.vm().count("help"))
	std::cerr << desc << "\n";
      
      else
	{
	  // If version & verbose requested, display version
	  if (the_main.vm().count("version") && opt_verbose)
	    std::cerr << "[main] version is "AST2DOT_VERSION_STRING"\n";

	  // If version requested, display it and terminate
	  if (the_main.vm().count("version"))
	    std::cout << argv[0] << "version "AST2DOT_VERSION_STRING"\n";
	  
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


