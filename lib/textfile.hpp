#pragma once
#include <string>
#include <iostream>

using namespace std;

namespace Deluxe {
    class Textfile {
        public:
            string contents;
            uint length;
        public:
            Textfile(const string content) {
                this->contents = string(content);
                this->length = this->contents.length();
            }

            Textfile(istream &stream) {
                string content;
                while (std::getline(stream, content)) {
                    this->contents.append(content);
                    this->contents.append("\n"); // keep newline character
                }
                this->length = this->contents.length();
            }

            string getContents () {
                string result(this->contents);
                return result;
            }
    };
};