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
#include <sstream>

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
     * Escape utf chars and some special chars for URL/HTML
     * (or equivalent) output
     */
    std::string&
    escape_dot_string(std::string& str)
    {
      // Found character position
      size_t pos = 0;
      // Length of escaped sequence
      size_t ilen = 0;
          
      do
	{
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
	}
      while (pos != std::string::npos);
          
      return str;
    }

    /**
     * Ast2DotParser Constructor
     */
    Ast2DotParser::Ast2DotParser()
    {
      //std::cerr << "Ast2DotParser::Ast2DotParser\n";
      _inbuf.clear();
      _scstr.clear();
      _name.clear();
      _label.clear();
      _address.clear();
      _props = std::vector<std::string>(5);
      _is_null = false;
      _name2null.clear();
      _name2public.clear();
    }
        
    /** 
     * Ast2DotParser Destructor
     */
    Ast2DotParser::~Ast2DotParser()
    {
      //std::cerr << "Ast2DotParser::~Ast2DotParser\n";
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
      //std::cerr << "Ast2DotParser::read_sibling_child_string\n";
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
      //std::cerr << "Ast2DotParser::quote_special_quotes\n";
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
     * Provide a name for a null vertex.
     * If ix is -1, add a new null else return indexed one
     *
     * @param ix index of the null vertex in parsing order
     *
     * @return name for this null vertex (as NULL_1)
     *
     */
    std::string
    Ast2DotParser::null_to_name(int const& ix)
    {
      //std::cerr << "Ast2DotParser::null_to_name\n";

      std::stringbuf strbuf("", std::ios_base::out);
      std::ostream ostr(&strbuf);

      int mapsz = _name2null.size();
      
      if (ix == -1 || ix >= mapsz)
	{
	  int next_ix = mapsz;
	  ostr << "NULL_" << next_ix;
	  _name2null[next_ix] = strbuf.str();
	  return _name2null[next_ix];
	}

      else if (ix >= 0 && ix < mapsz)
	return _name2null[ix];

    }
        
    /**
     * Provide a label for a null vertex.
     *
     * @param name  name of the null vertex
     *
     * @return label for this null vertex (as <<<NULL_#1>>>)
     *
     */
    std::string
    Ast2DotParser::null_to_label(std::string const& name)
    {
      //std::cerr << "Ast2DotParser::null_to_label\n";

      std::stringbuf strbuf("", std::ios_base::out);
      std::ostream ostr(&strbuf);

      std::map<int, std::string>::iterator it = _name2null.begin();

      do
	{
	  if (it->second.compare(name) == 0)
	    break;
	  it++;
	}
      while (it != _name2null.end());
      
      if (it != _name2null.end())
	{
	  ostr << "&lt;&lt;&lt;NULL_" << it->first << "&gt;&gt;&gt;";
	  return strbuf.str();
	}

      else 
	return std::string("");
    }
        
    /**
     * Provide a name for a public vertex.
     * If ix is -1, add a new public else return indexed one
     *
     * @param ix index of the public vertex in parsing orderame
     *
     * @return name for this public vertex (as NULL_1)
     *
     */
    std::string
    Ast2DotParser::public_to_name(int const& ix, int* allocated_ix)
    {
      //std::cerr << "Ast2DotParser::public_to_name\n";

      std::stringbuf strbuf("", std::ios_base::out);
      std::ostream ostr(&strbuf);

      int mapsz = _name2public.size();
      
      if (ix == -1 || ix >= mapsz)
	{
	  int next_ix = mapsz;
	  ostr << "public_" << next_ix;
	  _name2public[next_ix] = strbuf.str();

	  if (allocated_ix)
	    *allocated_ix = next_ix;

	  return _name2public[next_ix];
	}

      else if (ix >= 0 && ix < mapsz)
	return _name2public[ix];

      return std::string("");
    }
        
    /**
     * Provide a label for a public vertex.
     *
     * @param name  name of the public vertex
     *
     * @return label for this public vertex (as PUBLIC_0)
     *
     */
    std::string
    Ast2DotParser::public_to_label(std::string const& name)
    {
      //std::cerr << "Ast2DotParser::public_to_label\n";

      std::stringbuf strbuf("", std::ios_base::out);
      std::ostream ostr(&strbuf);

      std::map<int, std::string>::iterator it = _name2public.begin();

      do
	{
	  if (it->second.compare(name) == 0)
	    break;
	  it++;
	}
      while (it != _name2public.end());
      
      if (it != _name2public.end())
	{
	  ostr << "public_" << it->first;
	  return strbuf.str();
	}

      else 
	return std::string("");
    }

    /**
     * Provide a public node index from the name registered in the map
     *
     * @retval -1 if the name was not found in the map
     *
     * @return index
     *
     */
    std::string
    Ast2DotParser::public_to_index(std::string const& name)
    {
      //std::cerr << "Ast2DotParser::public_to_label\n";

      std::stringbuf strbuf("", std::ios_base::out);
      std::ostream ostr(&strbuf);

      std::map<int, std::string>::iterator it = _name2public.begin();

      do
	{
	  if (it->second.compare(name) == 0)
	    break;
	  it++;
	}
      while (it != _name2public.end());
      
      if (it != _name2public.end())
	ostr << it->first;

      else 
	ostr << -1;

      return strbuf.str();
    }
      
    /**
     *
     */
    std::string*
    Ast2DotParser::read_vertex_props(std::istream* is, std::ostream* os)
    {
      //std::cerr << "Ast2DotParser::read_vertex_props\n";

      std::string ast;
      std::string* astptr;

      std::string *ret = new std::string("");
            
      std::getline(*is, ast);

      if (!ast.empty())
	{
          /*
	   * Replace ":" with "\:"
	   */
	  size_t fpos = 0;
	  do
	    {
	      fpos = ast.find(":", fpos+3);
	      if (fpos >= 0 && fpos != std::string::npos)
		{
		  std::string::iterator rstart = ast.begin() + fpos; 
		  ast.insert(rstart, 1, '\\'); 
		}
	    }
	  while (fpos != std::string::npos);

	  /*
	   * We quote AST special quotes (<<<...>>>, <<...>> and <...>) for
	   * having the quoted string in one token
	   */
	  if (ast.find("<<<") != std::string::npos)
	    ast = quote_special_quotes(ast, "<<<", ">>>", astptr);
	  if (ast.find("<<") != std::string::npos &&
	      ast.find("<<<") == std::string::npos)
	    ast = quote_special_quotes(ast, "<<", ">>", astptr);
	  if (ast.find("<") != std::string::npos &&
	      ast.find("<<") == std::string::npos &&
	      ast.find("<<<") == std::string::npos)
	    ast = quote_special_quotes(ast, "<", ">", astptr);

	  /*
	   * let's start the real tokenizing work
	   */
	  // Init separator
	  boost::escaped_list_separator<char> f("\\", " ", "\\\"");
	  // And tokenizer
	  boost::tokenizer<boost::escaped_list_separator<char> > tok(ast, f);
	  // Start iteration
	  boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tok.begin();

	  _is_null = false;
	  _name.clear();
	  _label.clear();
	  _address.clear();
	  _props.clear();
                
	  do
	    {
	      // Name & address are always available but for <<<null>>>, where we have only this token
	      _name = (*it);
	      it++;
	      if (it == tok.end())
		break;

	      /*
	       * handle some particular node cases ...
	       */

	      // ------------------------------------------
	      // CXX Constructor Initializer
	      if (_name.compare("CXXCtorInitializer") == 0)
		{
		  // Get next token and test if need to be skipped
		  std::string tmp = (*it);
		  it++;
		  if (it == tok.end())
		    break;

		  // If 'Field' next token is address
		  if (tmp.compare("Field") == 0)
		    {
		      _address = (*it);
		      it++;
		      if (it == tok.end())
			break;		      
		    }
		}

	      // ------------------------------------------
	      // original namespace
	      else if (_name.compare("original") == 0)
		{
		  _name.append(*it);
		  // Address
		  _address = (*it);
		  it++;
		  if (it == tok.end())
		    break;
		}

	      // ------------------------------------------
	      // public, protected & private
	      else if (_name.compare("public") != 0 &&
		       _name.compare("protected") != 0 &&
		       _name.compare("private") != 0)
		{
		  // Address
		  _address = (*it);
		  it++;
		  if (it == tok.end())
		    break;
		}

	      // ------------------------------------------
	      // Overrides
	      else if (_name.compare("Overrides:") != 0)
		{
		  // Remove ':'
		  _name.pop_back();
		  
		  // Skip '['
		  it++;
		  // Read address
		  _address = (*it);
		  it++;
		  if (it == tok.end())
		    break;
		}
                    
	      for (; it != tok.end(); ++it)
		_props.push_back((*it));
	    }
	  while (it != tok.end());
                
	  std::stringbuf strbuf("", std::ios_base::out);
	  std::ostream ostr(&strbuf);

	  if (_name.compare("<<<NULL>>>") == 0)
	    {
	      _is_null = true;
	      _name = null_to_name(-1);
	      std::cerr << "NULL name = " << _name << "\n"; 
	      _label = null_to_label(_name);
	    }
	  else if (_name.compare("public") == 0)
	    {
	      _name = public_to_name(-1);
	      std::cerr << "public name = " << _name << "\n"; 
	      _address = public_to_index(_name);
	      _label = public_to_label(_name);
	    }
	  else
	    _label = _name;

	  ostr << "    " << _name;
	  if (!_address.empty())
	    ostr << "_" << _address;
                
	  ostr << " [shape=record,style=filled,fillcolor=lightgrey,label=\"{ ";
	  ostr << escape_dot_string(_label) << "| ";
                
	  if (!_address.empty())
	    ostr << _address << "| ";

	  for (std::vector<std::string>::iterator svit = _props.begin();
	       svit != _props.end();
	       ++svit)
	    ostr << escape_dot_string(*svit) << "| ";
                
	  ostr << "}\"];\n";
                
	  ret = new std::string(strbuf.str());
	}
            
      return ret;
    }
        
  } // ! parser
} // ! clang_ast2dot
