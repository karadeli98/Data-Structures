#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct Passenger {
	char flightName[20];
	char name[20];
	char wantSeatType[20];
	char hadSeatType[20];
	bool priority;
	bool haveTicket;

} Passenger;
typedef struct Queue {
	char name[20];
	int front, rear;
	Passenger *passenger;

} Queue;

typedef struct Stack {
	int top;
} Stack;

typedef struct Flight {
	char name[20];
	bool close;
	int business;
	int economy;
	int standard;
	Queue *businessQueue;
	Queue *economyQueue;
	Queue *standardQueue;
	Stack *businessSeat;
	Stack *economySeat;
	Stack *standardSeat;

} Flight;
typedef struct NodeFlight {
	Flight *f;
	struct NodeFlight *next;
} NodeFlight, *NodeFlight1;
typedef struct NodePassenger {
	Passenger *p;
	struct NodePassenger *next;
} NodePassenger, *NodePassenger1;
void pushPassenger(struct NodePassenger** head, Passenger * p) {

	struct NodePassenger* new = malloc(sizeof(struct NodePassenger*));

	new->p = p;

	new->next = (*head);

	(*head) = new;
}
void pushFlight(struct NodeFlight** head, Flight * f) {

	struct NodeFlight* new = malloc(sizeof(struct NodeFlight*));

	new->f = f;

	new->next = (*head);

	(*head) = new;
}
Queue* createQueue() {
	Queue* queue = (Queue*) malloc(sizeof(Queue));
	queue->passenger = malloc(sizeof(Passenger));
	queue->front = 0;
	queue->rear = 0;
	return queue;
}
Stack* createStack() {

	Stack* stack = (Stack*) malloc(sizeof(Stack));
	stack->top = -1;
	return stack;
}
void push(Stack *s) {

	s->top++;

}
void pop(Stack *s) {

	s->top--;
}

Passenger *findPassenger(NodePassenger *head, char *name) {
	Passenger *p;
	while (head != NULL) {
		if (strcmp(head->p->name, name) == 0) {
			p = head->p;
			return p;
		}
		head = head->next;
	}
	return NULL;
}

int travelQueue(Queue *q) {
	int i = 0;
	int priority = 0;
	for (i = q->front; i < q->rear; i++) {
		if (q->passenger[i].priority == 1) {
			priority++;
		}
	}
	return priority;
}

void enqueue(Queue *q, Passenger *p, FILE *output) {

	int priority = travelQueue(q);
	q->passenger = realloc(q->passenger, sizeof(Passenger) * (q->rear + 1));

	if (p->priority == 0) {

		strcpy(q->name, p->wantSeatType);
		strcpy(q->passenger[q->rear].name, p->name);
		strcpy(q->passenger[q->rear].flightName, p->flightName);
		strcpy(q->passenger[q->rear].wantSeatType, p->wantSeatType);
		q->passenger[q->rear].priority = p->priority;
		q->passenger[q->rear].haveTicket = 0;
		q->rear += 1;

	} else {
		int i = 0;
		for (i = q->rear; i > priority + q->front; i--) {

			strcpy(q->name, p->wantSeatType);
			strcpy(q->passenger[i].name, q->passenger[i - 1].name);
			strcpy(q->passenger[i].flightName, q->passenger[i - 1].flightName);
			strcpy(q->passenger[i].wantSeatType,
					q->passenger[i - 1].wantSeatType);
			q->passenger[i].priority = q->passenger[i - 1].priority;
			q->passenger[i].haveTicket = q->passenger[i - 1].haveTicket;

		}
		q->rear += 1;
		strcpy(q->name, p->wantSeatType);
		strcpy(q->passenger[priority + q->front].name, p->name);
		strcpy(q->passenger[priority + q->front].flightName, p->flightName);
		strcpy(q->passenger[priority + q->front].wantSeatType, p->wantSeatType);
		q->passenger[priority + q->front].priority = p->priority;
		q->passenger[priority + q->front].haveTicket = 0;

	}
	fprintf(output, "queue %s %s %s %d\n", p->flightName, p->name,
			p->wantSeatType, q->rear - q->front);

}

