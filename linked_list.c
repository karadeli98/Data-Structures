#include <stdio.h>
#include <stdlib.h>

typedef struct Chromosome {
	int gene;
	int rank;

	struct Chromosome *next;
} Chromosome;

typedef struct Population {
	Chromosome *chromosome;
	int fitness;
	float rank;
	struct Population *next;
} Population;

typedef struct BestPop {
	int fitness;
	char *chromosome;
} BestPop;

void pushChromosome(struct Chromosome** head_ref, int gene, int rank) {

	Chromosome* new_node = (Chromosome*) malloc(sizeof(Chromosome));

	struct Chromosome *last = *head_ref;

	new_node->gene = gene;
	new_node->rank = rank;

	new_node->next = NULL;

	if (*head_ref == NULL) {
		*head_ref = new_node;
		return;
	}

	while (last->next != NULL)		//Iterate until the end of linked list
		last = last->next;

	last->next = new_node;		//Equalize end of the linked list and new node
	return;
}

void pushPopulation(struct Population** head_ref, Chromosome *c) {
	struct Population* new_node = (struct Population*) malloc(
			sizeof(struct Population));

	struct Population *last = *head_ref;

	new_node->chromosome = c;
	new_node->fitness = 0;
	new_node->rank = 0;

	new_node->next = NULL;

	if (*head_ref == NULL) {
		*head_ref = new_node;
		return;
	}

	while (last->next != NULL)
		last = last->next;

	last->next = new_node;
	return;
}

int bestFitness(Population *head) {
	int best = head->fitness;
	while (head != NULL) {
		if (head->fitness < best) {
			best = head->fitness;
		}
		head = head->next;
	}
	return best;
}

int worstFitness(Population *head) {
	int worst = head->fitness;
	while (head != NULL) {
		if (head->fitness > worst) {
			worst = head->fitness;
		}
		head = head->next;
	}
	return worst;

}

int power(int number1, int number2) {
	int total = 1;				//Working like pow() function in c
	int i;

	for (i = 0; i < number2; i++) {			//While i<number2
											// total = number1 * total
		total *= number1;
	}
	return total;
}

void calculateFitness(Population *head) {

	while (head != NULL) {
		head->fitness = 0;
		Chromosome *headChromosome = head->chromosome;
		while (headChromosome != NULL) {
			head->fitness += power(2, headChromosome->rank)
					* headChromosome->gene;

			headChromosome = headChromosome->next;

		}
		head = head->next;
	}

}

void printPopulation(Population *head, int best, int worst) {

	while (head != NULL) {
		head->rank = (head->fitness - best) / (worst - best);
		Chromosome *headChromosome = head->chromosome;
		while (headChromosome != NULL) {
			printf("%d:", headChromosome->gene);
			headChromosome = headChromosome->next;
		}
		printf(" -> %d", head->fitness);
		printf("\n");
		head = head->next;
	}

}

void swap(struct Population *source, struct Population *target) {

	int fitness = source->fitness;
	int rank = source->rank;				//Store the datas
	Chromosome *c = source->chromosome;

	source->fitness = target->fitness;
	source->rank = target->rank;			//Change data target->source
	source->chromosome = target->chromosome;

	target->rank = rank;
	target->fitness = fitness;				//Insert store datas to target
	target->chromosome = c;
}

