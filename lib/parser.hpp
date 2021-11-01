#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <ctype.h>

using namespace std;

namespace Deluxe {
    enum TToken {
        Symbol,
        Number,
        String,
        OpenBracket,
        CloseBracket
    };

    struct SToken {
        TToken tokenType;
        string content;
        uint line;
    };

    class ParserException : public exception {
        public:
            string message;
            uint line;
            uint col;

            ParserException (string message) {
                this->message = string("Parser Exception: " + message);
            }

            ParserException (string message, uint line) {
                this->message = string("Parser Exception: " + message + " (at line: " + to_string(line) + ")");
            }

            ParserException (string message, uint line, uint col) {
                this->message = string("Parser Exception: " + message + " (at line: " + to_string(line) + "; col " + to_string(col) + ")");
            }

            const char * what () const throw () {
                return this->message.c_str();
            }
    };

    enum class ExpressionTag { SYMBOL, STRING, NUMBER, CALL };

    struct Expression {
        ExpressionTag tag;
        string stringValue;
        string symbolValue;
        double numberValue;
        vector<Expression> callValue;
        string callName;
    };

    struct ParseResult {
        vector<Expression> expressions;
        int index;
    };

    class Parser {
        public:
            Parser () {}
            static bool isBracket (char ch) {
                switch (ch) {
                    case '(': return true;
                    case ')': return true;
                    case '[': return true;
                    case ']': return true;
                    case '{': return true;
                    case '}': return true;
                    default:
                        return false;
                }
            }

            static string getTokenName (TToken tok) {
                switch (tok) {
                    case Symbol: return string("Symbol");
                    case Number: return string("Number");
                    case String: return string("String");
                    case OpenBracket: return string("OpenBracket");
                    case CloseBracket: return string("CloseBracket");
                    default:
                        return string("Token");
                }
            }

            static vector<SToken> getTokens (string content) {
                uint length = content.length();
                uint line = 1;
                uint col = 1;
                vector<SToken> tokens;
                string current;

                TToken currentToken = Symbol;

                for (uint i = 0; i < length; i++) {
                    char ch(content[i]);
                    col++;
                    if (ch == '\n') {
                        col = 0;
                        line++;
                    }
                    if (ch == ';') {
                        while (ch != '\n') {
                            i++;
                            ch = content[i];
                        }
                        line++;
                    }
                    if (ch == '"') {
                        if (currentToken == String) {
                            tokens.push_back({ currentToken, string(current), line });
                            currentToken = Symbol;
                            current.clear();
                            continue;
                        } else {
                            currentToken = String;
                            continue;
                        }
                    }
                    if (currentToken == String) {
                        current.push_back(ch);
                        continue;
                    }
                    if (isspace(ch)) {
                        if (!current.empty()) {
                            tokens.push_back({ currentToken, string(current), line });
                            currentToken = Symbol;
                            current.clear();
                        }
                        continue;
                    }
                    if (Parser::isBracket(ch)) {
                        if (!current.empty()) tokens.push_back({ currentToken, string(current), line });
                        current.clear();
                        current.push_back(ch);
                        if (ch == '(' ||ch == '[' ||ch == '{') currentToken = OpenBracket;
                        if (ch == ')' ||ch == ']' ||ch == '}') currentToken = CloseBracket;
                        tokens.push_back({ currentToken, string(current), line });
                        current.clear();
                        currentToken = Symbol;
                        continue;
                    }
                    if (isdigit(ch)) {
                        if (current.empty()) {
                            currentToken = Number;
                        }
                    }
                    if (currentToken == Number && !isdigit(ch) && ch != '.') {
                        // std::cout << "Error: digit expected" << endl;
                        throw ParserException("Invalid number", line, col);
                    }
                    current.push_back(ch);
                }
                if (!current.empty()) tokens.push_back({ currentToken, string(current), line }); // get last token
                return tokens;
            }

            static SToken expectSymbol(SToken token) {
                if (token.tokenType != Symbol) throw ParserException("expected Symbol (got " + Parser::getTokenName(token.tokenType) + ")", token.line);
                return token;
            }

            static ParseResult parse(vector<SToken> tokens, string currentBracket = "") {
                SToken current;
                ParseResult result;
                for(std::vector<SToken>::iterator it = std::begin(tokens); it != std::end(tokens); ++it) {
                    current = *it;
                    result.index = (int)std::distance(tokens.begin(), it);
                    if (current.tokenType == OpenBracket) {
                        string openingBracket = string(current.content);
                        it++;
                        auto dist = std::distance(tokens.begin(), it);
                        current = Parser::expectSymbol(*it);
                        auto tail = vector<SToken>(tokens.begin() + dist, tokens.end());
                        ParseResult body = Parser::parse( tail, openingBracket );
                        result.expressions.push_back(Expression {
                            ExpressionTag::CALL,
                            .callValue = vector<Expression>(body.expressions.begin() + 1, body.expressions.end()),
                            .callName = body.expressions.front().symbolValue
                        });
                        it += body.index;
                        continue; // TODO: Skip tail
                    }
                    if (current.tokenType == Symbol) {
                        result.expressions.push_back(Expression {
                            ExpressionTag::SYMBOL,
                            .symbolValue = current.content
                        });
                    }
                    if (current.tokenType == String) {
                        result.expressions.push_back(Expression {
                            ExpressionTag::STRING,
                            .stringValue = current.content
                        });
                    }
                    if (current.tokenType == Number) {
                        result.expressions.push_back(Expression {
                            ExpressionTag::NUMBER,
                            .numberValue = std::stod(current.content)
                        });
                    }
                    if (current.tokenType == CloseBracket) {
                        if (!currentBracket.empty()) {
                            if (currentBracket == "(" && current.content != ")") throw ParserException("Expected closing bracket ')'", current.line);
                            if (currentBracket == "[" && current.content != "]") throw ParserException("Expected closing bracket ']'", current.line);
                            if (currentBracket == "{" && current.content != "}") throw ParserException("Expected closing bracket '}'", current.line);
                        }
                        break;
                    }
                    
                }
                return result;
            }
    };
}
