/*
 * @file clang_ast2dot.cc
 */

#include <string.h>
#include <fstream>
#include <iostream>

#include<boost/tokenizer.hpp>

#include "clang_ast2dot.h"

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
	auto the_arg = *argit;
	if (the_arg.first > 0 && the_arg.first >= opt_ind)
	  {
	    std::cout << "Processing arg#" << the_arg.first << " = '" << the_arg.second << "'" << std::endl;
	  }
      }
    std::cout << "Option input = '" << _vm["input"].as<std::string>() << "'\n";
    std::cout << "Option output = '" << _vm["output"].as<std::string>() << "'\n";

    // Opening input file
    std::ifstream ifs (_vm["input"].as<std::string>().c_str(), std::ifstream::in);
    std::ofstream ofs (_vm["output"].as<std::string>().c_str(), std::ifstream::out);

    if ((ifs.rdstate() & std::ifstream::failbit) != 0)
      std::cerr << "[do_main] failed to open input file '" << _vm["input"].as<std::string>() << "'!\n";

    else if ((ofs.rdstate() & std::ofstream::failbit) != 0)
      std::cerr << "[do_main] failed to open output file '" << _vm["output"].as<std::string>() << "'!\n";

    else
      {
        ofs << "digraph {\n";
        
        size_t cur_size = 1024;
        while (!ifs.eof())
          {
            char* buf = new char[cur_size];
            ifs.getline(buf, cur_size);
            std::ios_base::iostate rdstate = ifs.rdstate();
            while ((rdstate & std::ifstream::failbit) != 0 && (rdstate & std::ifstream::eofbit) == 0)
              {
                std::cout << "[do_main] buf too small: double size to " << cur_size << " bytes\n";
                std::cout << "[do_main] line (" << ifs.gcount() << "): " << buf << "\n";
                size_t old_size = cur_size;
                cur_size = cur_size * 2;
                char* nbuf = new char[cur_size];
                memcpy(nbuf, buf, old_size);
                delete[] buf;
                ifs.getline(nbuf+old_size, cur_size - old_size);
                rdstate = ifs.rdstate();
                buf = nbuf;
                std::cout << "[do_main] new line (" << ifs.gcount() << "): " << nbuf << "\n";
              }
            std::cout << "[do_main] line (" << ifs.gcount() << "): " << buf << "\n";

            boost::tokenizer<boost::escaped_list_separator<char> > tok(buf);
            for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator beg = tok.begin();
                beg != tok.end();
                ++beg)
              {
                std::cout << "[do_main] token: '" << *beg << "'\n";
              }
          }
        ofs.close();
        ifs.close();
      }    
  }
} // namespace clang_ast2dot

std::string
var2option_mapper(std::string var_name)
{
  if (var_name.substr(0, 5).compare("CLANG") == 0)
    {
      std::cout << "[var2option_mapper] checking env var " << var_name << "\n";
      if (var_name.compare("CLANG_AST2DOT_INPUT_FILENAME") == 0)
        {
          std::cout << "[var2option_mapper] env var CLANG_AST2DOT_INPUT_FILENAME  mapped to 'input'\n";
          return (std::string("input"));
        }
      else if (var_name.compare("CLANG_AST2DOT_OUTPUT_FILENAME") == 0)
        {
          std::cout << "[var2option_mapper] env var CLANG_AST2DOT_OUTPUT_FILENAME  mapped to 'output'\n";
          return (std::string("output"));
        }
      else if (var_name.compare("CLANG_AST2DOT_VERBOSE_LEVEL") == 0)
        {
          std::cout << "[var2option_mapper] env var CLANG_AST2DOT_VERBOSE_LEVEL  mapped to 'verbose'\n";
          return (std::string("verbose"));
        }
      std::cout << "[var2option_mapper] env var " << var_name << " not mapped\n";
    }
  else
    std::cout << "[var2option_mapper] env var " << var_name << " discarded\n";
  
  return (std::string());
}

