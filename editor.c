#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

//se cursor for menor que tamanho da lista
typedef struct linha {
    char *texto; // armazena o texto da linha
    int cursorIndex; // armazena a posição do cursor na linha
    struct linha *linha;
    struct linha *prox; // ponteiro para a próxima linha na lista encadeada
    struct linha *antes; // ponteiro para a linha anteserior na lista encadeada
    int marked;
} linha;

typedef struct Cursor {
    int numeroLinha;
    int indice;
} Cursor;

typedef struct Pilha {
    char *texto;
    struct Pilha *prox;
} Pilha;

int M;
// declarações //
linha *inicializa_lista(linha *);
void Cursor_(int, char);
void mostrarCursor(linha *, Cursor *);
void carregarArquivo(char *, linha *);
void mostrarLinha(linha *, Cursor *);
void inserirTexto(linha *, char *, Cursor *);
void deletarTexto(linha *, Cursor *);
void moveCursor(linha *, Cursor *, char *);
void linhaAnterior(linha **, Cursor *);
void proximaLinha(linha *, Cursor *);
void juntarLinhas(linha *);
void separaLinha(linha *, Cursor *);
void antigo(linha *, char *, Cursor *);
int procurarPalavra(linha *, char *, Cursor *);
void substituirPalavra(linha *, char *, char *, Cursor *);
void C(linha *, Cursor *, Pilha **);
void X(linha *, Cursor *, Pilha **);
void V(linha *, Pilha *, Cursor *);
void mostrarPilha(Pilha *);
    
linha *inicializa_lista(linha *cabeca){
    linha *atual = malloc(sizeof(linha));
    atual = cabeca;
    atual->prox = NULL;
    atual->texto = NULL;
    atual->cursorIndex = 0;
    M = 0;
    return atual;
}

void Cursor_(int qt, char caracter){
    for (int i = 0; i < qt; i++)
    {
        printf("%c", caracter);
    }
    printf("%s", "^");
    printf("%s", "\n");
    if (M != -1){
            printf("\n");
            for (int i = 0; i < M; i++) printf(" ");
                printf("M");
        }
}

void mostrarCursor(linha *atual, Cursor *cursor) {
    if (cursor->indice <= strlen(atual->texto)) {
            printf("\n%d%c%d%c", cursor->numeroLinha, ',', cursor->indice, '>');
	}
}

void carregarArquivo(char *arquivo,  linha *cabeca) {
    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s", arquivo, "\n");
        return;
    }
    linha *atual = inicializa_lista(cabeca);
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        int tamanho = strlen(buffer);
        // remover o caractere de nova linha
        if (buffer[tamanho - 1] == '\n') {
            buffer[tamanho - 1] = '\0';
            tamanho--;
        }

        // alocar memória para o texto da linha
        atual->texto = (char *) malloc(tamanho + 1);
        strcpy(atual->texto, buffer);

        // criar uma nova linha
        struct linha *novaLinha = ( linha *) malloc(sizeof(linha));
        novaLinha->prox = NULL;
        novaLinha->texto = NULL;
        novaLinha->cursorIndex = 0;
        atual->prox = novaLinha;
        novaLinha->antes = atual;
        atual = novaLinha;
    }

    fclose(fp);
}

void sobrescreverArquivo(linha *cabeca, char *arquivo) { 
    FILE *fp = fopen(arquivo, "w");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }
    linha *atual = cabeca;
    while (atual != NULL) {
        fprintf(fp, "%s\n", atual->texto);
        atual = atual->prox;
    }
    fclose(fp);
}

void mostrarLinha(linha *cabeca, Cursor *cursor) {
    linha *atual = cabeca;
    int count = 0;
    while (atual != NULL) {
        if (count == cursor->numeroLinha) {
            printf("%s\n", atual->texto);
            Cursor_(cursor->indice, ' ');
		}
        count++;
        atual = atual->prox;
    }
}

