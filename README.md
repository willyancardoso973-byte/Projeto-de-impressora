# 🖨️ Integração em C com Impressora Elgin i9

### Projeto de comunicação e impressão utilizando a biblioteca `E1_Impressora01.dll`

------------------------------------------------------------------------

## 📘 Sobre o Projeto

Este projeto foi desenvolvido por alunos com o objetivo de criar uma
aplicação na linguagem **C** capaz de se comunicar com a impressora
térmica **Elgin i9**, utilizando a biblioteca de integração
disponibilizada pela Elgin.

A aplicação permite configurar e abrir a conexão com a impressora,
realizar impressões de texto, QR Code, código de barras e arquivos XML
SAT, além de executar funções auxiliares, como acionamento da gaveta de
dinheiro e sinal sonoro.

O programa também conta com um sistema simples de autenticação, com
limite de três tentativas de acesso, e um menu para selecionar as
operações disponíveis.

O trabalho foi desenvolvido como atividade acadêmica da disciplina

Construção de Algoritmos e Programação.

------------------------------------------------------------------------

## 🧰 Tecnologias e Recursos Utilizados

-   **Linguagem C**
-   **GCC / MinGW (TDM-GCC)**
-   **Dev-C++** (ambiente de desenvolvimento, se utilizado)
-   **Biblioteca de integração Elgin:** `E1_Impressora01.dll`
-   **Impressora térmica Elgin i9**
-   **Arquivos XML SAT:** `XMLSAT.xml` e `CANC_SAT.xml`
-   **Git e GitHub** para versionamento e compartilhamento do projeto

> **Atenção:** a versão da DLL precisa ser compatível com a arquitetura
> do compilador utilizado (32 ou 64 bits). Utilize a DLL correspondente
> à arquitetura do GCC/MinGW instalado.

------------------------------------------------------------------------

## 📁 Estrutura do Projeto

Os arquivos necessários para executar o programa devem ficar na mesma
pasta:

``` text
Projeto/
├── main.c
├── E1_Impressora01.dll
├── XMLSAT.xml
├── CANC_SAT.xml
└── .gitignore
```

-   `main.c` → código-fonte principal da aplicação.
-   `E1_Impressora01.dll` → biblioteca utilizada para realizar a
    comunicação com a impressora.
-   `XMLSAT.xml` → arquivo XML utilizado no teste de impressão SAT.
-   `CANC_SAT.xml` → arquivo XML utilizado no teste de cancelamento SAT.
-   `.gitignore` → define arquivos que não devem ser enviados ao
    repositório, como o executável gerado durante a compilação.

------------------------------------------------------------------------

## ⚙️ Instalação e Execução

### 🔽 1. Baixar o Projeto do GitHub

1.  Acesse o repositório do projeto no GitHub.
2.  Clique no botão **Code**.
3.  Selecione **Download ZIP**.
4.  Aguarde o download.

### 📦 2. Extrair o Arquivo ZIP

1.  Localize o arquivo ZIP baixado.
2.  Clique com o botão direito e selecione **Extrair tudo** (*Extract
    All*).
3.  Abra a pasta extraída e confirme que os arquivos necessários estão
    presentes.

### 🧰 3. Preparar o Ambiente

1.  Utilize um compilador **GCC/MinGW** compatível com a arquitetura da
    DLL fornecida.
2.  Caso utilize o Dev-C++, verifique se o compilador está instalado e
    configurado corretamente.
3.  Mantenha `main.c`, `E1_Impressora01.dll`, `XMLSAT.xml` e
    `CANC_SAT.xml` na mesma pasta.
4.  A impressora deve estar conectada e ligada para a realização dos
    testes físicos.

### ▶️ 4. Compilar e Executar

Abra o terminal ou Prompt de Comando na pasta do projeto.

Para compilar, execute:

``` bash
gcc main.c -o impressora.exe
```

Depois, execute o programa:

``` bash
impressora.exe
```

**Importante:** execute o programa a partir da pasta do projeto. A
aplicação procura a DLL e os arquivos XML pelos nomes definidos no
código; por isso, eles precisam estar disponíveis no diretório de
execução.

Se o comando `gcc` não for reconhecido, verifique se o GCC/MinGW está
instalado e se o caminho do compilador foi adicionado à variável de
ambiente `PATH`.

------------------------------------------------------------------------

## 🧪 Testes

O programa disponibiliza opções para testar diferentes recursos da
impressora, incluindo:

-   Autenticação de usuário, com limite de três tentativas.
-   Configuração e abertura da conexão com a impressora.
-   Fechamento da conexão.
-   Impressão de texto simples.
-   Impressão de QR Code.
-   Impressão de código de barras.
-   Impressão de XML SAT.
-   Impressão de XML de cancelamento SAT.
-   Acionamento da gaveta de dinheiro.
-   Emissão de sinal sonoro.

Para os testes de impressão, confirme que a impressora está ligada,
conectada e configurada corretamente. As operações que dependem da
impressora devem ser realizadas com a conexão aberta.

------------------------------------------------------------------------

## 📚 Documentação Adicional

-   Manual e documentação técnica da impressora **Elgin i9**.
-   Documentação da biblioteca de integração **`E1_Impressora01.dll`**.
-   Material e orientações fornecidos pelo professor para a realização
    do trabalho.

------------------------------------------------------------------------

## 👥 Autores

*(preencher em ordem alfabética)*

-   Gustavo Monteiro Ribeiro
-   João Pedro de Lima Valerio
-   Henrique Silva Rodrigues Mathias
-   Willyan Cardoso Andrade

------------------------------------------------------------------------

## 📝 Observações

-   Os nomes dos arquivos utilizados pelo programa devem ser mantidos
    exatamente como definidos no código.
-   A DLL deve permanecer acessível no diretório de execução do
    programa.
-   O executável `impressora.exe` é gerado durante a compilação e pode
    ser recriado a qualquer momento a partir do código-fonte.
-   A comunicação com a impressora depende da compatibilidade entre a
    DLL, o compilador e o ambiente Windows.