void dequeue(Queue *q, NodePassenger *head, char *name) {

	Passenger *p = findPassenger(head, name);

	strcpy(p->hadSeatType, q->name);
	q->passenger[q->front].haveTicket = 1;
	p->haveTicket = 1;
	q->front++;

}

void transferDequeue(Queue *q) {
	q->front++;
}

void transferenqueue(Queue *q) {
	q->rear++;
}

Flight * findFlight(NodeFlight *head, char *name) {
	Flight *f;
	while (head != NULL) {
		if (strcmp(name, head->f->name) == 0) {
			f = (head->f);
			return f;

		}
		head = head->next;

	}
	return NULL;
}

int totalPassenger(Queue *q) {
	int total = 0;
	int i = 0;
	for (i = 0; i < q->rear; i++) {
		if (q->passenger[i].haveTicket == 1) {
			total++;
		}
	}
	return total;
}

void transfer(Queue *source, Queue *target, Stack *standart) {
	int i = 0;

	target->passenger = realloc(target->passenger,
			sizeof(Passenger) * (target->rear + source->rear - source->front));
	int k = source->rear - source->front;
	if (standart->top + 1 < k) {
		k = standart->top + 1;
	}
	for (i = 0; i < k; i++) {

		strcpy(target->passenger[target->rear].name,
				source->passenger[source->front].name);
		strcpy(target->passenger[target->rear].flightName,
				source->passenger[source->front].flightName);
		strcpy(target->passenger[target->rear].hadSeatType,
				source->passenger[source->front].hadSeatType);
		strcpy(target->passenger[target->rear].wantSeatType,
				source->passenger[source->front].wantSeatType);
		target->passenger[target->rear].haveTicket =
				source->passenger[source->front].haveTicket;
		target->passenger[target->rear].priority =
				source->passenger[source->front].priority;
		transferenqueue(target);
		transferDequeue(source);

	}

}

void sell(NodeFlight *head, char *name, NodePassenger *headPassenger,
		FILE *output) {
	Flight *f = findFlight(head, name);
	if (f == NULL || f->close == 1) {
		fprintf(output, "error\n");
	} else {
		int i = 0;
		int bigBusiness = (f->businessQueue->rear) - (f->businessQueue->front);
		if (f->businessSeat->top + 1 < bigBusiness) {
			bigBusiness = f->businessSeat->top + 1;
		}

		for (i = 0; i < bigBusiness; i++) {
			int index = f->businessQueue->front;
			pop(f->businessSeat);
			dequeue(f->businessQueue, headPassenger,
					f->businessQueue->passenger[index].name);
		}

		int bigEco = (f->economyQueue->rear) - (f->economyQueue->front);
		if (f->economySeat->top + 1 < bigEco) {
			bigEco = f->economySeat->top + 1;
		}
		for (i = 0; i < bigEco; i++) {
			int index = f->economyQueue->front;
			pop(f->economySeat);
			dequeue(f->economyQueue, headPassenger,
					f->economyQueue->passenger[index].name);
		}

		int bigStd = (f->standardQueue->rear) - (f->standardQueue->front);
		if (f->standardSeat->top + 1 < bigStd) {
			bigStd = f->standardSeat->top + 1;
		}

		for (i = 0; i < bigStd; i++) {
			int index = f->standardQueue->front;
			pop(f->standardSeat);
			dequeue(f->standardQueue, headPassenger,
					f->standardQueue->passenger[index].name);
		}
		transfer(f->businessQueue, f->standardQueue, f->standardSeat);
		transfer(f->economyQueue, f->standardQueue, f->standardSeat);

		bigStd = (f->standardQueue->rear) - (f->standardQueue->front);
		if (f->standardSeat->top + 1 < bigStd) {
			bigStd = f->standardSeat->top + 1;
		}

		for (i = 0; i < bigStd; i++) {
			int index = f->standardQueue->front;
			pop(f->standardSeat);
			dequeue(f->standardQueue, headPassenger,
					f->standardQueue->passenger[index].name);
		}
		int business = totalPassenger(f->businessQueue);
		int economy = totalPassenger(f->economyQueue);
		int standard = totalPassenger(f->standardQueue);
		fprintf(output, "sold %s %d %d %d\n", f->name, business, economy,
				standard);
	}

}

