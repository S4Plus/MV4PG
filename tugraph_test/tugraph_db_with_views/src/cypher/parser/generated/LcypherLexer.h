
// Generated from /home2/xcj/tugraph-2024-04-11/tugraph-db-master/src/cypher/grammar/../../..//src/cypher/grammar/Lcypher.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"


namespace parser {


class  LcypherLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, EXPLAIN = 46, PROFILE = 47, OPTIMIZE = 48, MAINTENANCE = 49, 
    VIEW = 50, CONSTRUCT = 51, UNION = 52, ALL = 53, OPTIONAL_ = 54, MATCH = 55, 
    UNWIND = 56, AS = 57, MERGE = 58, ON = 59, CREATE = 60, SET = 61, DETACH = 62, 
    DELETE_ = 63, REMOVE = 64, CALL = 65, YIELD = 66, WITH = 67, DISTINCT = 68, 
    RETURN = 69, ORDER = 70, BY = 71, L_SKIP = 72, LIMIT = 73, ASCENDING = 74, 
    ASC = 75, DESCENDING = 76, DESC = 77, USING = 78, JOIN = 79, START = 80, 
    WHERE = 81, NO_DUPLICATE_EDGE = 82, OR = 83, XOR = 84, AND = 85, NOT = 86, 
    IN = 87, STARTS = 88, ENDS = 89, CONTAINS = 90, REGEXP = 91, IS = 92, 
    NULL_ = 93, COUNT = 94, ANY = 95, NONE = 96, SINGLE = 97, TRUE_ = 98, 
    FALSE_ = 99, EXISTS = 100, CASE = 101, ELSE = 102, END = 103, WHEN = 104, 
    THEN = 105, StringLiteral = 106, EscapedChar = 107, HexInteger = 108, 
    DecimalInteger = 109, OctalInteger = 110, HexLetter = 111, HexDigit = 112, 
    Digit = 113, NonZeroDigit = 114, NonZeroOctDigit = 115, OctDigit = 116, 
    ZeroDigit = 117, ExponentDecimalReal = 118, RegularDecimalReal = 119, 
    FILTER = 120, EXTRACT = 121, UnescapedSymbolicName = 122, CONSTRAINT = 123, 
    DO = 124, FOR = 125, REQUIRE = 126, UNIQUE = 127, MANDATORY = 128, SCALAR = 129, 
    OF = 130, ADD = 131, DROP = 132, IdentifierStart = 133, IdentifierPart = 134, 
    EscapedSymbolicName = 135, SP = 136, WHITESPACE = 137, Comment = 138
  };

  explicit LcypherLexer(antlr4::CharStream *input);

  ~LcypherLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

}  // namespace parser
