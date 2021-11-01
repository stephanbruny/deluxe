#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <ctype.h>

#include "lib/textfile.hpp"
#include "lib/parser.hpp"
#include "lib/interpreter.hpp"

using namespace std;

void show (vector<Deluxe::Expression> ast, string prefix) {
    for(std::vector<Deluxe::Expression>::iterator it = std::begin(ast); it != std::end(ast); ++it) {
        if (it->tag == Deluxe::ExpressionTag::CALL) {
            cout << prefix << "CALL " << it->callName << " {" << endl;
            show(it->callValue, prefix + "  ");
            cout << prefix << "}" << endl;
            continue;
        }
        if (it->tag == Deluxe::ExpressionTag::SYMBOL) {
            cout << prefix << "SYM " << it->symbolValue << endl;
        }
        if (it->tag == Deluxe::ExpressionTag::STRING) {
            cout << prefix << "STR \"" << it->stringValue << "\"" << endl;
        }
        if (it->tag == Deluxe::ExpressionTag::NUMBER) {
            cout << prefix << "NUM " << it->numberValue << endl;

        }
    }
}

int main() {
    unique_ptr<Deluxe::Textfile> file = std::make_unique<Deluxe::Textfile>(std::cin >> std::noskipws);
    // cout << "What: " << file->getContents() << endl;
    string content(file->getContents());

    try {
        auto tokens = Deluxe::Parser::getTokens(content);

        // cout << "Tokens: " << tokens.size() << endl;

        // for (int i = 0; i < tokens.size(); i++) {
        //     cout << "Token: " << Deluxe::Parser::getTokenName(tokens[i].tokenType) << " -> " << tokens[i].content << endl;
        // }

        uint len = 0;
        auto ast = Deluxe::Parser::parse(tokens);

        // show(ast.expressions, "");
        auto interpreter = Deluxe::Interpreter(ast);

        for(std::vector<Deluxe::Expression>::iterator it = std::begin(ast.expressions); it != std::end(ast.expressions); ++it) {
            interpreter.executeExpression(*it);
        }

    } catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }


    return 0;
}