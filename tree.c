#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

typedef struct {

} Terminal;

typedef struct Cond {

	int type;

} Cond;

typedef struct Expr {

	int type;

} Expr;

int lineNumber(char *filename) {				//Find the number of line of file which given as a parameter
	FILE* file = fopen(filename, "r");			//Open file
	char line[256];
	int i = 0;
	while (fgets(line, sizeof(line), file)) {	//While line is not equal eof, increase i
		i++;
	}

	fclose(file);									//Close file
	return i;
}

char *readFile(char *filename) {				//Return randomly char of file which given as a parameter
	int mod = lineNumber(filename);
	FILE* file = fopen(filename, "r");				//Open file
	char line[256];
	int i = 0;
	char *symbol = malloc(sizeof(char) * 10);
	int random = rand();
	while (fgets(line, sizeof(line), file)) {

		if (i == random % mod) {					//random % mod gives number of between 0 and number of line to us
			strcpy(symbol, line);					//and i and this number are equal symbol = line
		}
		i++;
	}
	int len = strlen(symbol);
	if (symbol[len - 1] == '\n') {
		symbol[len - 1] = '\0';
	}
	fclose(file);
	return symbol;

}

Terminal *createTerminal() {
	Terminal *terminal = malloc(sizeof(Terminal));

	return terminal;
}

Cond *createCond() {
	Cond *cond = malloc(sizeof(Cond));
	return cond;
}

Expr *createExpr() {
	Expr *expr = malloc(sizeof(Expr));
	return expr;
}
////////////////////////////////// For create tree we need 4 different nodes type
typedef struct Node {
	char *term;
} Node;

typedef struct Node1 {
	char *node;
	void *node1;
} Node1;

typedef struct Node2 {
	char *node;
	void *node1;
	void *node2;

} Node2;

typedef struct Node3 {
	char *node;
	void *node1;
	void *node2;
	void *node3;
} Node3;

/////////////////////////////////End of the create nodes type

void insertTree(void *new_data, char *name) { 		//Insert to Tree using cond as a root

	int random = rand();
	if (strcmp(name, "<cond>") == 0) {
		Cond *mainCond;
		mainCond = new_data;
		if (random % 2 == 0) {
			((Cond*) mainCond)->type = 0;
			Cond *cond = createCond();
			Cond *cond1 = createCond();
			Terminal *set = createTerminal();
			((Node3*) mainCond)->node1 = cond;
			((Node3*) mainCond)->node2 = set;
			((Node3*) mainCond)->node3 = cond1;
			insertTree(cond, "<cond>");
			insertTree(set, "<set_op>");
			insertTree(cond1, "<cond>");

		} else {
			((Cond*) mainCond)->type = 1;
			Expr *expr = createExpr();
			Expr *expr1 = createExpr();
			Terminal *rel = createTerminal();
			((Node3*) mainCond)->node1 = expr;
			((Node3*) mainCond)->node2 = rel;
			((Node3*) mainCond)->node3 = expr1;
			insertTree(expr, "<expr>");
			insertTree(rel, "<rel_op>");
			insertTree(expr1, "<expr>");
		}
	} else if (strcmp(name, "<expr>") == 0) {
		Expr *mainExpr;
		mainExpr = new_data;
		if (random % 3 == 0) {
			((Expr*) mainExpr)->type = 0;
			Expr *expr = createExpr();
			Expr *expr1 = createExpr();
			Terminal *op = createTerminal();
			((Node3*) mainExpr)->node1 = expr;
			((Node3*) mainExpr)->node2 = op;
			((Node3*) mainExpr)->node3 = expr1;
			insertTree(expr, "<expr>");
			insertTree(op, "<op>");
			insertTree(expr1, "<expr>");

		} else if (random % 3 == 1) {
			((Expr*) mainExpr)->type = 1;
			Expr *expr = createExpr();
			Terminal *pre = createTerminal();
			((Node2*) mainExpr)->node1 = pre;
			((Node2*) mainExpr)->node2 = expr;
			insertTree(pre, "<pre_op>");
			insertTree(expr, "<expr>");

		} else {
			((Expr*) mainExpr)->type = 2;
			Terminal *var = createTerminal();
			((Node1*) mainExpr)->node1 = var;
			insertTree(var, "<var>");
		}

	} else if (strcmp(name, "<op>") == 0) {
		Terminal *op;
		op = new_data;
		Node *node = malloc(sizeof(Node));
		node->term = malloc(sizeof(char) * 10);
		strcpy(node->term, readFile("op"));
		((Node1*) op)->node1 = node;

	} else if (strcmp(name, "<pre_op>") == 0) {
		Terminal *pre;
		pre = new_data;
		Node *node = malloc(sizeof(Node));
		node->term = malloc(sizeof(char) * 10);
		strcpy(node->term, readFile("pre_op"));
		((Node1*) pre)->node1 = node;

	} else if (strcmp(name, "<rel_op>") == 0) {
		Terminal *rel;
		rel = new_data;
		Node *node = malloc(sizeof(Node));
		node->term = malloc(sizeof(char) * 10);
		strcpy(node->term, readFile("rel_op"));
		((Node1*) rel)->node1 = node;

	} else if (strcmp(name, "<set_op>") == 0) {
		Terminal *set;
		set = new_data;
		Node *node = malloc(sizeof(Node));
		node->term = malloc(sizeof(char) * 10);
		strcpy(node->term, readFile("set_op"));
		((Node1*) set)->node1 = node;

	} else if (strcmp(name, "<var>") == 0) {
		Terminal *var;
		var = new_data;
		Node *node = malloc(sizeof(Node));
		node->term = malloc(sizeof(char) * 10);
		strcpy(node->term, readFile("var"));
		((Node1*) var)->node1 = node;

	}
}

