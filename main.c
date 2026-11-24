#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
 * Controle da impressora Elgin (i8 / i9) em C, usando a DLL E1_Impressora01.dll.
 * Desenvolvido com base nos padrões apresentados em aula e no PDF de trabalho.
 */

/* ===================== Ligacao com a DLL ===================== */

typedef int (*FnAbreConexaoImpressora)(int tipo, const char *modelo, const char *conexao, int param);
typedef int (*FnFechaConexaoImpressora)(void);
typedef int (*FnImpressaoTexto)(const char *dados, int posicao, int estilo, int tamanho);
typedef int (*FnCorte)(int avanco);
typedef int (*FnImpressaoQRCode)(const char *dados, int tamanho, int nivelCorrecao);
typedef int (*FnImpressaoCodigoBarras)(int tipo, const char *dados, int altura, int largura, int HRI);
typedef int (*FnAvancaPapel)(int linhas);
typedef int (*FnStatusImpressora)(int param);
typedef int (*FnAbreGavetaElgin)(int pino, int ti, int tf);
typedef int (*FnAbreGaveta)(int pino, int ti, int tf);
typedef int (*FnSinalSonoro)(int qtd, int tempoInicio, int tempoFim);
typedef int (*FnModoPagina)(void);
typedef int (*FnLimpaBufferModoPagina)(void);
typedef int (*FnImprimeModoPagina)(void);
typedef int (*FnModoPadrao)(void);
typedef int (*FnPosicaoImpressaoHorizontal)(int posicao);
typedef int (*FnPosicaoImpressaoVertical)(int posicao);
typedef int (*FnImprimeXMLSAT)(const char *dados, int param);
typedef int (*FnImprimeXMLCancelamentoSAT)(const char *dados, const char *assQRCode, int param);

static FnAbreConexaoImpressora AbreConexaoImpressora;
static FnFechaConexaoImpressora FechaConexaoImpressora;
static FnImpressaoTexto ImpressaoTexto;
static FnCorte Corte;
static FnImpressaoQRCode ImpressaoQRCode;
static FnImpressaoCodigoBarras ImpressaoCodigoBarras;
static FnAvancaPapel AvancaPapel;
static FnStatusImpressora StatusImpressora;
static FnAbreGavetaElgin AbreGavetaElgin;
static FnAbreGaveta AbreGaveta;
static FnSinalSonoro SinalSonoro;
static FnModoPagina ModoPagina;
static FnLimpaBufferModoPagina LimpaBufferModoPagina;
static FnImprimeModoPagina ImprimeModoPagina;
static FnModoPadrao ModoPadrao;
static FnPosicaoImpressaoHorizontal PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical PosicaoImpressaoVertical;
static FnImprimeXMLSAT ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT ImprimeXMLCancelamentoSAT;

static HMODULE dll = NULL;

static int carregaFuncao(void *destino, const char *nome) {
    FARPROC f = GetProcAddress(dll, nome);
    if (f == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }
    *(FARPROC *)destino = f;
    return 1;
}

static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        return 0;
    }

    int ok = 1;
    ok &= carregaFuncao(&AbreConexaoImpressora, "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora, "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto, "ImpressaoTexto");
    ok &= carregaFuncao(&Corte, "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode, "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras, "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel, "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora, "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin, "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta, "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro, "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina, "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina, "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina, "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao, "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical, "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT, "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT, "ImprimeXMLCancelamentoSAT");
    return ok;
}

/* ===================== Variaveis do programa ===================== */

static int conexaoAberta = 0; 
static int tipo = 1; 
static char modelo[50] = "i9"; 
static char conexao[50] = "USB";
static int parametro = 0;

static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

/* ===================== Funcoes auxiliares ===================== */

static void capturarEntrada(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    destino[strcspn(destino, "\r\n")] = '\0';
}