void createPassenger(char *flightName, char *name, char *wantSeatType,
		char *hadSeatType,
		bool priority, NodePassenger **headPassenger, NodeFlight *headFlight,
		FILE *output) {
	Passenger *p = malloc(sizeof(Passenger));
	Passenger *p1 = findPassenger(*headPassenger, name);
	if (p1 != NULL) {
		fprintf(output, "error");
	}
	strcpy(p->flightName, flightName);
	strcpy(p->name, name);
	strcpy(p->wantSeatType, wantSeatType);
	strcpy(p->hadSeatType, hadSeatType);
	p->priority = priority;
	p->haveTicket = 0;

	Flight *f = findFlight(headFlight, flightName);
	if (f == NULL) {
		fprintf(output, "error\n");
	} else {
		pushPassenger(headPassenger, p);
		if (strcmp(wantSeatType, "business") == 0) {
			enqueue(f->businessQueue, p, output);
		} else if (strcmp(wantSeatType, "economy") == 0) {
			enqueue(f->economyQueue, p, output);
		} else if (strcmp(wantSeatType, "standard") == 0) {
			enqueue(f->standardQueue, p, output);
		} else {
			fprintf(output, "error\n");
		}
	}
}

int existFlight(NodeFlight *head, char *name) {
	int flag = 0;
	while (head != NULL) {
		if (strcmp(head->f->name, name) == 0) {
			flag = 1;
		}
		head = head->next;
	}
	return flag;
}

void createFlight(char *name, int seat, char *type, NodeFlight **head,
		FILE *output) {

	int index = existFlight(*head, name);
	if (index == 0) {
		Flight *f = malloc(sizeof(Flight));
		f->businessQueue = createQueue();
		f->economyQueue = createQueue();
		f->standardQueue = createQueue();
		f->businessSeat = createStack();
		f->economySeat = createStack();
		f->standardSeat = createStack();
		f->business = 0;
		f->economy = 0;
		f->standard = 0;
		f->close = 0;
		int i = 0;
		strcpy(f->name, name);

		if (strcmp(type, "business") == 0) {
			f->business = seat;
			for (i = 0; i < seat; i++) {
				push(f->businessSeat);
			}
			pushFlight(head, f);
			fprintf(output, "addseats %s %d %d %d\n", name, f->business,
					f->economy, f->standard);
		} else if (strcmp(type, "economy") == 0) {
			f->economy = seat;
			for (i = 0; i < seat; i++) {
				push(f->economySeat);
			}
			pushFlight(head, f);
			fprintf(output, "addseats %s %d %d %d\n", name, f->business,
					f->economy, f->standard);
		} else if (strcmp(type, "standard") == 0) {
			f->standard = seat;
			for (i = 0; i < seat; i++) {
				push(f->standardSeat);
			}
			pushFlight(head, f);
			fprintf(output, "addseats %s %d %d %d\n", name, f->business,
					f->economy, f->standard);
		} else {
			fprintf(output, "error\n");
		}

	} else {
		Flight *f = findFlight(*head, name);
		if (f->close == 1) {
			fprintf(output, "error\n");
		} else {
			int i = 0;
			if (strcmp(type, "business") == 0) {
				f->business += seat;
				for (i = 0; i < seat; i++) {
					push(f->businessSeat);
				}
				fprintf(output, "addseats %s %d %d %d\n", name, f->business,
						f->economy, f->standard);
			} else if (strcmp(type, "economy") == 0) {
				f->economy += seat;
				for (i = 0; i < seat; i++) {
					push(f->economySeat);
				}
				fprintf(output, "addseats %s %d %d %d\n", name, f->business,
						f->economy, f->standard);
			} else if (strcmp(type, "standard") == 0) {
				f->standard += seat;
				for (i = 0; i < seat; i++) {
					push(f->standardSeat);
				}
				fprintf(output, "addseats %s %d %d %d\n", name, f->business,
						f->economy, f->standard);
			} else {
				fprintf(output, "error\n");
			}

		}
	}

}

