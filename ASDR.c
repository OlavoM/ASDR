/*
ArthurMPassos - https://github.com/ArthurMPassos
OlavoM - https://github.com/OlavoM

Analisador Sintatico Descendente Recursivo
*/

#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

// Definicao de constantes para os atomos da linguagem
typedef enum{
    // Identificadores basicos da Linguagem
    ERRO, 
    IDENTIFICADOR,
    NUMERO_INTEIRO, // DIGITOS+
    NUMERO_REAL,    // NUMERO_INTEIRO.(e|E)(+|-| λ)DIGITO+
    EOS,            // End Of Stream

    AND,            // E lógico 
    BEGIN,          // Início de um comando composto 
    BOOLEAN,        // Tipo booleano 
    CHAR,           // Tipo de dado para definir um caractere 
    DO,             // Usado no bloco de estrutura de repetição 
    ELSE,           // Caso contrário da instrução de seleção 
    END,            // Fim de um bloco ou do programa 
    FALSE,          // Constante booleana para o valor falso 
    IF,             // Determina uma estrutura de condicional 
    INTEGER,        // Tipo de dado para números inteiros 
    MOD,            // Operador de resto 
    NOT,            // Negação lógica 
    OR,             // OU lógico 
    PROGRAM,        // Início do programa 
    READ,           // Define uma função para entrada (leitura) 
    REAL,           // Tipo de Dado para números reais (ponto flutuante) 
    THEN,           // Usado no bloco de estrutura condicional 
    TRUE,           // Constante booleana para o valor verdadeiro 
    WRITE,          // Define uma função para saída (impressão) 
    WHILE,          // Determina um laço com condição no início 

    // Definição dos átomos simples 
    ABRE_PAR,       // Equivale a ( 
    ATRIBUICAO,     // Equivale a := 
    FECHA_PAR,      // Equivale a ) 
    PONTO,          // Equivale a . 
    PONTO_VIRGULA,  // Equivale a ; 
    VIRGULA,        // Equivale a , 

    // Operadores Aritméticos  
    SUBTRACAO,      // Equivale a − 
    ADICAO,         // Equivale a + 
    DIVISAO,        // Equivale a / 
    MULTIPLICACAO,  // Equivale a * 

    // Operadores Relacionais 
    ME,             // Equivale a < 
    MEI,            // Equivale a <= 
    IG,             // Equivale a = 
    DI,             // Equivale a != 
    MA,             // Equivale a > 
    MAI,            // Equivale a >= 

    // Comentários na Linguagem  
    COMENTARIO,  
    
    // Caractere ASCII
    CARACTERE       
    
}TAtomo;

// Estrutura para retornar o atomo e suas informacoes
typedef struct{
  TAtomo atomo;
  int linha; 
  int atributo_numero;
  float atributo_numero_real;
  char atributo_caractere;
  char atributo_ID[15];
}TInfoAtomo;

char *buffer;
int linha = 1;

// Reconhecer o atomo e retornar sua constante numerica
TInfoAtomo obter_atomo(void);
char *le_arquivo(char *nome_arquivo);
void reconhece_id(TInfoAtomo * info_atomo);
void reconhece_num(TInfoAtomo * info_atomo);
void reconhece_comentario_uma_linha(void);
int reconhece_comentario_varias_linhas(void);
float pot (int, int);
void reconhece_num_real(TInfoAtomo * info_atomo);       

void consome(TAtomo atomo);
TAtomo lookahead();

void programa();
void bloco();
void declaracao_de_variaveis();
void tipo();
void variaveis();
void comando_composto();
void comando();
void comando_atribuicao();
void comando_condicional();
void comando();
void comando_enquanto();
void comando_entrada();
void comando_saida();
void expressao();
void expressao_simples();
void operador_simples();
void termo();
void operador_termo();
void fator();

int main( void ){
    buffer = le_arquivo("entrada.pas");

    char *iniBuffer = buffer;

    programa();

    printf("\nFim de programa.\n");

    // No final da analise lexica libera a memoria alocada
    free(iniBuffer);

    return 0;
}