void inserirTexto(linha *cabeca, char *texto,  Cursor *cursor) {
    linha *atual = cabeca;
    for (int i = 0; i < cursor->numeroLinha; i++) {
        atual = atual->prox;
    }
    int atualTamanhoTexto = strlen(atual->texto);
    int tamanhoTexto = strlen(texto);
    // Aloca o novo texto
    char *novoTexto = malloc(atualTamanhoTexto + tamanhoTexto + 1);
    // Copia o texto atual e o novo texto para ser inserido 
    strcpy(novoTexto, atual->texto);
    strcpy(novoTexto + cursor->indice, texto);
    strcpy(novoTexto + cursor->indice + tamanhoTexto, atual->texto + cursor->indice);
    // Atualiza a linha atual
    free(atual->texto);
    atual->texto = novoTexto;

    // Atualiza a posicao do cursor
    cursor->indice += tamanhoTexto;
}

void deletarTexto(linha *cabeca, Cursor *cursor) {
    linha *atual = cabeca;
        for (int i = 0; i < cursor->numeroLinha; i++) {
        atual = atual->prox;
    }
    
    int atualTamanhoTexto = strlen(atual->texto);

    // Aloca o novo texto
    char *novoTexto = malloc(atualTamanhoTexto);

    // Copia o texto atual e exclui o que está na posição do cursor
    strncpy(novoTexto, atual->texto, cursor->indice);
    strncpy(novoTexto + cursor->indice, atual->texto + cursor->indice + 1, atualTamanhoTexto - cursor->indice - 1);
    novoTexto[atualTamanhoTexto - 1] = '\0';

    free(atual->texto);
    atual->texto = novoTexto;
}

void moveCursor( linha *atual,  Cursor *cursor, char *direction) {
    char *movimento = (direction - 1);
    if (*movimento == 'F') {
        if (cursor->indice < strlen(atual->texto)) {
            cursor->indice++;
        }
    } else if (*movimento == 'T') {
        if (cursor->indice > 0) {
            cursor->indice--;
        }
    } else if (*movimento == '0') {
        cursor->indice = 0;
    } else if (*movimento == 'P') { 
        int i = cursor->indice;
        while (i < strlen(atual->texto) && !isspace(atual->texto[i])) {
            i++;
        }
        while (i < strlen(atual->texto) && isspace(atual->texto[i])) {
            i++;
        }
        cursor->indice = i;
    } else if (*movimento == 'Q') {
        int i = cursor->indice;
        while (i > 0 && !isspace(atual->texto[i-1])) {
            i--;
        }
        cursor->indice = i;
    } else if (*movimento == '$') {
        cursor->indice = strlen(atual->texto);
    } else if (*movimento == ':') {
        movimento++;
        char numero[100];
        for (int i=0; *movimento != '\0'; movimento++, i++) numero[i] = *movimento;
        int numeroLinha = atoi(numero);
        linha **atual1 = &atual;
        while (cursor->numeroLinha != numeroLinha){
            if (cursor->numeroLinha > numeroLinha){
            cursor->numeroLinha = numeroLinha;
            cursor->indice = 0;    
                    }
            else{
                proximaLinha( atual, cursor);
            }
        }
        cursor->indice = 0;
    } else if (*movimento == ':' && *(movimento+1) == 'F' ) {
        while (atual->prox != NULL) {
            cursor->numeroLinha++;
            atual = atual->prox;
        }
    }
}

void linhaAnterior(linha **cabeca, Cursor *cursor) {
    linha *atual = *cabeca;
    if (cursor->numeroLinha == 0) {
        return;
  }
    cursor->numeroLinha--;
    for (int i = 0; i < cursor->numeroLinha; i++) {
        atual = atual->prox;
  }
    cursor->indice = strlen(atual->texto);
}

void proximaLinha(linha *atual, Cursor *cursor) {
    if (atual->prox != NULL) {
        atual = atual->prox;
        cursor->numeroLinha++;
        cursor->indice = MIN(cursor->indice, strlen(atual->texto));
    }
    else{
        return;
    }   
}

