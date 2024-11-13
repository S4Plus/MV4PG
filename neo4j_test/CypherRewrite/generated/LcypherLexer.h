
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
    T__44 = 45, EXPLAIN = 46, PROFILE = 47, OPTIMIZE = 48, VIEW = 49, UNION = 50, 
    ALL = 51, OPTIONAL_ = 52, MATCH = 53, UNWIND = 54, AS = 55, MERGE = 56, 
    ON = 57, CREATE = 58, SET = 59, DETACH = 60, DELETE_ = 61, REMOVE = 62, 
    CALL = 63, YIELD = 64, WITH = 65, DISTINCT = 66, RETURN = 67, ORDER = 68, 
    BY = 69, L_SKIP = 70, LIMIT = 71, ASCENDING = 72, ASC = 73, DESCENDING = 74, 
    DESC = 75, USING = 76, JOIN = 77, START = 78, WHERE = 79, NO_DUPLICATE_EDGE = 80, 
    OR = 81, XOR = 82, AND = 83, NOT = 84, IN = 85, STARTS = 86, ENDS = 87, 
    CONTAINS = 88, REGEXP = 89, IS = 90, NULL_ = 91, COUNT = 92, ANY = 93, 
    NONE = 94, SINGLE = 95, TRUE_ = 96, FALSE_ = 97, EXISTS = 98, CASE = 99, 
    ELSE = 100, END = 101, WHEN = 102, THEN = 103, StringLiteral = 104, 
    EscapedChar = 105, HexInteger = 106, DecimalInteger = 107, OctalInteger = 108, 
    HexLetter = 109, HexDigit = 110, Digit = 111, NonZeroDigit = 112, NonZeroOctDigit = 113, 
    OctDigit = 114, ZeroDigit = 115, ExponentDecimalReal = 116, RegularDecimalReal = 117, 
    FILTER = 118, EXTRACT = 119, UnescapedSymbolicName = 120, CONSTRAINT = 121, 
    DO = 122, FOR = 123, REQUIRE = 124, UNIQUE = 125, MANDATORY = 126, SCALAR = 127, 
    OF = 128, ADD = 129, DROP = 130, IdentifierStart = 131, IdentifierPart = 132, 
    EscapedSymbolicName = 133, SP = 134, WHITESPACE = 135, Comment = 136
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
