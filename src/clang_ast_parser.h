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

            /**
             * Parser explicit constructor
             */
            Ast2DotParser(void);

            /**
             * Parser destructor 
             */ 
            virtual ~Ast2DotParser(void);

            /**
             * Read the string that represent the relationship (edge) between
             * vertex of one line and the vertex of the next line
             */
            std::string& read_sibling_child_string(std::istream* = &std::cin);

            /**
             * Replace special quoting string with quoted one
             */
            std::string& quote_special_quotes(std::string&,
                                              std::string const&,
                                              std::string const&,
                                              std::string*&);

            /**
             * Read properties of a vertex (until end of line)
             */
            std::string& read_vertex_props(std::istream* = &std::cin);

            /**
             * Empty relationship string exception
             */
            class EmptyScStrException : public ::std::runtime_error
            {
              public:
              EmptyScStrException() : ::std::runtime_error("Empty ScStr String Found") {};
                virtual ~EmptyScStrException() {};
            };

            /**
             * Invalid relationship string (this string shall terminate with 1 dash)
             */
            class InvalidScStrException : public ::std::runtime_error
            {
              public:
              InvalidScStrException() : ::std::runtime_error("Invalid ScStr String Found") {};
                virtual ~InvalidScStrException() {};
            };

            /**
             * Unexpected EOF found while parsing the edge string 
             */
            class UnexpectedEofException : public ::std::runtime_error
            {
              public:
              UnexpectedEofException() : std::runtime_error("Unexpected Eof while parsing ScStr") {};
                virtual ~UnexpectedEofException() {};
            };

            /** Current string buffer */
	    std::string& inbuf(void) { return _inbuf; }
            /** Current edge string: relationship between one vertex and the previopus parent one */
	    std::string& scstr(void) { return _scstr; }
            /** Name of the vertex */
	    std::string& name(void) { return _name; }
            /** Address of the vertex (will be part of the vertex ID) */
	    std::string& address(void) { return _address; }
            /** Vector of string used for loading vertex properties */
	    std::vector<std::string>& props(void) { return _props; }
            /** Boolean property set to true if this vertex is a leaf  */
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
