# QReferencia

Este programa calcula uma série de estatísticas e outras variáveis a partir de uma série temporal de vazões.

O(s) arquivo(s) de entrada devem estar no formato: dia mês ano valor. Podem ser gerados utilizando o aplicativo [Manejo de Dados HidroWeb](https://www.ufrgs.br/hge/modelos-e-outros-produtos/softwares-de-manejo-e-visualizacao-de-dados-hidrologicos/).


Váriaveis Calculadas
---

- Vazão média de longo período, calculada com a vazão diária de anos com nível de falha aceitável (Configuravel)
- Vazão média de 7 dias calculada para todos os anos com mais de 363 dias de dados. (Configuravel)
- Vazão mínima média de sete dias, com 10 anos de tempo de retorno (Configuravel).
- Vazao máxima no ano.

Configuração
---

Exemplo de arquivo 'config.txt':
```
dias_minimos=363
nome_saida=QRef_
q7n=10
```

Uso
---

Para processar os arquivos de entrada basta executar o programa. A pasta que ele irá utilizar por padrão é a mesma em que ele se encontra.

Para processar dados em outra pasta existem as seguintes opções:

* Arrastar a pasta desejada e soltar sobre o executavel do programa.
* Utilizar a linha de comando como no exemplo abaixo.

```
$ qreferencia c:\dados\
```

Compilando
---

Ferramentas necessárias:
* [MinGW](http://www.mingw.org/) - Minimalist GNU for Windows [(Download)](https://sourceforge.net/projects/mingw/files/Installer/mingw-get-setup.exe/download)
* [NetBeans IDE](https://netbeans.org/) [C/C++] [(Download)](https://netbeans.org/downloads/)

Siga as instruções que estão [aqui](https://netbeans.org/community/releases/80/cpp-setup-instructions.html).

Após instalar tudo basta abrir o projeto no NetBeans e seguir as seguintes etapas:
* Run -> Set Project Configuration -> Release
* Run -> Build Project

O arquivo executavel deve estar disponivel na seguinte pasta:

```
[PASTA DO PROJETO]\dist\Release\MinGW-Windows\
```

O nome da ultima pasta pode ser diferente dependendo da configuração do NetBeans.


Executando em outro PC
---

Para que o programa possa ser executado é necessário que se distribua junto com o executavel a seguinte DLL:
```
C:\MinGW\bin\libgcc_s_dw2-1.dll
```

Assumindo que o MinGW foi instalado em sua pasta padrão C:\MinGW\

Autores e Versões anteriores
---

* Desenvolvido originalmente em Fortran por Bibiana Rodrigues Colossi - [Rhama Consultoria Ambiental](http://rhama.com.br/) / [IPH](http://www.ufrgs.br/iph/) - [UFRGS](http://ufrgs.br)
* Modificado por Carlo Lucca Rossi - [Rhama Consultoria Ambiental](http://rhama.com.br/) / [IPH](http://www.ufrgs.br/iph/) - [UFRGS](http://ufrgs.br)
* Reescrito em C++ por Natanael Rodrigo Zimmer  - [INF](http://inf.ufrgs.br/) - [UFRGS](http://ufrgs.br)