void juntarLinhas(linha *atual) {
    if (atual->prox != NULL) {
        
        int atualTamanhoTexto = strlen(atual->texto);
        int proxTamanhoTexto = strlen(atual->prox->texto);
        int newTamanhoTexto = atualTamanhoTexto + proxTamanhoTexto;

        // Aloca o novo texto
        char *novoTexto = malloc(newTamanhoTexto + 1);
        novoTexto[newTamanhoTexto] = '\0';
        // Copia o texto atual e o texto da proxima linha e transforma em um novo texto
        strncpy(novoTexto, atual->texto, atualTamanhoTexto);
        strncpy(novoTexto + atualTamanhoTexto, atual->prox->texto, proxTamanhoTexto);

        // Atualiza a linha atual
        free(atual->texto);
        atual->texto = novoTexto;

        // Atualiza a lista encadeada
        linha *temp = atual->prox;
        atual->prox = temp->prox;
        if (temp->prox != NULL) {
            temp->prox->antes = atual;
        }

        // Libera memoria
        free(temp->texto);
        free(temp);
    }
}

void separaLinha(linha *cabeca, Cursor *cursor) {
    linha *atual = cabeca;
    for (int i = 0; i < cursor->numeroLinha; i++) {
        atual = atual->prox;
    }
    // Aloca a nova linha
    linha *novaLinha = malloc(sizeof(struct linha));

    // Inicializa a nova linha
    novaLinha->texto = malloc(strlen(atual->texto + cursor->indice) + 1);
    strcpy(novaLinha->texto, atual->texto + cursor->indice);
    novaLinha->prox = atual->prox;
    novaLinha->antes = atual;

    // Atualiza a linha atual
    atual->texto[cursor->indice] = '\0';
    atual->prox = novaLinha;

    // Atualiza o cursor
    cursor->numeroLinha++;
    cursor->indice = 0;
}

int procurarPalavra(linha *cabeca, char *texto, Cursor *cursor) {
    int numeroLinha = 0;
    struct linha *atual = cabeca;
    while (atual != NULL) {
        char *found = strstr(atual->texto + cursor->indice, texto);
        if (found != NULL) {
            cursor->numeroLinha = numeroLinha;
            cursor->indice = found - atual->texto;
            return 1;
        }
        atual = atual->prox;
        numeroLinha++;
        cursor->indice = 0;
    }
	return 0;
}

void substituirPalavra(linha *cabeca, char *antigo, char *novo, Cursor *cursor) {
	char *padrao = antigo;
	int antigoTamanhoTexto = strlen(antigo);
	int novoTamanhoTexto = strlen(novo);
	int diferencaTamanho = novoTamanhoTexto - antigoTamanhoTexto;
	while (procurarPalavra(cabeca, padrao, cursor)) {
    for (int i = 0; i < antigoTamanhoTexto; i++) {
        deletarTexto(cabeca, cursor);
		}
    for (int i = 0; i < novoTamanhoTexto; i++) {
        inserirTexto(cabeca, &novo[i], cursor);
		}
		cursor->indice += diferencaTamanho;
	}
	free(cabeca->texto);
    cabeca->texto = novo;
}
	
void C(linha *cabeca, Cursor *cursor, Pilha **memoPilha) {
	linha *atual = cabeca;
    for (int i = 0; i < cursor->numeroLinha; i++) {
        atual = atual->prox;
    }
    int inicio = M;
    int fim = cursor->indice;
    int comprimento = fim - inicio + 1;
    char *texto = malloc(comprimento + 1);
    int indice = 0;
    for (int i = inicio; i <= fim; i++) {
        texto[indice++] = atual->texto[i];
    }
    texto[indice] = '\0';
    Pilha *novaPilha = malloc(sizeof(Pilha));
    novaPilha->texto = texto;
    novaPilha->prox = *memoPilha;
    *memoPilha = novaPilha;
}

void X(linha *cabeca, Cursor *cursor, Pilha **memoPilha) {
	linha *atual = cabeca;
	for (int i = 0; i < cursor->numeroLinha; i++) {
		atual = atual->prox;
	}
	int inicio = M;
	int fim = cursor->indice;
	int comprimento = fim - inicio + 1;
	char *texto = malloc(comprimento + 1);
	int indice = 0;
	for (int i = inicio; i <= fim; i++) {
		texto[indice++] = atual->texto[i];
	}
	texto[indice] = '\0';
	Pilha *novaPilha = malloc(sizeof(Pilha));
	novaPilha->texto = texto;
	novaPilha->prox = *memoPilha;
	*memoPilha = novaPilha;
	int tamanho = strlen(atual->texto);
	for (int i = inicio; i < fim; i++) {
		for (int j = inicio; j < tamanho - 1; j++) {
			atual->texto[j] = atual->texto[j + 1];
		}
		tamanho--;
		indice--;
	}
	atual->texto[tamanho] = '\0';
}