void printTree(void *ptr, char *name) {				//Print Tree

	if (strcmp(name, "<cond>") == 0) {
		if (((Cond*) ptr)->type == 0) {
			printf("(");
			printTree(((Node3*) ptr)->node1, "<cond>");
			printTree(((Node3*) ptr)->node2, "<set>");
			printTree(((Node3*) ptr)->node3, "<cond>");
			printf(")");
		} else {									//If ptr has 3 child function takes child1,child2,child3 recursively
			printf("(");
			printTree(((Node3*) ptr)->node1, "<expr>");
			printTree(((Node3*) ptr)->node2, "<rel>");
			printTree(((Node3*) ptr)->node3, "<expr>");
			printf(")");
		}

	} else if (strcmp(name, "<expr>") == 0) {
		if (((Expr*) ptr)->type == 0) {
			printf("(");
			printTree(((Node3*) ptr)->node1, "<expr>");
			printf(")");
			printTree(((Node3*) ptr)->node2, "<op>");
			printf("(");
			printTree(((Node3*) ptr)->node3, "<expr>");
			printf(")");

		} else if (((Expr*) ptr)->type == 1) {

			printTree(((Node2*) ptr)->node1, "<pre>");
			printf("(");								//If ptr has 2 child function takes child1,child2 recursively
			printTree(((Node2*) ptr)->node2, "<expr>");
			printf(")");
		} else {
			printTree(((Node1*) ptr)->node1, "<var>");
		}
	} else if (strcmp(name, "<pre>") == 0) {
		printTree(((Node1*) ptr)->node1, "Pre_Op");
	} else if (strcmp(name, "<set>") == 0) {
		printTree(((Node1*) ptr)->node1, "Set_Op");
	} else if (strcmp(name, "<op>") == 0) {				//If ptr has 1 child function takes child1
		printTree(((Node1*) ptr)->node1, "Op");
	} else if (strcmp(name, "<var>") == 0) {
		printTree(((Node1*) ptr)->node1, "Var");
	} else if (strcmp(name, "<rel>") == 0) {
		printTree(((Node1*) ptr)->node1, "Rel_Op");
	} else {
		printf("%s", ((Node *) ptr)->term);				//If ptr has no child print it's term
	}
}

int main() {
	srand(time(NULL));
	Cond *cond = createCond();
	printf("if");
	insertTree(cond, "<cond>");				//Call insert tree function using cond as a root
	printTree(cond, "<cond>");				//Call print tree function using cond as a root
	return 0;
}
