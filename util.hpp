/*
 * MIT License
 * 
 * Copyright (C) 2017 Natanael Zimmer <nrzimmer at gmail.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef DEBUG
    #define LOG(var ) (std::cout) << "LOG: " << __FILE__ << "(" << __LINE__ << ") " << #var << " = [" << (var) << "]" << std::endl
#else
    #define LOG(var)
#endif

#define FLOAT_MAX std::numeric_limits<float>::max()

#ifndef UTIL_HPP
    #define UTIL_HPP

    #include <string>
    #include <vector>
    #include <map>
    #include <iostream>

using std::string;
using std::vector;
using std::map;

typedef struct t_vazao
{
    int dia;
    int mes;
    float vazao;
    bool invalido;

    bool operator<(const t_vazao& ref) const
    {
        if (vazao < 0) return false;
        if (ref.vazao < 0) return true;
        return vazao < ref.vazao;
    }
} t_vazao;

typedef struct t_config
{
    int dias_minimos;
    int q7n;
    string nome_saida;
    string basepath;
} t_config;

//Estrutura de dados de saida Q7/QMAX

typedef struct t_resultado
{
    int ano;
    int dias_com_dados;
    float min;
    float max;
    float media_min;
    float media;
} t_resultado;

typedef map<int, vector<t_vazao>> dados_vazao;

int dirExists(const char *path);
int str_toint(string in);
int utf8_strlen(const string& str);

string exePath();
string center(string text, bool alinhar);

float str_tofloat(string in);

bool data_valida(t_vazao dado, int ano);

vector<string> list_files(string folder, string filter);
vector<string> explode(string const & s, char delim);

std::ostream& operator<<(std::ostream& os, const t_resultado& r);

#endif /* UTIL_HPP */