//<programa>::= PROGRAM IDENTIFICADOR “;” <bloco> “;” 
void programa(){
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO_VIRGULA);
}
// <bloco>::= <declaracao_de_variaveis> <comando_composto> 
void bloco(){
    declaracao_de_variaveis();
    comando_composto();
}
// <declaracao_de_variaveis> ::= {<tipo> <variaveis> “;”} 
void declaracao_de_variaveis(){
    TAtomo aux = lookahead();
    while( 
        aux == INTEGER ||
        aux == REAL ||
        aux == CHAR ||
        aux == BOOLEAN 
    ){
        tipo();
        variaveis();
        consome(PONTO_VIRGULA);
        aux = lookahead();
    }
}
// <tipo> ::= INTEGER | REAL | CHAR | BOOLEAN 
void tipo(){
    TAtomo aux = lookahead();
    if( aux == INTEGER ){
        consome(INTEGER);
    }
    else if( aux == REAL ){
        consome(REAL);
    }
    else if( aux == CHAR ){
        consome(CHAR);
    }
    else if( aux == BOOLEAN ){
        consome(BOOLEAN);
    }
}
// <variaveis> ::= IDENTIFICADOR {“,” IDENTIFICADOR } 
void variaveis(){
    consome(IDENTIFICADOR);
    while( lookahead() == VIRGULA ){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}
// <comando_composto> ::= BEGIN <comando> {“;”<comando>} END 
void comando_composto(){
    consome(BEGIN);
    comando();
    while(lookahead() == PONTO_VIRGULA){
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(END);
}
/* 
<comando> ::= <comando_atribuicao>  | 
                <comando_condicional> | 
                <comando_enquanto>   | 
                <comando_entrada>     | 
                <comando_saida>       | 
                <comando_composto> 
*/
void comando(){
    TAtomo aux = lookahead();
    if(aux == IDENTIFICADOR){
        comando_atribuicao();
    }else if(aux == IF){
        comando_condicional();
    }else if(aux == WHILE){
        comando_enquanto();
    }else if(aux == READ){
        comando_entrada();
    }else if(aux == WRITE){
        comando_saida();
    }else if(aux == BEGIN){
        comando_composto();
    }
}
// <comando_atribuicao> ::= IDENTIFICADOR “:=” <expressão> 
void comando_atribuicao(){
    consome(IDENTIFICADOR);
    consome(ATRIBUICAO);
    expressao();
}
// <comando_condicional> ::= IF “(” <expressao> “)” THEN <comando> [ ELSE <comando> ] 
void comando_condicional(){
    consome(IF);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    consome(THEN);
    comando();
    if(lookahead() == ELSE){
        comando();
    }
}
// <comando_enquanto> ::= WHILE “(” <expressao> “)” DO <comando> 
void comando_enquanto(){
    consome(WHILE);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    consome(DO);
    comando();
}

//<laco_do_while> ::= do <comando> while <expressao_condicional>
void laco_do_while(){
    consome(DO);
    comando();
    consome(WHILE);
    expressao();
}

// <comando_entrada> ::= READ “(“ <variaveis> “)”  
void comando_entrada(){
    consome(READ);
    consome(ABRE_PAR);
    variaveis();
    consome(FECHA_PAR);
}
// <comando_saida> ::= WRITE “(“ <expressao> { “,” <expressao> } “)” 
void comando_saida(){
    consome(WRITE);
    consome(ABRE_PAR);
    expressao();
    while (lookahead() == VIRGULA){
        consome(VIRGULA);
        expressao();
    }
    consome(FECHA_PAR);    
}

// <expressao> ::= <expressao_simples> [ OP_RELACIONAL <expressao_simples> ] => TEM Q SER ALGEBRICO
void expressao(){
    expressao_simples();
    TAtomo aux = lookahead();
    if (aux == ME){
        consome(ME);
        expressao_simples();
    }else if (aux == MEI){
        consome(MEI);
        expressao_simples();
    }else if (aux == IG){
        consome(IG);
        expressao_simples();
    }else if (aux == DI){
        consome(DI);
        expressao_simples();
    }else if (aux == MA){
        consome(MA);
        expressao_simples();
    }else if (aux == MAI){
        consome(MAI);
        expressao_simples();
    }
}

// expressao_simples ::= [“+” | “−”] <termo> { <operador_simples> termo } 
void expressao_simples(){
    TAtomo aux = lookahead();
    if(aux == ADICAO){
        consome(ADICAO);
    }else if (aux == SUBTRACAO){
        consome(SUBTRACAO);
    }
    termo();
    aux = lookahead();
    if(
        aux == ADICAO ||
        aux == SUBTRACAO ||
        aux == OR
    ){
        operador_simples();
        termo();
    }
}

// <operador_simples> ::= “+” | “−” | OR 
void operador_simples(){
    TAtomo aux = lookahead();
    if (aux == ADICAO){
        consome(ADICAO);
    }else if (aux == SUBTRACAO){
        consome(SUBTRACAO);
    }else if (aux == OR){
        consome(OR);
    }
}

// <termo> ::= <fator> { <operador_termo> <fator> } 
void termo(){
    fator();
    TAtomo aux = lookahead();
    while(
        aux == MULTIPLICACAO ||
        aux == DIVISAO ||
        aux == MOD ||
        aux == AND
    ){
        operador_termo();
        fator();
    }
}

// <operador_termo> ::= “*” | “/” | MOD | AND  
void operador_termo(){
    TAtomo aux = lookahead();
    if (aux == MULTIPLICACAO){
        consome(MULTIPLICACAO);
    }
    else if (aux == DIVISAO){
        consome(DIVISAO);
    }
    else if (aux == MOD){
        consome(MOD);
    }
    else if (aux == AND){
        consome(AND);   
    }
}

/* 
<fator> ::= IDENTIFICADOR  | 
            NUMERO_INTEIRO | 
            NUMERO_REAL    | 
            CARACTERE      | 
            TRUE           | 
            FALSE          | 
            NOT <fator>    | 
            “(” <expressao> “)” 
*/
void fator(){
    TAtomo aux = lookahead();
    if ( aux == IDENTIFICADOR){
        consome(IDENTIFICADOR);
    }
    else if( aux == NUMERO_INTEIRO){
        consome(NUMERO_INTEIRO);
    }
    else if(aux == NUMERO_REAL){
        consome(NUMERO_REAL);
    }
    else if( aux == CARACTERE){
        consome(CARACTERE);
    }
    else if( aux == TRUE){
        consome(TRUE);
    }
    else if( aux == FALSE){
        consome(FALSE);
    }
    else if( aux == NOT){
        consome(NOT);
        fator();
    }
    else if( aux == ABRE_PAR){
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    }
}

/*
    Obtem um atomo do arquivo texto por vez
*/
TInfoAtomo obter_atomo(){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;
    // Ignora espaco em branco e contagem de linha
    while( *buffer == ' ' || *buffer=='\n'){
        if( *buffer =='\n')
            linha++;
        buffer++;
    }
    info_atomo.linha = linha;
    if( *buffer == '\x0')
        info_atomo.atomo  = EOS;
    // Identificador
    else if( isalpha(*buffer)){
        reconhece_id(&info_atomo);
    }
    // Numero inteiro
    else if( isdigit(*buffer)){
        reconhece_num(&info_atomo);
        // Caso seja um Numero real
        if (*buffer == '.'){
            reconhece_num_real(&info_atomo);       
        }
    }

    // Operador atribuicao
    else if( *buffer == ':' && *(buffer+1)=='='){ // Operador de atribuicao
        info_atomo.atomo = ATRIBUICAO;
        buffer+=2;
    }
    // Operador abre_par
    else if( *buffer == '('){ 
        info_atomo.atomo = ABRE_PAR;
        buffer+=1;
    }
    // Operador fecha_par
    else if( *buffer == ')'){ 
        info_atomo.atomo = FECHA_PAR;
        buffer+=1;
    }
    // Operador ponto
    else if( *buffer == '.'){
        info_atomo.atomo = PONTO;
        buffer+=1;
    }
    // Operador ponto_virgula
    else if( *buffer == ';'){
        info_atomo.atomo = PONTO_VIRGULA;
        buffer+=1;
    }
    // Operador virgula
    else if( *buffer == ','){
        info_atomo.atomo = VIRGULA;
        buffer+=1;
    }
    // Operador subtracao
    else if( *buffer == '-'){
        info_atomo.atomo = SUBTRACAO;
        buffer+=1;
    }
    // Operador adicao
    else if( *buffer == '+'){
        info_atomo.atomo = ADICAO;
        buffer+=1;
    }
    // Operador divisao
    else if( *buffer == '/'){
        info_atomo.atomo = DIVISAO;
        buffer+=1;
    }
    // Operador multiplicacao
    else if( *buffer == '*'){
        info_atomo.atomo = MULTIPLICACAO;
        buffer+=1;
    }
    // Operador menor igual (MEI)
    else if( *buffer == '<' && *(buffer+1)=='='){
        info_atomo.atomo = MEI;
        buffer+=2;
    }
    // Operador menor
    else if( *buffer == '<'){
        info_atomo.atomo = ME;
        buffer+=1;
    }
    // Operador diferente
    else if( *buffer == '!' && *(buffer+1)=='='){
        info_atomo.atomo = DI;
        buffer+=2;
    }
    // Operador igual
    else if( *buffer == '='){
        info_atomo.atomo = IG;
        buffer+=1;
    }
    // Operador maior igual (MAI)
    else if( *buffer == '>' && *(buffer+1)=='='){
        info_atomo.atomo = MAI;
        buffer+=2;
    }
    // Operador maior
    else if( *buffer == '>'){
        info_atomo.atomo = MA;
        buffer+=1;
    }
    // COMENTARIO_UMA_LINHA
    else if( *buffer == '#'){
        reconhece_comentario_uma_linha();
        info_atomo.atomo = COMENTARIO;
    }
    // COMENTARIO_VARIAS_LINHAS
    else if( *buffer == '{'){
        if (reconhece_comentario_varias_linhas())
            info_atomo.atomo = COMENTARIO;
        else
            info_atomo.atomo = ERRO;
    }
    // CARACTERE
    else if( *buffer == '\''){
        buffer+=1;
        if (*(buffer+1)=='\''){
            char temp = *buffer;
            info_atomo.atomo = CARACTERE;
            info_atomo.atributo_caractere = temp;
            buffer+=2;            
        }
        else{
            info_atomo.atomo = ERRO;
        }
    }
    else{
        info_atomo.atomo = ERRO;
    }
    return info_atomo;
}

/*
    Le o comentario de uma linha
*/
void reconhece_comentario_uma_linha(){
    do{
        buffer++;
    }while(*buffer != '\n');
    buffer++;
    linha++;
    return;
}

/*
    Le comentarios de varias linhas e retorna 1
    Caso o comentario nao seja fechado com '}', retorna 0
*/
int reconhece_comentario_varias_linhas(){
    do{
        buffer++;
        if(*buffer == '\x0'){ //erro, chegou no final do arquivo e nao fechou com '}'
            return 0;
        }
        if(*buffer == '\n'){
            linha++;
        }
    } while(*buffer != '}');
    buffer++;
    return 1;
}

/*
    Reconhece numero real a partir do ponto, utilizando os automatos de numero inteiro como auxilio
*/
void reconhece_num_real(TInfoAtomo * info_atomo){
    buffer++; // Passando o "."
    int mantissa = info_atomo->atributo_numero;
    int expoente;
    int sinal = 1; // Ausencia de sinal significa positivo
    float valor;
    if (!((*buffer) == 'e' || (*buffer) == 'E')){
        info_atomo->atomo = ERRO; // Erro caso nao haja o "E" ou "e" esperados
    } 
    else{
        buffer++;
        if(*buffer == '+'){
            sinal = 1;
            buffer++;
        }
        else if(*buffer == '-'){
            sinal = -1;
            buffer++;
        }
        if( !isdigit(*buffer)){
            info_atomo->atomo = ERRO; // Erro caso nao haja digitos apos a verificacao de sinal
        }
        else{
            reconhece_num(info_atomo);
            expoente = info_atomo->atributo_numero;
            valor = mantissa * (pot(10, sinal*expoente));
            info_atomo->atributo_numero_real = valor;
            info_atomo->atomo = NUMERO_REAL;
        }
    }
    return;
}

/*
    Restorna o resultado da potencia x^y , supondo y nao negativo
*/
float pot (int x, int y){
    float result = 1;
    if (y>=0){ // Para expoentes positivos
        for(int i = 0; i<y; i++){
            result = result*x;
        }
    }
    else{ // Para expoentes negativos
        for(int i = 0; i< -y; i++){
            result = result/x;
        }
    }
    return result;
}

/*
    Automato finito que reconhece identificadores
*/
void reconhece_id(TInfoAtomo * info_atomo){
    char *iniID=buffer;
//id0
    if( isalpha(*buffer)){
        buffer++;
        goto id1;
    }
    info_atomo->atomo = ERRO;
    return;
id1:
    if( isalpha(*buffer) || isdigit(*buffer)){
        buffer++;
        goto id1;
    }
    goto id2;

id2:
    // Recorta o identificador do buffer
    strncpy(info_atomo->atributo_ID,iniID,buffer-iniID);
    info_atomo->atributo_ID[buffer-iniID] = '\x0';
    if(stricmp(info_atomo->atributo_ID,"while")==0)
        info_atomo->atomo = WHILE;
    else if(stricmp(info_atomo->atributo_ID,"and")==0)
        info_atomo->atomo = AND;
    else if(stricmp(info_atomo->atributo_ID,"begin")==0)
        info_atomo->atomo = BEGIN;
    else if(stricmp(info_atomo->atributo_ID,"boolean")==0)
        info_atomo->atomo = BOOLEAN;
    else if(stricmp(info_atomo->atributo_ID,"char")==0)
        info_atomo->atomo = CHAR;
    else if(stricmp(info_atomo->atributo_ID,"do")==0)
        info_atomo->atomo = DO;
    else if(stricmp(info_atomo->atributo_ID,"else")==0)
        info_atomo->atomo = ELSE;
    else if(stricmp(info_atomo->atributo_ID,"end")==0)
        info_atomo->atomo = END;
    else if(stricmp(info_atomo->atributo_ID,"false")==0)
        info_atomo->atomo = FALSE;
    else if(stricmp(info_atomo->atributo_ID,"if")==0)
        info_atomo->atomo = IF;
    else if(stricmp(info_atomo->atributo_ID,"if")==0)
        info_atomo->atomo = IF;
    else if(stricmp(info_atomo->atributo_ID,"integer")==0)
        info_atomo->atomo = INTEGER;
    else if(stricmp(info_atomo->atributo_ID,"mod")==0)
        info_atomo->atomo = MOD;
    else if(stricmp(info_atomo->atributo_ID,"not")==0)
        info_atomo->atomo = NOT;
    else if(stricmp(info_atomo->atributo_ID,"or")==0)
        info_atomo->atomo = OR;
    else if(stricmp(info_atomo->atributo_ID,"program")==0)
        info_atomo->atomo = PROGRAM;
    else if(stricmp(info_atomo->atributo_ID,"read")==0)
        info_atomo->atomo = READ;
    else if(stricmp(info_atomo->atributo_ID,"real")==0)
        info_atomo->atomo = REAL;
    else if(stricmp(info_atomo->atributo_ID,"then")==0)
        info_atomo->atomo = THEN;
    else if(stricmp(info_atomo->atributo_ID,"true")==0)
        info_atomo->atomo = TRUE;
    else if(stricmp(info_atomo->atributo_ID,"write")==0)
        info_atomo->atomo = WRITE;
    else
        info_atomo->atomo = IDENTIFICADOR;
}

/*
    Automato finito que reconhece numeros inteiros
*/
void reconhece_num(TInfoAtomo * info_atomo){
    char *iniNum=buffer;
    char strNUM[15];
//n0
    if( isdigit(*buffer)){
        buffer++;
        goto n1;
    }
    info_atomo->atomo = ERRO;
    return;
n1:
    if(isdigit(*buffer)){
        buffer++;
        goto n1;
    }
    if(isalpha(*buffer)){
        info_atomo->atomo = ERRO;
        return;
    }
    // [outro]
    goto n2;

n2:
    // Recorta o numero do buffer
    strncpy(strNUM,iniNum,buffer-iniNum);
    strNUM[buffer-iniNum] = '\x0';
    // converte vetor de char para numero inteiro
    info_atomo->atributo_numero = atoi(strNUM);
    info_atomo->atomo = NUMERO_INTEIRO;
}

/*
    Le o arquivo e armazenada todo o arquivo em bloco na memoria e
    Atribui o inicio do bloco ponteiro buffer.
*/
char *le_arquivo(char *nome_arquivo){
    // abre o arquivo
    FILE *fp_entrada;
    fp_entrada = fopen(nome_arquivo,"r");
    if(fp_entrada == NULL){
        printf("erro na abertura do arquivo de entrada.\n");
        exit(1);
    }

    char *buffer_entrada;
    // Calcula o tamanho do arquivo para alocar um buffer na memoria.
    fseek(fp_entrada,0,SEEK_END); // desloca o ponteiro do arquivo do inicio para o fim
    int size_entrada = ftell(fp_entrada); // retorna qtd de bytes deslocados
    fseek(fp_entrada,0,SEEK_SET); // volta o ponteiro para o inicio do arquivo

    // Aloca memoria
    buffer_entrada = (char*)calloc(size_entrada+1, sizeof(char));

    // Le o arquivo todo para o buffer_entrada
    fread(buffer_entrada, sizeof(char),size_entrada, fp_entrada);
    // Fecha o arquivo
    fclose(fp_entrada);
    return buffer_entrada;
}


void imprimeAtomo(TAtomo info_atomo){
    if( info_atomo == IDENTIFICADOR )
        printf("IDENTIFICADOR");
    else if( info_atomo == NUMERO_INTEIRO )
        printf("NUMERO_INTEIRO");
    else if( info_atomo == NUMERO_REAL )
        printf("NUMERO_REAL");
    else if( info_atomo == WHILE )
        printf("WHILE");
    else if( info_atomo == ERRO ){
        printf("ERRO");
    }
    // Print dos identificadores da linguagem
    else if( info_atomo == AND)
        printf("AND");
    else if( info_atomo == BEGIN)
        printf("BEGIN");
    else if( info_atomo == BOOLEAN)
        printf("BOOLEAN");
    else if( info_atomo == CHAR)
        printf("CHAR");
    else if( info_atomo == DO)
        printf("DO");
    else if( info_atomo == ELSE)
        printf("ELSE");
    else if( info_atomo == END)
        printf("END");
    else if( info_atomo == FALSE)
        printf("FALSE");
    else if( info_atomo == IF)
        printf("IF");
    else if( info_atomo == INTEGER)
        printf("INTEGER");
    else if( info_atomo == MOD)
        printf("MOD");
    else if( info_atomo == NOT)
        printf("NOT");
    else if( info_atomo == OR)
        printf("OR");
    else if( info_atomo == PROGRAM)
        printf("PROGRAM");
    else if( info_atomo == READ)
        printf("READ");
    else if( info_atomo == REAL)
        printf("REAL");
    else if( info_atomo == THEN)
        printf("THEN");
    else if( info_atomo == TRUE)
        printf("TRUE");
    else if( info_atomo == WRITE)
        printf("WRITE");
        
    // Print dos atomos simples
    else if( info_atomo == ABRE_PAR)
        printf("ABRE_PAR");        
    else if( info_atomo == ATRIBUICAO)
        printf("ATRIBUICAO");
    else if( info_atomo == FECHA_PAR)
        printf("FECHA_PAR");
    else if( info_atomo == PONTO)
        printf("PONTO");
    else if( info_atomo == PONTO_VIRGULA)
        printf("PONTO_VIRGULA");
    else if( info_atomo == VIRGULA)
        printf("VIRGULA");
    
    // Print dos operadores aritmeticos
    else if( info_atomo == SUBTRACAO)
        printf("SUBTRACAO");
    else if( info_atomo == ADICAO)
        printf("ADICAO");
    else if( info_atomo == DIVISAO)
        printf("DIVISAO");
    else if( info_atomo == MULTIPLICACAO)
        printf("MULTIPLICACAO");

    // Print dos operadores relacionais 
    else if( info_atomo == ME)
        printf("ME");
    else if( info_atomo == MEI)
        printf("MEI");
    else if( info_atomo == IG)
        printf("IG");
    else if( info_atomo == DI)
        printf("DI");
    else if( info_atomo == MA)
        printf("MA");
    else if( info_atomo == MAI)
        printf("MAI");

    // Print do atomo de comentario
    else if( info_atomo == COMENTARIO)
        printf("COMENTARIO");

    // Print do atomo de comentario
    else if( info_atomo == CARACTERE)
        printf("CARACTERE");
}

void consome(TAtomo atomoEsperado){
    TInfoAtomo aux = obter_atomo();
    TAtomo proximoAtomo = aux.atomo;

    // evita considerar atomo comentario
    while (proximoAtomo == COMENTARIO && proximoAtomo != EOS){
        aux = obter_atomo();
        proximoAtomo = aux.atomo;
    }
    
    if(proximoAtomo == atomoEsperado){
        printf("encontrou!: '");
        imprimeAtomo(atomoEsperado);
        printf("' na linha %d", aux.linha);
        printf("'\n");
        return;
    }
    else{
        printf("erro sintatico: esperado '");
        imprimeAtomo(atomoEsperado);
        printf("' encontrado '");
        imprimeAtomo(proximoAtomo);
        printf("' na linha %d", aux.linha);
        printf("'\n");
        exit(1);
    }
}

TAtomo lookahead(){
    void *bufferTemp = buffer;
    int linhaTemp = linha;
    TAtomo proximoAtomo;
    // evita considerar atomo comentario
    do{
        proximoAtomo = obter_atomo().atomo;
    } while (proximoAtomo == COMENTARIO && proximoAtomo != EOS);

    buffer = bufferTemp;
    linha = linhaTemp;

    return proximoAtomo;
}