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
#include <iostream>
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
            virtual std::string& read_sibling_child_string(std::istream *is = &std::cin);

            /**
             * Replace special quoting string with quoted one
             */
            virtual std::string& quote_special_quotes(std::string&,
                                                      std::string const&,
                                                      std::string const&,
                                                      std::string*&);

            /*
             * Escape vertex properties strings special characters
             * (ex. < with &lt;). Almost the same as json/html
             */

            /*
             * Provide a name for a null vertex.
             * If ix is -1, add a new null else return indexed one
             */
            virtual std::string null_to_name(int const& ix = -1);

            /*
             * Provide a label for a null vertex.
             */
            virtual std::string null_to_label(std::string const&);

            /**
             * Read properties of a vertex (until end of line)
             */
            virtual std::string* read_vertex_props(std::istream *, std::ostream *);

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
            virtual std::string& inbuf(void) { return _inbuf; }

            /** Current edge string: relationship between one vertex and the previopus parent one */
            virtual std::string& scstr(void) { return _scstr; }

            /** Is vertex is null */
            virtual bool is_null(void) { return _is_null; }

            /** Name of the vertex */
            virtual std::string& name(void) { return _name; }

            /** Label of the vertex */
            virtual std::string& label(void) { return _label; }

            /** Address of the vertex (will be part of the vertex ID) */
            virtual std::string& address(void) { return _address; }

            /** Vector of string used for loading vertex properties */
            virtual std::vector<std::string>& props(void) { return _props; }
            
          private:
            // Line buffer
            std::string _inbuf;
            // Edges
            std::string _scstr;
            // Is null
            bool _is_null;
            // Vertex name
            std::string _name;
            // Vertex label
            std::string _label;
            // Vertex address
            std::string _address;
            // Other props (class dependent)
            std::vector<std::string> _props;
            // Null vertex are numbered
            std::map<int, std::string> _name2null;
        };
        
    } // ! namespace parser
} // ! namespace clang_ast2dot

#endif /* ! _CLANG_AST_PARSER_H_ */
