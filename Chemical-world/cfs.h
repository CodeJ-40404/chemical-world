/*
 * @file cfs.h
 * @author weigao (https://github.com/weigao-china)
 * @version 1.0
 * @date 2026-04-27
 *
 * @copyright 2026 weigao
 * @license MIT

MIT License

Copyright (c) 2026 weigao

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#ifndef CFS_H
#define CFS_H

#include <fstream>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

//"CON" is not available in macOS and Linux,so use /dev/tty instead -- CodeJ-40404

#ifdef _WIN32
    #define CONSOLE_IN "CON"
    #define CONSOLE_OUT "CON"
#else
    #define CONSOLE_IN "/dev/tty"
    #define CONSOLE_OUT "/dev/tty"
#endif


namespace cfs {
    static std::ifstream cin;
    static std::ofstream cout;

    void cfs(const char* input, const char* output) {
        std::freopen(input, "r", stdin);
        std::freopen(output, "w", stdout);
    }

    void cppfs(const char* input, const char* output) {
        cin.open(input);
        cout.open(output);
    }

    void closecfs() {
        std::freopen(CONSOLE_IN, "r", stdin);
        std::freopen(CONSOLE_OUT, "w", stdout);
    }

    void closecppfs(){
        if (cin.is_open()) cin.close();
        if (cout.is_open()) cout.close();
    }
    
    // a simple calculation funcction,LoL
    double calc(const char* filename) { 
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "can't find the file: " << filename << std::endl;
            return 0;
        }

        double a, b;
        char op;
        file >> a >> op >> b;
        file.close();

        switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                std::cerr << "0 can't devide a num" << std::endl;
                return 0;
            }
            return a / b;
        default:
            std::cerr << "not supported calc " << op << std::endl;
            return 0;
        }
    }

}

#endif // CFS_H
