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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <regex>
#include <sstream>
#include "util.hpp"

#define PRECISAO 3

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::setw;
using std::ifstream;
using std::ofstream;
using std::stringstream;

//Variaveis globais
t_config config;

//Funções
bool valid_input(string file);
dados_vazao parse_input(string file);
void calcular(string file);
bool ler_config(string file);

void cabecalho(bool alinhar)
{
    cout << center(" ╔════════════════════════════════════════════════════════╗", alinhar) << endl;
    cout << center(" ║          Cálculo de Estatísticas Hidrológicas          ║", alinhar) << endl;
    cout << center(" ║                                                        ║", alinhar) << endl;
    cout << center(" ║       Desenvolvido por Bibiana Rodrigues Colossi       ║", alinhar) << endl;
    cout << center(" ║     Modificado por Carlo Lucca Rossi (Outubro,2016)    ║", alinhar) << endl;
    cout << center(" ║ Reescrito em C++ por Natanael Rodrigo Zimmer (02/2017) ║", alinhar) << endl;
    cout << center(" ╚════════════════════════════════════════════════════════╝", alinhar) << endl;
    cout << endl;
}

void ajuda(string basepath)
{
    cabecalho(true);
    cout << "   Este programa calcula uma série de estatísticas e outras variáveis a partir de uma série temporal de vazões." << endl;
    cout << "   O(s) arquivo(s) de entrada devem estar no formato: dia mês ano valor." << endl;
    cout << "   Arquivo de entrada = arquivo de saída do Manejo de Dados HidroWeb." << endl;
    cout << endl;
    cout << "   VARIÁVEIS CALCULADAS:" << endl;
    cout << endl;
    cout << " » Vazão média de longo período, calculada com a vazão diária de anos com nível de falha aceitável (Configuravel)" << endl;
    cout << " » Vazão média de 7 dias calculada para todos os anos com mais de 363 dias de dados. (Configuravel)" << endl;
    cout << " » Vazao máxima no ano." << endl;
    cout << " » Vazão mínima média de sete dias, com 10 anos de tempo de retorno (Configuravel)." << endl;
    cout << endl;
    cout << "Não foi possivel encontrar um arquivo de configuração 'config.txt' válido." << endl;
    cout << "Diretório base: " << basepath << endl;
    cout << endl;
    cout << "Exemplo de arquivo config.txt:" << endl;
    cout << "dias_minimos=363" << endl;
    cout << "nome_saida=QRef_" << endl;
    cout << "q7n=10" << endl;
    cout << endl;
    cout << "Pressione ENTER para terminar\t\t";
    ;
    std::cin.get();
    cout << endl;
}

int main(int argc, char** argv)
{
    //Variaveis
    vector<string> arquivos, entradas;
    //Configura o prompt do windows para exibir acentos corretamente.
    system("chcp 65001 > NUL");
    //Verifica se foi passado uma pasta como parametro da linha de comando     
    //Caso não tenha sido, utiliza a pasta onde está o executavel  
    if (argc > 1)
    {
        if (dirExists(argv[1]))
        {
            config.basepath = string(argv[1]);
        }
        else
        {
            config.basepath = exePath();
        }
    }
    else
    {
        config.basepath = exePath();
    }
    config.basepath.append("\\");
    config.basepath = std::regex_replace(config.basepath, std::regex("\\\\+"), "\\");
    string config_fname = config.basepath;
    config_fname.append("config.txt");
    if (ler_config(config_fname))
    {
        cabecalho(false);
    }
    else
    {
        ajuda(config.basepath);
        return 0;
    }
    //Lista os arquivos do tipo txt que estão na pasta   
    arquivos = list_files(config.basepath, "*.txt");
    //Valida se é um nome de arquivo válido como entrada.    
    for (int i = 0; i < arquivos.size(); i++)
    {
        if (valid_input(arquivos[i]))
        {
            entradas.push_back(arquivos[i]);
        }
    }
    //Para cada arquivo de entrada executa os calculos  
    int numitens = entradas.size();
#pragma omp parallel for ordered schedule(dynamic,1)
    for (int i = 0; i < numitens; i++)
    {
        calcular(entradas[i]);
#pragma omp ordered     
        cout << i + 1 << "/" << numitens << " " << entradas[i] << " Processado" << endl;
    }
    return 0;
}