void getWords(char *base, int line, int column, char target[line][column]) {

	int n = 0, i, j = 0;
	int TRUE = 1;
	for (i = 0; TRUE; i++) {
		if (base[i] == '\n' || base[i] == '\0') {
			target[n][j++] = '\0';
			break;
		} else if (base[i] != ' ') {
			target[n][j++] = base[i];
		} else {
			target[n][j++] = '\0';
			n++;
			j = 0;
		}
	}

}

void report(char *name, NodeFlight *headFlight, FILE *output) {

	Flight *f = findFlight(headFlight, name);
	if (f == NULL) {
		fprintf(output, "error\n");
	} else {
		fprintf(output, "report %s\n", name);
		int countBusiness = 0;
		int countEconomy = 0;
		int countStandard = 0;

		int i = 0;
		for (i = 0; i < f->businessQueue->rear; i++) {
			if (f->businessQueue->passenger[i].haveTicket == 1) {
				countBusiness++;
			}
		}
		for (i = 0; i < f->economyQueue->rear; i++) {

			if (f->economyQueue->passenger[i].haveTicket == 1) {
				countEconomy++;

			}

		}

		for (i = 0; i < f->standardQueue->rear; i++) {
			if (f->standardQueue->passenger[i].haveTicket == 1) {
				countStandard++;

			}

		}

		fprintf(output, "business %d\n", countBusiness);
		for (i = 0; i < f->businessQueue->rear; i++) {
			if (f->businessQueue->passenger[i].haveTicket == 1) {
				fprintf(output, "%s\n", f->businessQueue->passenger[i].name);

			}

		}
		fprintf(output, "economy %d\n", countEconomy);
		for (i = 0; i < f->economyQueue->rear; i++) {

			if (f->economyQueue->passenger[i].haveTicket == 1) {
				fprintf(output, "%s\n", f->economyQueue->passenger[i].name);

			}

		}

		fprintf(output, "standard %d\n", countStandard);
		for (i = 0; i < f->standardQueue->rear; i++) {
			if (f->standardQueue->passenger[i].haveTicket == 1) {
				fprintf(output, "%s\n", f->standardQueue->passenger[i].name);

			}

		}
		fprintf(output, "end of report %s\n", name);
	}

}

void info(NodePassenger *head, char *name, FILE *output) {

	Passenger *p = findPassenger(head, name);

	if (p == NULL) {
		fprintf(output, "error\n");
	} else {
		if (strlen(p->hadSeatType) > 1) {
			fprintf(output, "info %s %s %s %s\n", p->name, p->flightName,
					p->wantSeatType, p->hadSeatType);
		} else {
			fprintf(output, "info %s %s %s none\n", p->name, p->flightName,
					p->wantSeatType);
		}
	}

}

void close(NodeFlight *head, char *name, FILE *output,
		NodePassenger *headPassenger) {

	Flight *f = findFlight(head, name);
	if (f == NULL || f->close == 1) {
		fprintf(output, "error\n");
	} else {
		int haveTicket = 0;
		int waiting = 0;
		int i = 0;

		for (i = 0; i < f->businessQueue->rear; i++) {
			if (f->businessQueue->passenger[i].haveTicket == 1) {
				haveTicket++;

			}
		}

		for (i = 0; i < f->economyQueue->rear; i++) {
			if (f->economyQueue->passenger[i].haveTicket == 1) {
				haveTicket++;

			}
		}
		for (i = 0; i < f->standardQueue->rear; i++) {
			if (f->standardQueue->passenger[i].haveTicket == 1) {
				haveTicket++;

			}
		}

		while (headPassenger != NULL) {
			if (strcmp(headPassenger->p->flightName, name) == 0
					&& headPassenger->p->haveTicket == 0) {
				waiting++;
			}
			headPassenger = headPassenger->next;
		}
		fprintf(output, "closed %s %d %d\n", name, haveTicket, waiting);
		if (waiting > 0) {

			for (i = f->businessQueue->front; i < f->businessQueue->rear; i++) {
				if (f->businessQueue->passenger[i].haveTicket == 0) {
					fprintf(output, "waiting %s\n",
							f->businessQueue->passenger[i].name);

				}
			}

			for (i = f->economyQueue->front; i < f->economyQueue->rear; i++) {
				if (f->economyQueue->passenger[i].haveTicket == 0) {
					fprintf(output, "waiting %s\n",
							f->economyQueue->passenger[i].name);

				}
			}
			for (i = 0; i < f->standardQueue->rear; i++) {
				if (f->standardQueue->passenger[i].haveTicket == 0) {
					fprintf(output, "waiting %s\n",
							f->standardQueue->passenger[i].name);

				}
			}
		}
		f->close = 1;
	}
}