void
compute_verbose(std::vector<std::string> v)
{
  for (std::vector<std::string>::iterator it = v.begin();
       it != v.end();
       it++)
    {
      std::cout << "[compute_verbose] value '" << *it << "'\n";
      if ((*it).empty())
        opt_verbose = 0;
      else
        opt_verbose += atoi((*it).c_str());
    }
  std::cout << "[compute_verbose] verbosity set to " << opt_verbose << "\n";
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
      implicit_verbose.push_back(std::string("0"));
      
      // Add spec for options
      desc.add_options()
	("help,h", "CPAD help message")
	("version,V", "Print version information")
	("verbose,v:", po::value<std::vector<std::string> >()->implicit_value(implicit_verbose, std::string("1"))->default_value(default_verbose, std::string("1"))->multitoken()->notifier(compute_verbose), "Verbosity level")
	("output,o", po::value<std::string>()->default_value(std::string("-")), "Output dot file name: defaults to '-' that is stdout")
	("input,i", po::value<std::string>()->default_value(std::string("-")), "Input dot file name: defaults to '-' that is stdin")
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
      
      try
	{
          std::cout << "trying to read system config\n";
	  std::ifstream ifss ("/etc/clang_ast2dotrc", std::ifstream::in);
	  po::store(po::parse_config_file(ifss,
					  desc,
					  true /* allow_unregistered */),
		    the_main.vm());
          po::notify(the_main.vm());
        }
      catch (std::ios_base::failure iosf)
	{
	  std::cerr << "Error: Exception while opening system config file /etc/clang_ast2dotrc\n";
	  std::cerr << "Error: (#" << iosf.code() << "): " << iosf.what() << "\n";
	}
      catch (std::exception stde)
	{
	  std::cerr << "Error: Exception while opening system config file /etc/clang_ast2dotrc\n";
	  std::cerr << "): " << stde.what() << "\n";
	}

      try
	{
          std::cout << "trying to read home config\n";
	  std::ifstream ifsh (std::string(getenv("HOME")).append("/.clang_ast2dotrc"), std::ifstream::in);
	  po::store(po::parse_config_file(ifsh,
					  desc,
					  true /* allow_unregistered */),
		    the_main.vm());
          po::notify(the_main.vm());
        }
      catch (std::ios_base::failure iosf)
	{
	  std::cerr << "Error: Exception while opening home config file ~/.clang_ast2dotrc\n";
	  std::cerr << "Error: (#" << iosf.code() << "): " << iosf.what() << "\n";
	}
      catch (std::exception stde)
	{
	  std::cerr << "Error: Exception while opening system config file ~/.clang_ast2dotrc\n";
	  std::cerr << "): " << stde.what() << "\n";
	}

      try
	{
          std::cout << "trying to read local config\n";
	  std::ifstream ifsl ("./.clang_ast2dotrc", std::ifstream::in);
	  po::store(po::parse_config_file(ifsl,
					  desc,
					  true /* allow_unregistered */),
		    the_main.vm());
          po::notify(the_main.vm());
        }
      catch (std::ios_base::failure iosf)
	{
	  std::cerr << "Error: Exception while opening local config file .clang_ast2dotrc\n";
	  std::cerr << "Error: (#" << iosf.code() << "): " << iosf.what() << "\n";
	}
      catch (std::exception stde)
	{
	  std::cerr << "Error: Exception while opening system config file .clang_ast2dotrc\n";
	  std::cerr << "): " << stde.what() << "\n";
	}

      po::store(po::parse_environment(desc,
                                      var2option_mapper),
		the_main.vm());
      po::notify(the_main.vm());
  
      if (the_main.vm().count("help"))
	std::cout << desc << "\n";
      
      else
	{
	  if (the_main.vm().count("version") && opt_verbose)
	    std::cout << "* version is 0.0.1\n";
	  
	  if (the_main.vm().count("version"))
	    std::cout << argv[0] << " version 0.0.1\n";
	  else
	    {
	      ret = the_main.do_main(0);
	    }
	}
    }
  catch (boost::program_options::error poe)
    {
      std::cerr << "Error: Exception while processing command line options\n";
      std::cerr << "Error: " << poe.what() << "\n";
    }
    
  return ret;
}


