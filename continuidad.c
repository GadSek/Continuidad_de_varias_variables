#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

//-------------------------------
// Estructuras para el interprete
//-------------------------------
typedef enum {
    NUMERO, VARIABLE, OPERADOR, FUNCION, PAR_IZQ, PAR_DER
} TipoToken;

typedef struct {
    TipoToken tipo;
    double valor;      // Para números
    char simbolo[10];  // Para operadores, funciones, variables
} Token;

//-------------------------------
// Funciones del interprete
//-------------------------------
int es_funcion(const char* s) {
    const char* funciones[] = {"sin", "cos", "exp", "log", "sqrt", NULL};
    for (int i = 0; funciones[i] != NULL; i++) {
        if (strcmp(s, funciones[i]) == 0) return 1;
    }
    return 0;
}

Token* tokenizar(const char* expr, int* num_tokens) {
    Token* tokens = malloc(100 * sizeof(Token));
    int i = 0, j = 0;

    while (expr[i] != '\0') {
        if (isspace(expr[i])) { i++; continue; }

        // Números (ej: 3.14, -5.2)
        if (isdigit(expr[i]) || (expr[i] == '-' && isdigit(expr[i+1]))) {
            char* endptr;
            tokens[j].tipo = NUMERO;
            tokens[j].valor = strtod(&expr[i], &endptr);
            i = endptr - expr;
            j++;
        }
        // Variables (x, y)
        else if (expr[i] == 'x' || expr[i] == 'y') {
            tokens[j].tipo = VARIABLE;
            tokens[j].simbolo[0] = expr[i];
            tokens[j].simbolo[1] = '\0';
            j++; i++;
        }
        // Funciones (sin, cos, sqrt, etc.)
        else if (isalpha(expr[i])) {
            int k = 0;
            while (isalpha(expr[i])) {
                tokens[j].simbolo[k++] = expr[i++];
            }
            tokens[j].simbolo[k] = '\0';
            tokens[j].tipo = es_funcion(tokens[j].simbolo) ? FUNCION : VARIABLE;
            j++;
        }
        // Operadores y paréntesis
        else {
            tokens[j].simbolo[0] = expr[i];
            tokens[j].simbolo[1] = '\0';
            if (expr[i] == '(') tokens[j].tipo = PAR_IZQ;
            else if (expr[i] == ')') tokens[j].tipo = PAR_DER;
            else tokens[j].tipo = OPERADOR;
            j++; i++;
        }
    }

    *num_tokens = j;
    return tokens;
}

double evaluar(Token* tokens, int num_tokens, double x, double y) {
    double pila[100];
    int tope = -1;

    for (int i = 0; i < num_tokens; i++) {
        Token t = tokens[i];
        if (t.tipo == NUMERO) {
            pila[++tope] = t.valor;
        }
        else if (t.tipo == VARIABLE) {
            pila[++tope] = (t.simbolo[0] == 'x') ? x : y;
        }
        else if (t.tipo == OPERADOR || t.tipo == FUNCION) {
            double a, b;
            if (strcmp(t.simbolo, "+") == 0) {
                b = pila[tope--]; a = pila[tope--];
                pila[++tope] = a + b;
            }
            else if (strcmp(t.simbolo, "-") == 0) {
                b = pila[tope--]; a = pila[tope--];
                pila[++tope] = a - b;
            }
            else if (strcmp(t.simbolo, "*") == 0) {
                b = pila[tope--]; a = pila[tope--];
                pila[++tope] = a * b;
            }
            else if (strcmp(t.simbolo, "/") == 0) {
                b = pila[tope--]; a = pila[tope--];
                pila[++tope] = a / b;
            }
            else if (strcmp(t.simbolo, "^") == 0) {
                b = pila[tope--]; a = pila[tope--];
                pila[++tope] = pow(a, b);
            }
            else if (strcmp(t.simbolo, "sin") == 0) {
                a = pila[tope--];
                pila[++tope] = sin(a);
            }
            else if (strcmp(t.simbolo, "cos") == 0) {
                a = pila[tope--];
                pila[++tope] = cos(a);
            }
            else if (strcmp(t.simbolo, "exp") == 0) {
                a = pila[tope--];
                pila[++tope] = exp(a);
            }
            else if (strcmp(t.simbolo, "log") == 0) {
                a = pila[tope--];
                pila[++tope] = log(a);
            }
            else if (strcmp(t.simbolo, "sqrt") == 0) {
                a = pila[tope--];
                pila[++tope] = sqrt(a);
            }
        }
    }

    return pila[tope];
}

//-------------------------------
// Derivadas parciales
//-------------------------------
double derivada_parcial_x(const char* expr, double x, double y, double h) {
    int num_tokens;
    Token* tokens = tokenizar(expr, &num_tokens);
    double f_xy = evaluar(tokens, num_tokens, x, y);
    double f_xhy = evaluar(tokens, num_tokens, x + h, y);
    free(tokens);
    return (f_xhy - f_xy) / h;
}

double derivada_parcial_y(const char* expr, double x, double y, double h) {
    int num_tokens;
    Token* tokens = tokenizar(expr, &num_tokens);
    double f_xy = evaluar(tokens, num_tokens, x, y);
    double f_xyh = evaluar(tokens, num_tokens, x, y + h);
    free(tokens);
    return (f_xyh - f_xy) / h;
}

//-------------------------------
// Programa principal
//-------------------------------
int main() {
    char expr[100];
    double x, y, h = 0.0001;
    int num_tokens;  // Variable necesaria para tokenizar

    printf("===========================================\n");
    printf("  CALCULADORA DE DERIVADAS PARCIALES EN C  \n");
    printf("===========================================\n\n");
    printf("Instrucciones:\n");
    printf("- Usa 'x' e 'y' como variables.\n");
    printf("- Operadores: +, -, *, /, ^ (potencia).\n");
    printf("- Funciones: sin, cos, exp, log, sqrt.\n");
    printf("- Ejemplo: sqrt(x^2 + y^2)\n\n");

    printf("Ingresa la función f(x, y): ");
    fgets(expr, sizeof(expr), stdin);
    expr[strcspn(expr, "\n")] = '\0';  // Elimina el salto de línea

    printf("Ingresa el punto (x y): ");
    scanf("%lf %lf", &x, &y);

    // Tokenizar la expresión una sola vez
    Token* tokens = tokenizar(expr, &num_tokens);

    printf("\n📊 Resultados:\n");
    printf("f(%.2lf, %.2lf) = %lf\n", x, y, evaluar(tokens, num_tokens, x, y));
    printf("∂f/∂x = %lf\n", derivada_parcial_x(expr, x, y, h));
    printf("∂f/∂y = %lf\n", derivada_parcial_y(expr, x, y, h));

    free(tokens);
    return 0;
}