void V(linha *atual, Pilha *memoPilha, Cursor *cursor){
	if (memoPilha->prox != NULL) {
		Pilha *temp = memoPilha;
		memoPilha = memoPilha->prox;
		inserirTexto(atual, temp->texto, cursor);
		free(temp);
  }
}

void mostrarPilha(Pilha *pilha) {
    int indice = 1;
    Pilha *atual = pilha;
    while (atual != NULL) {
        printf("%d. %s\n", indice, atual->texto);
        indice++;
        atual = atual->prox;
    }
}

int main() {
    char input[256];
    char texto[256];
    char *antigo;
	char *novo;
	antigo = (char*)calloc(256, sizeof(char));
    novo = (char*)calloc(256, sizeof(char));
    linha *cabeca = (linha*) malloc(sizeof( linha));
    linha *atual = cabeca;
    Cursor *cursor = malloc(sizeof(Cursor));
    Pilha *memoPilha = (Pilha*) malloc(sizeof(Pilha));
    memoPilha->prox = NULL;

    cabeca->texto = "";
    cabeca->antes = NULL;
    cabeca->prox = NULL;

    printf("Entre com o nome do arquivo: ");
    do
    {
        mostrarCursor(atual, cursor);
        scanf("%s", input);
        char pos_comando[256];
        int j;
        
        // Limpa o a variável que recebe o pós-comando
        for (int i = 0; i < 256; i++)
        {
            pos_comando[i] = ' ';
        }

        // Recebe o valor do comando
        for (int i = 0; i < 256; i++)
        {
            pos_comando[i] = input[i + 1];
        }
        
        int contador = 0;
        scanf("%[^\n]", pos_comando);
        while(input[contador] != '\0'){
            switch (input[contador++])
            {
            case 'A':
                carregarArquivo(pos_comando, cabeca);
                input[contador] = '\0';
                break;
            case 'E':
                sobrescreverArquivo(cabeca, pos_comando);
                break;
            case 'I':
                inserirTexto(atual, pos_comando, cursor);
                input[contador] = '\0';
                break;
            case 'D':
                deletarTexto(atual, cursor);
                input[contador] = '\0';
                break;
            case 'J':
                proximaLinha(atual, cursor);
                break;
            case 'H':
                linhaAnterior(&atual, cursor);
                break;
            case '!':
                exit(0);
                break;
            case 'F':
                 moveCursor(atual, cursor, &input[contador]);
                break;
            case 'T':
                moveCursor(atual, cursor, &input[contador]);
                break;
            case '0':
                moveCursor(atual, cursor, &input[contador]);
                break;
            case 'P':
                moveCursor(atual, cursor, &input[contador]);
                break;
            case 'Q':
                moveCursor(atual, cursor, &input[contador]);
                break;
            case '$':
                moveCursor(atual, cursor, &input[contador]);
                break;
            case ':': 
                moveCursor(atual, cursor, &input[contador]);
                break; 
            case 'U':
                juntarLinhas(atual);
                input[contador] = '\0';
                break;
            case 'N':
                separaLinha(atual, cursor);
                input[contador] = '\0';
                break;
            case 'B':
                procurarPalavra(cabeca, pos_comando, cursor);
                break;
            case 'M':
                M = cursor->indice;
                break;
            case 'S':
                j =0;
                for (int i=0; pos_comando[i] != '/';j++, i++) antigo[i] = pos_comando[i];
                j++;
                for (int i=0; pos_comando[j] != '\0';j++, i++) novo[i] = pos_comando[j];
                substituirPalavra(atual, antigo, novo, cursor);
                break;
			case 'C':
                C(atual, cursor, &memoPilha);
				break;	
			case 'X':
				X(atual, cursor, &memoPilha);
				break;
			case 'V':
                V(atual, memoPilha, cursor);
                break;	
			case 'Z':
                mostrarPilha(memoPilha);
				break;
            }
    }
    mostrarLinha(cabeca, cursor);
    } while (1);
}
