#pragma once
#include <memory>
#include <string>
#include <map>
#include <stack>

#include "parser.hpp"

using namespace std;
using namespace Deluxe;


namespace Deluxe {
    
    typedef std::function<void(vector<Expression>)> RuntimeFunction;

    class RuntimeException : public exception {
        public:
            string message;
            uint line;
            uint col;

            RuntimeException (string message) {
                this->message = string("Runtime Exception: " + message);
            }

            RuntimeException (string message, uint line) {
                this->message = string("Runtime Exception: " + message + " (at line: " + to_string(line) + ")");
            }

            RuntimeException (string message, uint line, uint col) {
                this->message = string("Runtime Exception: " + message + " (at line: " + to_string(line) + "; col " + to_string(col) + ")");
            }

            const char * what () const throw () {
                return this->message.c_str();
            }
    };
    
    class Interpreter {
        public:
            unique_ptr<Deluxe::ParseResult> ast;
            std::map<string, RuntimeFunction> runtime;
            std::map<string, Expression> environment;
            stack<Expression> stack;

            Interpreter(Deluxe::ParseResult ast) {
                this->ast = make_unique<Deluxe::ParseResult>(ast);
                this->initialize();
            }

            Expression getNone () {
                return Expression {
                        .tag = ExpressionTag::SYMBOL,
                        .symbolValue = "none"
                    };
            }

            void initialize () {
                RuntimeFunction printf = [](vector<Expression> params){
                    for (auto it = params.begin(); it != params.end(); ++it) {
                        switch (it->tag) {
                            case ExpressionTag::NUMBER: { cout << to_string(it->numberValue); break; }
                            case ExpressionTag::STRING: { cout << it->stringValue; break; }
                            case ExpressionTag::CALL:   { cout << "@CALL " << it->callName; break; }
                            case ExpressionTag::SYMBOL: { cout << "#" << it->symbolValue; break; }
                        }
                    }
                    cout << endl;
                };

                RuntimeFunction fnReturn = [&](vector<Expression> params){
                    for (auto it = params.begin(); it != params.end(); ++it) {
                        this->stack.push(*it);
                    }
                };

                RuntimeFunction fnLet = [&](vector<Expression> params) {
                    if (params.size() == 0) return;
                    Expression val = this->getNone();
                    auto sym = params[0];
                    if (params.size() >= 1) val = params[1];
                    if (sym.tag != ExpressionTag::SYMBOL) throw RuntimeException("Symbol expected");
                    this->environment.insert(pair<string, Expression>(sym.symbolValue, val));
                };

                this->runtime.insert(pair<string, RuntimeFunction>("printf", printf));
                this->runtime.insert(pair<string, RuntimeFunction>("return", fnReturn));
                this->runtime.insert(pair<string, RuntimeFunction>("let", fnLet));
            }

            Expression executeExpression(Expression exp) {
                switch (exp.tag) {
                    case ExpressionTag::CALL: {
                        string functionName(exp.callName);
                        auto function = this->runtime.find(functionName);
                        if (function == this->runtime.end()) {
                            throw RuntimeException("Undefined function " + functionName);
                        }
                        vector<Expression> parameters;
                        for (auto param = exp.callValue.begin(); param != exp.callValue.end(); ++param) {
                            parameters.push_back(this->executeExpression(*param));
                        }
                        function->second(parameters);
                        break;
                    }
                    case ExpressionTag::SYMBOL: {
                        auto val = this->environment.find(exp.symbolValue);
                        if (val == this->environment.end()) {
                            return exp;
                        }
                        return val->second;
                    }
                    default:
                        return exp;
                        break;
                }
                if (this->stack.empty()) {
                    return this->getNone();
                }
                auto result = this->stack.top();
                this->stack.pop();
                return result;
            }
    };
}