namespace clang_ast2dot {
namespace parser {

class MockAst2DotParser : public Ast2DotParser {
 public:
  MOCK_METHOD1(read_sibling_child_string,
      std::string&(std::istream));
  MOCK_METHOD4(quote_special_quotes,
      std::string&(std::string&, std::string const&, std::string const&, std::string*&));
  MOCK_METHOD1(null_to_name,
      std::string(const));
  MOCK_METHOD1(null_to_label,
      std::string(std::string const&));
  MOCK_METHOD2(read_vertex_props,
      std::string*(std::istream *, std::ostream *));
  MOCK_METHOD0(inbuf,
      std::string&(void));
  MOCK_METHOD0(scstr,
      std::string&(void));
  MOCK_METHOD0(is_null,
      bool(void));
  MOCK_METHOD0(name,
      std::string&(void));
  MOCK_METHOD0(label,
      std::string&(void));
  MOCK_METHOD0(address,
      std::string&(void));
  MOCK_METHOD0(props,
      std::vector<std::string>&(void));
};

}  // namespace parser
}  // namespace clang_ast2dot
