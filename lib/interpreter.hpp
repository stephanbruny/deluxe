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
    typedef map<string, Expression> EnvironmentScope;
    typedef vector<Expression> ExpressionList;


    class FunctionObject {
        public:
            vector<string> arguments;
            EnvironmentScope scope;
            RuntimeFunction body;

            FunctionObject (vector<string> args, EnvironmentScope scope, RuntimeFunction body) {
                this->arguments = vector<string>(args);
                this->scope = EnvironmentScope(scope);
                this->body = RuntimeFunction(body);
            }
    };
    
    union InterpreterExpression {
        Expression expression;
        FunctionObject function;
    };

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
                RuntimeFunction printf = [&](vector<Expression> params) {
                    auto args = this->executeAll(params);
                    for (auto it = args.begin(); it != args.end(); ++it) {
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
                    auto args = this->executeAll(params);
                    for (auto it = args.begin(); it != args.end(); ++it) {
                        this->stack.push(*it);
                    }
                };

                RuntimeFunction fnLet = [&](vector<Expression> params) {
                    if (params.size() == 0) return;
                    Expression val = this->getNone();
                    auto sym = params[0];
                    if (params.size() >= 1) val = this->executeExpression(params[1]);
                    if (sym.tag != ExpressionTag::SYMBOL) throw RuntimeException("Symbol expected");
                    this->environment.insert(pair<string, Expression>(sym.symbolValue, val));
                };

                RuntimeFunction callFn =[&](vector<Expression> params) {
                    Expression fnName = params[0];
                    // auto arguments = 
                };

                RuntimeFunction defineFunction = [&](vector<Expression> params) {
                    vector<string> argumentNames;
                    uint paramCount = 0;
                    for (auto param = params.begin(); param != params.end(); ++param) {
                        if (param->tag != ExpressionTag::SYMBOL) break;
                        paramCount++;
                        argumentNames.push_back(param->symbolValue);
                    }
                    auto callBody = ExpressionList(params.begin() + paramCount, params.end());
                    auto body = this->getCallableBody(argumentNames, this->environment, callBody);
                    FunctionObject fun(argumentNames, this->environment, body);
                    this->stack.push(Expression {
                        .tag = ExpressionTag::CALL,
                        .callValue = callBody
                    });
                    // TODO: CALL cannFn with FunctionObject
                };

                this->runtime.insert(pair<string, RuntimeFunction>("printf", printf));
                this->runtime.insert(pair<string, RuntimeFunction>("return", fnReturn));
                this->runtime.insert(pair<string, RuntimeFunction>("let", fnLet));
                this->runtime.insert(pair<string, RuntimeFunction>("fn", defineFunction));
            }

            void letBinding(string symbolName, Expression value) {
                this->environment.insert(pair<string, Expression>(symbolName, value));
            }

            RuntimeFunction getCallableBody (vector<string> arguments, EnvironmentScope scope, ExpressionList body) {
                return [&](vector<Expression> params) {
                    uint i = 0;
                    cout << "CALL BODY CALL" << endl;
                    for (auto arg = arguments.begin(); arg != arguments.end(); ++arg) {
                        auto param = params[i++];
                        this->letBinding(*arg, param);
                    }
                    auto results = this->executeAll(body, scope);
                    for (auto it = results.begin(); it != results.end(); ++it) {
                        this->stack.push(*it);
                    }
                };
            }

            vector<Expression> executeAll(vector<Expression> expressions) {
                vector<Expression> results;
                for (auto exp = expressions.begin(); exp != expressions.end(); ++exp) {
                    results.push_back(this->executeExpression(*exp));
                }
                return results;
            }

            vector<Expression> executeAll(vector<Expression> expressions, EnvironmentScope scope) {
                vector<Expression> results;
                for (auto exp = expressions.begin(); exp != expressions.end(); ++exp) {
                    results.push_back(this->executeExpression(*exp, scope));
                }
                return results;
            }

            Expression getSymbolValue(string symbol, std::map<string, Expression> env) {
                auto val = env.find(symbol);
                if (val == env.end()) {
                    return this->getNone();
                }
                return val->second;
            }

            Expression executeExpression(Expression exp, std::map<string, Expression> env) {
                switch (exp.tag) {
                    case ExpressionTag::CALL: {
                        string functionName(exp.callName);
                        if (!functionName.empty()) {
                            auto function = this->runtime.find(functionName);
                            if (function == this->runtime.end()) {
                                // Execute scope function
                                auto scopeFn = this->environment.find(functionName);
                                if (scopeFn != this->environment.end()) {
                                    cout << "FN " << functionName << endl;
                                    executeExpression(scopeFn->second, env);
                                    cout << this->stack.size() << endl;
                                    // executeExpression(this->stack.top(), env);
                                    break;
                                } else {
                                    throw RuntimeException("Undefined function " + functionName);
                                }
                            }
                            // vector<Expression> parameters;
                            // for (auto param = exp.callValue.begin(); param != exp.callValue.end(); ++param) {
                            //     parameters.push_back(this->executeExpression(*param));
                            // }
                            function->second(exp.callValue);
                        }
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

            Expression executeExpression(Expression exp) {
                return this->executeExpression(exp, this->environment);
            }
    };
}