static int lerInteiro(const char *mensagem) {
    char linha[50];
    char *fim;
    while (1) {
        capturarEntrada(mensagem, linha, sizeof(linha));
        long valor = strtol(linha, &fim, 10);
        if (linha[0] != '\0' && *fim == '\0') {
            return (int)valor;
        }
        printf("Digite apenas numeros!\n");
    }
}

static char *lerArquivo(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (arq == NULL) {
        return NULL;
    }
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);

    char *conteudo = (char *)malloc(tamanho + 1);
    if (conteudo == NULL) {
        fclose(arq);
        return NULL;
    }
    size_t lidos = fread(conteudo, 1, tamanho, arq);
    conteudo[lidos] = '\0';
    fclose(arq);
    return conteudo;
}

/* ===================== Funcao de Login ===================== */

int fazerLogin(void) {
    char usuario[50], senha[50];
    for (int tentativa = 1; tentativa <= 3; tentativa++) {
        printf("\n--- TELA DE LOGIN (Tentativa %d de 3) ---\n", tentativa);
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        capturarEntrada("Senha: ", senha, sizeof(senha));

        if (strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0) {
            printf("Login realizado com sucesso!\n");
            return 1;
        } else {
            int restantes = 3 - tentativa;
            if (restantes > 0) {
                printf("Usuario ou senha incorretos. Restam %d tentativa(s).\n", restantes);
            } else {
                printf("Numero maximo de tentativas atingido.\n");
            }
        }
    }
    return 0;
}

/* ===================== Funcao de Menu ===================== */

void exibirMenu(void) {
    printf("\n*****************\n");
    printf("****** MENU IMPRESSORA ******\n");
    printf("*****************\n\n");

    printf("1 - Configurar Conexao\n");
    printf("2 - Abrir Conexao\n");
    printf("3 - Fechar Conexao\n");
    printf("4 - Imprimir Texto\n");
    printf("5 - Imprimir QR Code\n");
    printf("6 - Imprimir Codigo de Barras\n");
    printf("7 - Imprimir XML SAT\n");
    printf("8 - Imprimir XML de Cancelamento SAT\n");
    printf("9 - Abrir Gaveta (Elgin)\n");
    printf("10 - Abrir Gaveta\n");
    printf("11 - Emitir Sinal Sonoro\n");
    printf("0 - Sair\n");
    printf("--------------------------------------\n");
}

/* ===================== Funcoes da impressora ===================== */

void configurarConexao(void) {
    if (!conexaoAberta) {
        tipo = lerInteiro("Digite o tipo de conexao (1 para USB, 2 para Serial, etc.): ");
        capturarEntrada("Digite o modelo da impressora: ", modelo, sizeof(modelo));
        capturarEntrada("Digite a porta de conexao (ex: USB): ", conexao, sizeof(conexao));
        parametro = lerInteiro("Digite o parametro adicional (ex: 0 para padrao): ");
        printf("Parametros de conexao configurados com sucesso.\n");
    } else {
        printf("Conexao ja configurada. Pronta para uso.\n");
    }
}

void abrirConexao(void) {
    if (!conexaoAberta) {
        int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);
        if (retorno == 0) {
            conexaoAberta = 1;
            printf("Conexao aberta com sucesso.\n");
        } else {
            printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Conexao ja esta aberta.\n");
    }
}

void fecharConexao(void) {
    if (conexaoAberta) {
        FechaConexaoImpressora();
        conexaoAberta = 0;
        printf("Conexao fechada.\n");
    } else {
        printf("Nenhuma conexao aberta para fechar.\n");
    }
}