char *readline(FILE *fp, char **buffer) {
	int ch;
	size_t buflen = 0, nchar = 64;

	*buffer = malloc(nchar);

	while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
		(*buffer)[buflen++] = ch;

		if (buflen + 1 >= nchar) {
			char *tmp = realloc(*buffer, nchar * 2);
			*buffer = tmp;
			nchar *= 2;
		}
	}
	(*buffer)[buflen] = 0;

	if (buflen == 0 && ch == EOF) {
		free(*buffer);
		*buffer = NULL;
	}

	return *buffer;
}

int space(char *line) {
	int len = strlen(line);
	int i, total = 0;
	for (i = 0; i < len; i++) {
		if (*line == ' ') {
			total++;
		}
		line++;
	}
	return total;
}

int length(char *line) {
	int total = 0;
	while (*line != '\0') {
		line++;
		total++;
	}
	return total;
}
int validInput(char *line) {
	int i = 0;
	for (i = 0; i < strlen(line); i++) {
		if (isdigit(*line) == 0) {
			return -1;
		}
		line++;
	}
	return 0;
}
int main(int argc, char *argv[]) {
	FILE *input;
	FILE *output;
	char * line = NULL;
	NodeFlight *headFlight = NULL;
	NodePassenger *headPassenger = NULL;
	input = fopen(argv[1], "r");
	output = fopen(argv[2], "w");
	while (readline(input, &line)) {
		int x = space(line);
		int y = length(line);
		char arr[x + 1][y];
		getWords(line, x + 1, y, arr);
		if (strcmp(arr[0], "addseat") == 0) {

			if (x != 3 || validInput(arr[3]) == -1) {
				fprintf(output, "error\n");
			} else {
				char *name = arr[1];
				char *seatType = arr[2];
				int numberOfSeats = atoi(arr[3]);
				createFlight(name, numberOfSeats, seatType, &headFlight,
						output);
			}

		} else if (strcmp(arr[0], "enqueue") == 0) {
			char *flight = arr[1];
			char *seatType = arr[2];
			char *name = arr[3];
			char *type = "";
			if (x == 4) {
				type = arr[4];
			}

			int priority = 0;
			if (strcmp(type, "diplomat") == 0 || strcmp(type, "veteran") == 0) {
				priority = 1;
			}
			if ((((strcmp(seatType, "economy") == 0
					&& strcmp(type, "veteran") != 0)
					|| (strcmp(seatType, "business") == 0
							&& strcmp(type, "diplomat") != 0)) && x == 4)
					|| (strcmp(seatType, "standard") == 0 && x != 3)) {
				fprintf(output, "error\n");
			} else {
				createPassenger(flight, name, seatType, " ", priority,
						&headPassenger, headFlight, output);

			}
		} else if (strcmp(arr[0], "sell") == 0) {
			char *name = arr[1];
			if (x != 1) {
				fprintf(output, "error\n");
			} else {
				sell(headFlight, name, headPassenger, output);
			}
		} else if (strcmp(arr[0], "report") == 0) {
			char *name = arr[1];
			if (x != 1) {
				fprintf(output, "error\n");
			} else {
				report(name, headFlight, output);
			}

		} else if (strcmp(arr[0], "close") == 0) {
			if (x != 1) {
				fprintf(output, "error\n");
			} else {
				char *flight = arr[1];
				close(headFlight, flight, output, headPassenger);

			}

		} else if (strcmp(arr[0], "info") == 0) {
			if (x != 1) {
				fprintf(output, "error\n");
			} else {
				char *name = arr[1];
				info(headPassenger, name, output);
			}

		} else {
			fprintf(output, "error\n");
		}
		free(line);
		line = NULL;
	}

	fclose(input);
	fclose(output);

}
