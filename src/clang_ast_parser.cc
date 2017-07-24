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
	  _inbuf.clear();
	  _scstr.clear();
	  _name.clear();
	  _address.clear();
	  _props = std::vector<std::string>(5);;
	  _is_leaf = false;
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
		    }
		  while(c == '|' || c == ' ' || c == '`');

		  is->unget();
		  
		  if (_scstr.empty())
		    throw Ast2DotParser::EmptyScStrException();
		 		  
		  if ((c = is->get()) != '-')
		    {
		      is->unget();
		      throw Ast2DotParser::InvalidScStrException();
		    }
		  else
		    _scstr.append(1, '-');
                }
	    
            else
	      throw Ast2DotParser::UnexpectedEofException();
            
            return _scstr;
        }

        /**
         * This method quotes some specific quoting string in order for 
         * the quoted string to be handled as one token.
         * ex.: the string 
         *    <<a special quoted string>>
         * will be replaced with
         *    "<<a special quoted string>>"
         * This way, only one token:
         *  '<<a special quoted string>>'
         * will be parsed instead of four tokens:
         *  1. '<<a'
         *  2. 'special'
         *  3. 'quoted'
         *  4. 'string>>'
         */
        std::string&
        Ast2DotParser::quote_special_quotes(std::string& input_buf,
                                            std::string const& in_quoting_str,
                                            std::string const& out_quoting_str,
                                            std::string*& out_buf_ptr)
        {
            // Copy of input buffer
            std::string *inbuf = new std::string(input_buf);
            // Set out_buf_ptr to null
            out_buf_ptr = (std::string*)NULL;
            // Length of in quote string
            int in_len = in_quoting_str.length();
            // Length of out quote string
            int out_len = out_quoting_str.length();
            // curpos (pos of in quote)
            size_t curpos = std::string::npos;
            // lastpos (pos of out quote)
            size_t lastpos = std::string::npos;
            
            // Search the special quoting string to handle, 'in' must be before 'out'
            if ((curpos = input_buf.find(in_quoting_str)) != std::string::npos &&
                (lastpos = input_buf.find(out_quoting_str)) != std::string::npos &&
                curpos < lastpos)
                {
                    // Insert a dbl quote at start of in_quote
                    std::string quote_entry = std::string("\"").append(in_quoting_str);
                    // and replace in_quote in result buf
                    inbuf->replace(curpos, in_len, quote_entry);
                    // Also insert a dbl quote at end for tokenizing the whole string
                    std::string quote_exit = std::string(out_quoting_str).append("\"");
                    // and replace out_quote in result buf 
                    inbuf->replace(inbuf->find(out_quoting_str, curpos + in_len +1), out_len, quote_exit);
                    // Set return ptr
                    out_buf_ptr = inbuf;
                }

            // Return result buf
            return (*inbuf);
        }

        /**
         *
         */
        std::string&
        Ast2DotParser::read_vertex_props(std::istream* is)
        {
            std::string ast;
            std::string* astptr;

            std::getline(*is, ast);

            /*
             * We quote AST special quotes (<<<...>>>, <<...>> and <...>) for
             * having the quoted string in one token
             */
            ast = quote_special_quotes(ast, "<<<", ">>>", astptr);
            ast = quote_special_quotes(ast, "<<", ">>", astptr);
            ast = quote_special_quotes(ast, "<", ">", astptr);
                        
            /*
             * let's start the real tokenizing work
             */
            // Init separator
            boost::escaped_list_separator<char> f("\\", " ", "\\\"");
            // And tokenizer
            boost::tokenizer < boost::escaped_list_separator<char> > tok(ast, f);
            // Start iteration
            boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tok.begin();
        }
        
    } // ! parser
} // ! clang_ast2dot
