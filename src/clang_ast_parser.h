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
                    
            Ast2DotParser(void);
            ~Ast2DotParser(void);
            
            std::string& read_sibling_child_string(std::istream* = &std::cin);
            void read_vertex_props(void);
            
            class EmptyScStrException : public ::std::runtime_error
            {
              public:
              EmptyScStrException() : ::std::runtime_error("Empty ScStr String Found") {};
                virtual ~EmptyScStrException() {};
            };

            class InvalidScStrException : public ::std::runtime_error
            {
              public:
              InvalidScStrException() : ::std::runtime_error("Invalid ScStr String Found") {};
                virtual ~InvalidScStrException() {};
            };
            
            class UnexpectedEofException : public ::std::runtime_error
            {
              public:
              UnexpectedEofException() : std::runtime_error("Unexpected Eof while parsing ScStr") {};
                virtual ~UnexpectedEofException() {};
            };

	    std::string& inbuf(void) { return _inbuf; }
	    std::string& scstr(void) { return _scstr; }
	    std::string& name(void) { return _name; }
	    std::string& address(void) { return _address; }
	    std::vector<std::string>& props(void) { return _props; }
	    bool is_leaf(void) {return _is_leaf;}

	    
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
        };
        
    } // ! namespace parser
} // ! namespace clang_ast2dot

#endif /* ! _CLANG_AST_PARSER_H_ */