bool valid_input(string file)
{
    //Verifica o tamanho do nome do arquivo com a extensão (8 + .txt)  
    if (file.size() != 12)
    {
        return false;
    }
    //Verifica se os 8 caracteres do nome do arquivo são números    
    for (char i = 0; i < 8; i++)
    {
        if ('0' < file[i] && file[i] > '9')
        {
            return false;
        }
    }
    return true;
}

dados_vazao parse_input(string file)
{
    ifstream inFile;
    string fin = config.basepath;
    string str_vazao, str_dia, str_mes, str_ano;
    t_vazao dados;
    dados_vazao retorno;
    int ano, last_ano;
    fin.append(file);
    inFile.open(fin.c_str());
    while (inFile.good())
    {
        inFile >> str_dia >> str_mes >> str_ano >> str_vazao;
        dados.dia = str_toint(str_dia);
        dados.mes = str_toint(str_mes);
        ano = str_toint(str_ano);
        dados.vazao = str_tofloat(str_vazao);
        if (!data_valida(dados, ano))
        {
#pragma omp ordered    
            {
                cout << "Erro ao ler arquivo " << file << endl;
                cout << "Ultimos dados válidos: " << retorno[last_ano].back().dia << "/" << retorno[last_ano].back().mes << "/" << last_ano << " " << retorno[last_ano].back().vazao << endl;
                cout << "Dados inválidos: " << dados.dia << "/" << dados.mes << "/" << ano << " " << dados.vazao << endl;
                exit(-1);
            }
        }
        last_ano = ano;
        if (dados.vazao < 0)
        {
            dados.vazao = 0;
            dados.invalido = true;
        }
        else
        {
            dados.invalido = false;
        }
        retorno[ano].push_back(dados);
    }
    return retorno;
}

