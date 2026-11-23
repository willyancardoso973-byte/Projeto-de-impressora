#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
 * Projeto: Controle da impressora termica Elgin i8/i9
 * Versao 1 - estrutura inicial, autenticacao e conexao.
 *
 * Nesta etapa, o programa carrega a DLL, realiza o login,
 * permite configurar/abrir a conexao e apresenta um menu inicial.
 * As demais operacoes da impressora serao acrescentadas nos proximos commits.
 */

/* Tipos das funcoes exportadas pela DLL da Elgin. */
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

/* Ponteiros para os comandos disponibilizados pela DLL. */
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

/* Localiza uma funcao pelo nome dentro da DLL. */
static int carregaFuncao(void *destino, const char *nome) {
    FARPROC funcao = GetProcAddress(dll, nome);

    if (funcao == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }

    *(FARPROC *)destino = funcao;
    return 1;
}

/* Carrega a biblioteca e associa os 19 comandos aos ponteiros. */
static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");

    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n",
               GetLastError());
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

/* Dados utilizados para configurar a comunicacao com a impressora. */
static int conexaoAberta = 0;
static int tipo = 1;
static char modelo[50] = "i9";
static char conexao[50] = "USB";
static int parametro = 0;

/* Le uma linha de texto sem manter o ENTER no final. */
static void capturarEntrada(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    destino[strcspn(destino, "\r\n")] = '\0';
}

/* Le um numero inteiro e repete a pergunta se a entrada nao for numerica. */
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

/* Solicita usuario e senha, permitindo no maximo tres tentativas. */
static int fazerLogin(void) {
    char usuario[50];
    char senha[50];

    for (int tentativa = 1; tentativa <= 3; tentativa++) {
        printf("\n--- TELA DE LOGIN (Tentativa %d de 3) ---\n", tentativa);
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        capturarEntrada("Senha: ", senha, sizeof(senha));

        if (strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0) {
            printf("Login realizado com sucesso!\n");
            return 1;
        }

        printf("Usuario ou senha incorretos. Restam %d tentativa(s).\n",
               3 - tentativa);
    }

    printf("Numero maximo de tentativas atingido.\n");
    return 0;
}

/* Solicita os parametros de comunicacao antes de abrir a conexao. */
static void configurarConexao(void) {
    if (conexaoAberta) {
        printf("A conexao ja esta aberta.\n");
        return;
    }

    tipo = lerInteiro("Digite o tipo de conexao (1 para USB, 2 para Serial, etc.): ");
    capturarEntrada("Digite o modelo da impressora: ", modelo, sizeof(modelo));
    capturarEntrada("Digite a porta de conexao (ex: USB): ", conexao, sizeof(conexao));
    parametro = lerInteiro("Digite o parametro adicional (ex: 0 para padrao): ");

    printf("Parametros de conexao configurados com sucesso.\n");
}

/* Abre a conexao usando os parametros definidos pelo usuario. */
static void abrirConexao(void) {
    if (conexaoAberta) {
        printf("A conexao ja esta aberta.\n");
        return;
    }

    int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);

    if (retorno == 0) {
        conexaoAberta = 1;
        printf("Conexao aberta com sucesso.\n");
    } else {
        printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
    }
}

/* Menu reduzido desta primeira etapa do desenvolvimento. */
static void exibirMenu(void) {
    printf("\n============================================\n");
    printf("           MENU INICIAL - IMPRESSORA\n");
    printf("============================================\n");
    printf("1 - Configurar conexao\n");
    printf("2 - Abrir conexao\n");
    printf("0 - Sair\n");
}

/* Executa o fluxo inicial: carregar DLL, autenticar e acessar o menu. */
int main(void) {
    if (!carregarDLL()) {
        printf("Nao foi possivel inicializar a biblioteca. Programa encerrado.\n");
        if (dll != NULL) {
            FreeLibrary(dll);
        }
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
        opcao = lerInteiro("Digite a opcao desejada: ");

        switch (opcao) {
            case 1:
                configurarConexao();
                break;
            case 2:
                abrirConexao();
                break;
            case 0:
                printf("Encerrando programa...\n");
                break;
            default:
                printf("Opcao invalida. Escolha uma opcao do menu.\n");
                break;
        }
    }

    /* Evita deixar a conexao aberta ao encerrar o programa. */
    if (conexaoAberta && FechaConexaoImpressora != NULL) {
        FechaConexaoImpressora();
    }

    FreeLibrary(dll);
    return 0;
}
