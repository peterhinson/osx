set(LLVM_NO_RTTI 1)

# TODO: Add -maltivec when ARCH is PowerPC.

add_clang_library(clangLex
  HeaderMap.cpp
  HeaderSearch.cpp
  Lexer.cpp
  LiteralSupport.cpp
  MacroArgs.cpp
  MacroInfo.cpp
  PPCaching.cpp
  PPDirectives.cpp
  PPExpressions.cpp
  PPLexerChange.cpp
  PPMacroExpansion.cpp
  Pragma.cpp
  Preprocessor.cpp
  PreprocessorLexer.cpp
  PTHLexer.cpp
  ScratchBuffer.cpp
  TokenLexer.cpp
  TokenConcatenation.cpp
  )

add_dependencies(clangLex ClangDiagnosticLex)