void calcular(string file)
{
    //Variaveis 
    ofstream saida;
    vector<t_resultado> resultados;
    int anos = 0;
    float longo_periodo = 0;
    string fout = config.basepath;
    vector<float> Q;
    vector<float> TR;
    float TR1, TR2, Q1, Q2, QTR;
    map<float, float> Q7N;

    //Lê dados de entrada do arquivo   
    dados_vazao dados = parse_input(file);

    //Loop para processar os dados ano a ano 
    for (dados_vazao::iterator dados_iterator = dados.begin(); dados_iterator != dados.end(); dados_iterator++)
    {
        t_resultado resultado_ano;
        int ano = dados_iterator->first;
        vector<t_vazao> dados_ano = dados_iterator->second;
        resultado_ano.ano = ano;
        resultado_ano.min = FLOAT_MAX;
        resultado_ano.max = 0;
        resultado_ano.media = 0;
        resultado_ano.media_min = FLOAT_MAX;
        resultado_ano.dias_com_dados = 0;
        //Loop para passar por todos os dias do ano, menos os ultimos 6  
        for (int i = 0; i < dados_ano.size() - 6; i++)
        {
            //Se qualquer uma das vazões da sequencia de 7 dias for negativa (não existir dados), não deve ser calculado             
            if (!dados_ano[i].invalido && !dados_ano[i + 1].invalido && !dados_ano[i + 2].invalido && !dados_ano[i + 3].invalido &&
                !dados_ano[i + 4].invalido && !dados_ano[i + 5].invalido && !dados_ano[i + 6].invalido)
            {
                float media = (dados_ano[i].vazao + dados_ano[i + 1].vazao + dados_ano[i + 2].vazao + dados_ano[i + 3].vazao +
                               dados_ano[i + 4].vazao + dados_ano[i + 5].vazao + dados_ano[i + 6].vazao) / 7.0;
                if (resultado_ano.media_min > media)
                {
                    resultado_ano.media_min = media;
                }
            }
        }
        if (resultado_ano.media_min == FLOAT_MAX)
        {
            resultado_ano.media_min = 0;
        }
        for (auto dia:dados_ano)
        {
            if (!dia.invalido)
            {
                resultado_ano.media += dia.vazao;
                resultado_ano.dias_com_dados++;
                if (resultado_ano.min > dia.vazao)
                {
                    resultado_ano.min = dia.vazao;
                }
                if (resultado_ano.max < dia.vazao)
                {
                    resultado_ano.max = dia.vazao;
                }
            }
        }
        if (resultado_ano.min == FLOAT_MAX)
        {
            resultado_ano.min = 0;
        }
        if (resultado_ano.dias_com_dados > 0)
        {
            resultado_ano.media = resultado_ano.media / (float)resultado_ano.dias_com_dados;
        }
        if (resultado_ano.dias_com_dados > config.dias_minimos)
        {
            Q.push_back(resultado_ano.media_min);
            longo_periodo += resultado_ano.media;
            anos++;
        }
        resultados.push_back(resultado_ano);
    }

    //Calculo de Q7,10 (QTR) 
    if (anos >= 2)
    {
        int i;

        //Ordena valores de Q;    
        std::sort(Q.begin(), Q.end());

        for (i = 1; i <= anos; i++)
        {
            TR.push_back((float)1.0 / ((float)i / (anos + 1)));
        }

        for (i = 1; i <= anos; i++)
        {
            if (TR[i - 1] < config.q7n)
            {
                break;
            }
        }
        if (i == 1)
        {
            i = 2;
        }

        TR1 = TR[i - 1];
        TR2 = TR[i - 2];
        Q1 = Q[i - 1];
        Q2 = Q[i - 2];
        QTR = ((Q2 - Q1) / (TR2 - TR1)) * (config.q7n - TR1) + Q1;

        Q7N[10.0] = QTR;
        for (int i = 0; i < anos; i++)
        {
            Q7N[TR[i]] = Q[i];
        }
    }
    else
    {
        QTR = 0;
    }

    fout.append(config.nome_saida).append(file);
    saida.open(fout);

    //Formatação para 5 numeros depois da virgula 
    saida << std::fixed << std::setprecision(PRECISAO);
    saida << file << endl << endl;
    if (anos > 0)
    {
        saida << "Vazão média de longo período: " << (longo_periodo / anos) << " m³/s" << endl;
    }
    else
    {
        saida << "Vazão média de longo período: IMPOSSIVEL CALCULAR" << endl;
    }
    saida << anos << " anos de dados" << endl;

    if (Q7N.find(10.0)==Q7N.end() && TR2 < 10)
    {
        saida << "Q7," << config.q7n << ": " << "SÉRIE MUITO CURTA!!!" << endl << endl;
    }
    else
    {
        saida << "Q7," << config.q7n << ": " << QTR << endl << endl;
    }

    //Cabeçalho  
    saida << setw(4) << "ANO" << setw(15) << "Q7_MIN" << setw(15) << "QMAX_DIA" << setw(15) << "DIAS" << endl;
    for (auto result:resultados)
    {
        //Verifica se ano FALHOU   
        if (result.dias_com_dados > config.dias_minimos)
        {
            saida << setw(4) << result.ano << setw(15) << result.media_min << setw(15) << result.max << setw(15) << result.dias_com_dados << endl;
        }
        else
        {
            saida << setw(4) << result.ano << setw(15) << "FALHOU" << setw(15) << result.max << setw(15) << result.dias_com_dados << endl;
        }
    }

    if (anos > 0)
    {
        saida << endl << "Dados para gerar grafico TR x Q7." << endl;
        saida << setw(8) << "TR" << setw(12) << "Q7" << endl;
        for (const auto pair:Q7N)
        {
            saida << setw(8) << pair.first << setw(12) << pair.second << endl;
        }
        saida.flush();
        saida.close();
    }
}

bool ler_config(string file)
{
    ifstream fconfig;
    string line;
    vector<string> param;
    int result = 0;
    fconfig.open(file);
    while (fconfig.good())
    {
        fconfig >> line;
        param = explode(line, '=');
        if (param[0].compare("dias_minimos") == 0)
        {
            config.dias_minimos = atoi(param[1].c_str());
            result++;
        }
        if (param[0].compare("q7n") == 0)
        {
            config.q7n = atoi(param[1].c_str());
            result++;
        }
        if (param[0].compare("nome_saida") == 0)
        {
            config.nome_saida = param[1];
            result++;
        }
    }
    if (result != 3)
    {
        return false;
    }
    return true;
}