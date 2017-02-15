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

#include <sys/stat.h>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>
#include "util.hpp"

using std::string;
using std::vector;

std::time_t ztime = std::time(nullptr);
std::tm* zlocaltime = std::localtime(&ztime);
int zdia = zlocaltime->tm_mday;
int zmes = zlocaltime->tm_mon + 1;
int zano = zlocaltime->tm_year + 1900;

int dirExists(const char *path)
{
    struct stat info;
    if (stat(path, &info) != 0)
        return 0;
    else if (info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

string exePath()
{
    char ownPth[MAX_PATH];
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleFileName(hModule, ownPth, (sizeof (ownPth)));
    string dir = ownPth;
    const size_t last_slash_idx = dir.rfind('\\');
    if (string::npos != last_slash_idx)
    {
        dir = dir.substr(0, last_slash_idx);
    }
    return dir;
}

vector<string> list_files(string folder, string filter)
{
    vector<string> names;
    string search_path = folder + filter;
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                names.push_back(fd.cFileName);
            }
        }
        while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}

int str_toint(string in)
{
    in.erase(std::remove(in.begin(), in.end(), ','), in.end());
    return (int)atoi(in.c_str());
}

float str_tofloat(string in)
{
    in.erase(std::remove(in.begin(), in.end(), ','), in.end());
    return (float)atof(in.c_str());
}

bool data_valida(t_vazao dado, int ano)
{
    if ((ano > zano) || (ano == zano && dado.mes > zmes) || (ano == zano && dado.mes == zmes && dado.dia > zdia) || (dado.dia < 1))
    {
        return false;
    }
    switch (dado.mes)
    {
        case 1: if (dado.dia > 31) return false;
            break;
        case 2:
            if ((ano % 4 == 0) && !(ano % 100 == 0) || (ano % 400 == 0))
            {
                if (dado.dia > 29) return false;
            }
            else
            {
                if (dado.dia > 28) return false;
            }
            break;
        case 3: if (dado.dia > 31) return false;
            break;
        case 4: if (dado.dia > 30) return false;
            break;
        case 5: if (dado.dia > 31) return false;
            break;
        case 6: if (dado.dia > 30) return false;
            break;
        case 7: if (dado.dia > 31) return false;
            break;
        case 8: if (dado.dia > 31) return false;
            break;
        case 9: if (dado.dia > 30) return false;
            break;
        case 10: if (dado.dia > 31) return false;
            break;
        case 11: if (dado.dia > 30) return false;
            break;
        case 12: if (dado.dia > 31) return false;
            break;
        default: return false;
            break;
    }
    return true;
}

/*
std::ostream& operator<<(std::ostream& os, const t_resultado& r) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(5) << r.media_min;
    return os << "Ano: " << r.ano
              << "\tDias: " << r.dias_com_dados << std::fixed << std::setprecision(5)
              << " \tMedia Min: " << ((r.dias_com_dados > 363) ? ss.str() : "FALHOU")
              << "\tMedia Ano: " << r.media
              << "\tMin: " << r.min
              << "\tMax: " << r.max << std::endl;
}
 */

string center(string text, bool alinhar)
{
    if (!alinhar)
    {
        return text;
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int n = (columns / 2 - utf8_strlen(text) / 2);
    string saida = string(n, ' ');
    saida.append(text);
    return saida;
}

int utf8_strlen(const string& str)
{
    int c, i, ix, q;
    for (q = 0, i = 0, ix = str.length(); i < ix; i++, q++)
    {
        c = (unsigned char)str[i];
        if (c >= 0 && c <= 127) i += 0;
        else if ((c & 0xE0) == 0xC0) i += 1;
        else if ((c & 0xF0) == 0xE0) i += 2;
        else if ((c & 0xF8) == 0xF0) i += 3;
            //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
            //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return 0; //invalid utf8
    }
    return q;
}

vector<string> explode(string const & s, char delim)
{
    vector<string> result;
    std::istringstream iss(s);
    for (string token; std::getline(iss, token, delim);)
    {
        result.push_back(std::move(token));
    }
    return result;
}