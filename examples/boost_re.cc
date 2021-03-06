#include <boost/regex.hpp>
#include <iostream>

void print_captures(const std::string& regx, const std::string& text)
{
   boost::regex e(regx);
   boost::smatch what;
   std::cout << "Expression:  \"" << regx << "\"\n";
   std::cout << "Text:        \"" << text << "\"\n";
#ifdef USE_RE_CAPTURES
   if(boost::regex_match(text, what, e, boost::match_extra))
#else     
   if(boost::regex_match(text, what, e))
#endif     
   {
      unsigned i, j;
      std::cout << "** Match found **\n   Sub-Expressions:\n";
      for(i = 0; i < what.size(); ++i)
         std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
#ifdef USE_RE_CAPTURES
      std::cout << "   Captures:\n";
      for(i = 0; i < what.size(); ++i)
      {
         std::cout << "      $" << i << " = {";
         for(j = 0; j < what.captures(i).size(); ++j)
         {
            if(j)
               std::cout << ", ";
            else
               std::cout << " ";
            std::cout << "\"" << what.captures(i)[j] << "\"";
         }
         std::cout << " }\n";
      }
#endif
   }
   else
   {
      std::cout << "** No Match found **\n";
   }
}

int main(int , char* [])
{
  //print_captures("(([[:lower:]]+)|([[:upper:]]+))+", "aBBcccDDDDDeeeeeeee");
  //print_captures("(.*)bar|(.*)bah", "abcbar");
  //print_captures("(.*)bar|(.*)bah", "abcbah");
  //print_captures("^(?:(\\w+)|(?>\\W+))*$",
  std::string re = "^((\\| )|( \\|)|(  ))*(\\|-|`-)$";
  print_captures(re, "|-");
  print_captures(re, "| |-");
  print_captures(re, "| `-");
  print_captures(re, "|   |-");
  print_captures(re, "|   | |-");
  print_captures(re, "|   | `-");
  print_captures(re, "|   |   `-");
  print_captures(re, "|   |-");
  print_captures(re, "|   | |-");
  print_captures(re, "|   | `-");
  print_captures(re, "|   |   | `-");
  print_captures(re, "|   |   `-");
  print_captures(re, "|   |     |-");
  print_captures(re, "|   |     | `-");
  print_captures(re, "|   |     `-");
  print_captures(re, "|   |       `-");
  print_captures(re, "|   |         |-");
  print_captures(re, "|   |         | `-");
  print_captures(re, "|   |         `-");
  print_captures(re, "|   |           `-");
  print_captures(re, "|   `-");
  print_captures(re, "|     `-");
  print_captures(re, "|       `-");
  print_captures(re, "`-");
  print_captures(re, "  |-");
  print_captures(re, "  |-");
  print_captures(re, "  `-");
  return 0;
}