void sortLinkedList(Population *head) {
	int swapped;
	Population *ptr1;
	Population *lptr = NULL;

	if (head == NULL)
		return;

	do {
		swapped = 0;
		ptr1 = head;

		while (ptr1->next != lptr) {
			if (ptr1->fitness > ptr1->next->fitness) {
				swap(ptr1, ptr1->next);
				swapped = 1;
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	} while (swapped);
}

void mutate(Population *head, int index) {//Change value of each chromosome on index
	while (head != NULL) {						//which taken by parameter
		int i = 0;
		Chromosome *headChromosome = head->chromosome;
		for (i = 0; i < index - 1; i++) {			//Iterate until the index
			headChromosome = headChromosome->next;
		}
		if (headChromosome->gene == 0) {			//If gene is 0,change by 1
			headChromosome->gene = 1;
		} else {
			headChromosome->gene = 0;					//Else change by 0
		}
		head = head->next;
	}
}

Population *findPopulation(Population *head, int rank) {
	//Find chromosome accarding to line
	Population* population;

	int i = 0;
	//Iterate while i = rank -1
	for (i = 0; i < rank - 1; i++) {
		head = head->next;
	}
	population = head;
	return population;
}

void xover(Population *chromosome1, Population *chromosome2, int start, int end) {

	int arr[end - start];
	int i;
	Chromosome *c1 = chromosome1->chromosome;
	for (i = 0; i < start - 1; i++) {
		c1 = c1->next;
	}
	for (i = 0; i < end - start + 1; i++) {
		arr[i] = c1->gene;
		c1 = c1->next;
	}
	//Change chromosome values start to end between
	c1 = chromosome1->chromosome;				//chromosome1 and chromosome2
	Chromosome *c2 = chromosome2->chromosome;

	for (i = 0; i < start - 1; i++) {
		c1 = c1->next;
		c2 = c2->next;
	}
	for (i = 0; i < end - start + 1; i++) {
		c1->gene = c2->gene;
		c1 = c1->next;
		c2 = c2->next;
	}
	c2 = chromosome2->chromosome;
	for (i = 0; i < start - 1; i++) {
		c2 = c2->next;
	}
	for (i = 0; i < end - start + 1; i++) {
		c2->gene = arr[i];
		c2 = c2->next;
	}

}

void selection(Population *head, int chr1, int chr2, int start, int end) {

	Population *chromosome1 = findPopulation(head, chr1);//Return first chromosome according to line chr1
	Population *chromosome2 = findPopulation(head, chr2);//Return second chromosome according to line chr2
	xover(chromosome1, chromosome2, start, end);

}

void freePop(Population *head) {

	while (head != NULL) {
		Chromosome *ch = head->chromosome;
		while (ch != NULL) {
			free(ch);
			ch = ch->next;
		}
		free(head);
		head = head->next;
	}
}

void equalize(char *best, Population *head, int probSize) {
	int i = 0;
	Chromosome *ch1 = head->chromosome;
	for (i = 0; i < probSize - 1; i++) {
		char gene = ch1->gene + '0';
		best[i * 2] = gene;
		best[(2 * i) + 1] = ':';
		ch1 = ch1->next;
	}
	best[(probSize - 1) * 2] = ch1->gene + '0';
}

int main(int argc, char *argv[]) {
	int PROB_SIZE = atoi(argv[1]);
	int POP_SIZE = atoi(argv[2]);
	int MAX_GEN = atoi(argv[3]);

	FILE *population;
	FILE *mutateFile;
	FILE *selectionFile;
	FILE *xoverFile;

	int i, a, j;
	population = fopen("population", "r");			//
	mutateFile = fopen("mutate", "r");				//		Open files
	selectionFile = fopen("selection", "r");		//
	xoverFile = fopen("xover", "r");				//
	Population *headPop = NULL;
	BestPop *bestchromosome = malloc(sizeof(BestPop));
	bestchromosome->chromosome = malloc(sizeof(char) * PROB_SIZE * 2);

	for (i = 0; i < POP_SIZE; i++) {
		Chromosome *head = NULL;
		for (j = 0; j < PROB_SIZE; j++) {
			fscanf(population, "%d:", &a);
			pushChromosome(&head, a, PROB_SIZE - j - 1);

		}
		pushPopulation(&headPop, head);

	}

	int mut, x1, x2, s1, s2;
	printf("GENERATION: 0\n");
	calculateFitness(headPop);				//Calculate fitness
	int best = bestFitness(headPop);	//Find smallest fitness of population
	int worst = worstFitness(headPop);	//Find greatest fitness of population
	sortLinkedList(headPop);		//Sort linked list according to fitness
	printPopulation(headPop, best, worst);		//Print all gene

	bestchromosome->fitness = headPop->fitness;
	equalize (bestchromosome->chromosome, headPop, PROB_SIZE);
	printf("Best chromosome found so far: %s -> %d \n",
			bestchromosome->chromosome, bestchromosome->fitness);

	for (i = 1; i < MAX_GEN + 1; i++) {
		printf("GENERATION: %d\n", i);
		fscanf(mutateFile, "%d\n", &mut);
		fscanf(xoverFile, "%d:", &x1);
		fscanf(xoverFile, "%d", &x2);
		for (j = 0; j < POP_SIZE / 2; j++) {
			fscanf(selectionFile, "%d:", &s1);
			fscanf(selectionFile, "%d", &s2);
			selection(headPop, s1, s2, x1, x2);
		}
		mutate(headPop, mut);
		calculateFitness(headPop);
		int best = bestFitness(headPop);
		int worst = worstFitness(headPop);
		sortLinkedList(headPop);
		printPopulation(headPop, best, worst);
		if (headPop->fitness < bestchromosome->fitness) {
			bestchromosome->fitness = headPop->fitness;
			equalize(bestchromosome->chromosome, headPop, PROB_SIZE);

		}
		printf("Best chromosome found so far: %s -> %d \n",
				bestchromosome->chromosome, bestchromosome->fitness);
	}
	free(bestchromosome->chromosome);
	free(bestchromosome);
	free(selectionFile);
	free(mutateFile);
	free(xoverFile);
	free(population);
	freePop(headPop);

}
