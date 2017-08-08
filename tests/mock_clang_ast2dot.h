namespace clang_ast2dot {

class MockAst2DotMain : public Ast2DotMain {
 public:
  MOCK_METHOD0(vm,
      po::variables_map&(void));
  MOCK_METHOD1(do_main,
      int(int));
  MOCK_METHOD4(create_dot,
      int(std::istream *, std::ostream *, std::string const&, int));
};

}  // namespace clang_ast2dot