void imprimirTexto(void) {
    if (conexaoAberta) {
        int retorno = ImpressaoTexto("Teste de impressao", 1, 4, 0);
        if (retorno == 0) {
            Corte(5);
            printf("Impressao realizada com sucesso.\n");
        } else {
            printf("Erro ao realizar impressao. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Nao tem conexao com impressora em aberto.\n");
    }
}

void imprimirQRCode(void) {
    if (conexaoAberta) {
        int retorno = ImpressaoQRCode("Teste de impressao", 6, 4);
        if (retorno == 0) {
            Corte(5);
            printf("QR Code impresso com sucesso.\n");
        } else {
            printf("Erro ao imprimir QR Code. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Erro: Conexao nao esta aberta.\n");
    }
}

void imprimirCodigoBarras(void) {
    if (conexaoAberta) {
        int retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
        if (retorno == 0) {
            Corte(5);
            printf("Codigo de barras impresso com sucesso.\n");
        } else {
            printf("Erro ao imprimir codigo de barras. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Erro: Conexao nao esta aberta.\n");
    }
}

void imprimirXMLSAT(void) {
    if (!conexaoAberta) {
        printf("Nao tem conexao com impressora em aberto.\n");
        return;
    }
    char *dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o XMLSAT.xml\n");
        return;
    }
    int retorno = ImprimeXMLSAT(dadosXML, 0);
    Corte(5);
    free(dadosXML);
    if (retorno == 0) {
        printf("XML SAT impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir XML SAT. Codigo de erro: %d\n", retorno);
    }
}

void imprimirXMLCancelamentoSAT(void) {
    if (!conexaoAberta) {
        printf("Nao tem conexao com impressora em aberto.\n");
        return;
    }
    char *dadosXML = lerArquivo("CANC_SAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o CANC_SAT.xml\n");
        return;
    }
    int retorno = ImprimeXMLCancelamentoSAT(dadosXML, ASS_QRCODE, 0);
    Corte(5);
    free(dadosXML);
    if (retorno == 0) {
        printf("XML de cancelamento impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir cancelamento. Codigo de erro: %d\n", retorno);
    }
}

void abrirGavetaElgin(void) {
    if (!conexaoAberta) {
        printf("Nao tem conexao com impressora em aberto.\n");
        return;
    }
    int retorno = AbreGavetaElgin(1, 50, 50);
    if (retorno == 0) {
        printf("Gaveta Elgin aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir gaveta. Codigo de erro: %d\n", retorno);
    }
}

void abrirGaveta(void) {
    if (!conexaoAberta) {
        printf("Nao tem conexao com impressora em aberto.\n");
        return;
    }
    int retorno = AbreGaveta(1, 5, 10);
    if (retorno == 0) {
        printf("Gaveta aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir gaveta. Codigo de erro: %d\n", retorno);
    }
}

void emitirSinalSonoro(void) {
    if (!conexaoAberta) {
        printf("Nao tem conexao com impressora em aberto.\n");
        return;
    }
    int qtd = lerInteiro("Quantidade de apitos: ");
    int retorno = SinalSonoro(qtd, 50, 5);
    if (retorno == 0) {
        printf("Sinal sonoro emitido com sucesso!\n");
    } else {
        printf("Erro ao emitir sinal sonoro. Codigo de erro: %d\n", retorno);
    }
}

/* ===================== Programa principal ===================== */

int main(void) {
    if (!carregarDLL()) {
        printf("Programa encerrado.\n");
        return 1;
    }

    if (!fazerLogin()) {
        printf("Acesso negado.\n");
        FreeLibrary(dll);
        return 1;
    }

    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        opcao = lerInteiro("\nDigite a opcao desejada: ");

        switch (opcao) {
            case 1: configurarConexao(); break;
            case 2: abrirConexao(); break;
            case 3: fecharConexao(); break;
            case 4: imprimirTexto(); break;
            case 5: imprimirQRCode(); break;
            case 6: imprimirCodigoBarras(); break;
            case 7: imprimirXMLSAT(); break;
            case 8: imprimirXMLCancelamentoSAT(); break;
            case 9: abrirGavetaElgin(); break;
            case 10: abrirGaveta(); break;
            case 11: emitirSinalSonoro(); break;
            case 0: 
                fecharConexao();
                printf("Encerrando programa...\n");
                break;
            default:
                printf("Opcao invalida.\n");
                break;
        }
    }

    FreeLibrary(dll);
    return 0;
}
