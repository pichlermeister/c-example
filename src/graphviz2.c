/*
 ============================================================================
 Name        : graphviz2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//not needed, as replaced by command2
struct command {
	char* action;
	char* args; //whole line
};

//specific command. arguments represent "cleaned" arguments
struct command2 {
	char* action;

	int argc; //real argument count (whitespaces already filtered..)
	char** argv; //list of real arguments
};

//specific command for add operations
struct AddCommand {
	int valid; //0=false, 1=true
	char* name1;
	char sex1; //f=female, m=male

	char* relation;

	char* name2;
	char sex2; //f=female, m=male
};

typedef struct _person Person;
struct _person {
	char* name;
	char sex;
	Person* mother;
	Person* father;
};

typedef struct _personvector PersonVector;

struct _personvector {
	int capacity;
	int size;
	Person* people;
};

/**
 * "re-formats" / joins names in add command
 * (a name can have whitespaces, those need to be "joined")
 *
 * this would be much simpler (and probably not even needed) if arguments where separated by doublequotes.. "
 */
char** joinAddArguments(int argc, char* argv[], int *numtokens) {
	if (argc < 5) {
		numtokens = 0;
		return NULL;
	}
	char** result; //<person1> <relation> <person2>
	result = calloc(5, sizeof(char*));

	/**
	 * name1,name2=<person incl. sex>. e.g. "mother [f]", or "Sirius Black Jr. [m]"
	 *
	 *
	 * find the relation at position 'x' (argv[x])
	 *
	 * name1 = join of all strings before argv[x]
	 * name2 = join of all strings after argv[x]
	 */
	int rel_idx = -1; //position of relation
	size_t name1_size = 0, name2_size = 0;
	for (int i = 0; i < argc; i++) {
		if ( //
		strcmp("mother", argv[i]) == 0 //
		|| strcmp("father", argv[i]) == 0 //
		|| strcmp("mgm", argv[i]) == 0 //
		|| strcmp("fgm", argv[i]) == 0 //
		|| strcmp("mgf", argv[i]) == 0 //
		|| strcmp("fgf", argv[i]) == 0 //
				) {
			rel_idx = i;
		}
	}

	if (rel_idx + 2 <= argc)
		*numtokens = 5; //as we have found something and we have more than 5 arguments, we parse everything for 5 arguments...

	for (int i = 0; i < rel_idx - 1; i++) {
		if (i > 0)
			name1_size += 1; //reserve for whitespace
		name1_size += strlen(argv[i]);
	}
	for (int i = rel_idx + 1; i < argc; i++) {
		if (i > 0)
			name2_size += 1; //reserve for whitespace
		name2_size += strlen(argv[i]);
	}

	//copy name1
	char* name1;
	name1 = malloc(name1_size * sizeof(char)); //allocate memory for name1
	for (int i = 0; i < rel_idx - 1; i++) {
		if (i > 0)
			strcat(name1, " "); //add whitespace
		strcat(name1, argv[i]);
	}
	result[0] = name1;
	result[1] = argv[rel_idx - 1];
	result[2] = argv[rel_idx];

	//copy name2
	char* name2;
	name2 = malloc(name2_size * sizeof(char)); //allocate memory for name2
	for (int i = (rel_idx + 1); i < argc - 1; i++) {
		if (i > (rel_idx + 1))
			strcat(name2, " "); //add whitespace
		strcat(name2, argv[i]);
	}
	result[3] = name2;
	result[4] = argv[argc - 1];

	return result;
}

/**
 * [f] -> f
 * [m] -> m
 */
char parseSex(char* token) {
	if (strstr(token, "[f]") != '\0')
		return 'f';
	else if (strstr(token, "[m]") != NULL)
		return 'm';
	return ' ';
}

//converts list of arguments to "typed" operation for easier access..
struct AddCommand parseAddCommand(int argc, char* argv[]) {
	struct AddCommand cmd;

	char** tokens;
	int numtokens;
	tokens = joinAddArguments(argc, argv, &numtokens);

	if (numtokens < 5) {
		puts("invalid command. at least 5 parameters required");
		cmd.valid = 0;
		return cmd;
	}

	cmd.name1 = tokens[0];
	cmd.sex1 = parseSex(tokens[1]);

	cmd.relation = tokens[2];

	cmd.name2 = tokens[3];
	cmd.sex2 = parseSex(tokens[4]);

	cmd.valid = 1;

	printf("name1=%s[sex1=%c] -(%s)-> name2=%s[sex2=%c]\n", cmd.name1, cmd.sex1,
			cmd.relation, cmd.name2, cmd.sex2);

	return cmd;
}

//read generic command from stdin
struct command readCommand() {
	struct command cmd;

	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	printf("esp> ");
	read = getline(&line, &len, stdin);

	printf("you entered: %s", line);

	int idx = strcspn(line, " \n");
	char* action = malloc(idx * sizeof(char));
	strncpy(action, line, idx);

	char* args = malloc(strlen(line) - idx);
	strcpy(args, &(line[idx + 1]));

	printf("command is %s\n", action);
	cmd.action = action;
	cmd.args = args;

	puts("");

	return cmd;

}

// copied from: https://www.quora.com/How-do-you-write-a-C-program-to-split-a-string-by-a-delimiter
char** strsplit(const char* str, const char* delim, size_t* numtokens) {
// copy the original string so that we don't overwrite parts of it
// (don't do this if you don't need to keep the old line,
// as this is less efficient)
	char *s = strdup(str);
// these three variables are part of a very common idiom to
// implement a dynamically-growing array
	size_t tokens_alloc = 1;
	size_t tokens_used = 0;
	char **tokens = calloc(tokens_alloc, sizeof(char*));
	char *token, *strtok_ctx;
	for (token = strtok_r(s, delim, &strtok_ctx); token != NULL; token =
			strtok_r(NULL, delim, &strtok_ctx)) {
		// check if we need to allocate more space for tokens
		if (tokens_used == tokens_alloc) {
			tokens_alloc *= 2;
			tokens = realloc(tokens, tokens_alloc * sizeof(char*));
		}
		tokens[tokens_used++] = strdup(token);
	}
// cleanup
	if (tokens_used == 0) {
		free(tokens);
		tokens = NULL;
	} else {
		tokens = realloc(tokens, tokens_used * sizeof(char*));
	}
	*numtokens = tokens_used;
	free(s);
	return tokens;
}

struct command2 readCommand2() {
	struct command2 cmd;

	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	do {
		printf("esp> ");
		read = getline(&line, &len, stdin);
	} while (strcmp("\n", line) == 0); //ignore "empty" line

	printf("you entered: %s", line);

	size_t numtokens;
	char** tokens;
	tokens = strsplit(line, " \n", &numtokens);

	cmd.action = (tokens[0]);

	cmd.argc = numtokens - 1; //does this work?
	tokens++;
	cmd.argv = tokens;
	free(line);

	printf("action=%s, args=%d\n", cmd.action, cmd.argc);
	for (int i = 0; i < cmd.argc; i++) {
		printf("  cmd%d=%s\n", i, cmd.argv[i]);
	}
	puts("");
	return cmd;
}

// 1==valid, 0=invalid
int isValidAddCommand(struct AddCommand cmd) {

	if ((cmd.sex1 == 'm' || cmd.sex1 == 'f')
			&& (cmd.sex2 == 'm' || cmd.sex2 == 'f')) {
		return 1;
	}
	puts("TODO: validate add command not yet finished");
	return 0;
}

//-1=not found,x=position
Person* findPerson(PersonVector* vec, char* name, char sex) {
	for (int i = 0; i < vec->size; i++) {
		if ((strcmp(name, vec->people[i].name) == 0)
				&& (sex == vec->people[i].sex)) {
			return &vec->people[i];
		}
	}
	return NULL;
}
//NULL in case of error
//pointer to created person if success
Person* addPerson(PersonVector* vec, char* name, char sex) {
	if (vec->size == vec->capacity) {
		return NULL;
	}
	Person* p;
	p = malloc(sizeof(Person));
	p->name = name;
	p->sex = sex;
	p->mother = NULL;
	p->father = NULL;
	vec->people[vec->size] = *p;
	vec->size++;
	return p;
}

PersonVector* createPeople(int capacity) {
	PersonVector* result;

	result = malloc(sizeof(PersonVector));
	result->people = malloc(capacity * sizeof(Person));
	result->capacity = capacity;
	result->size = 0;

	return result;
}

int main(int argc, char *argv[]) {

	PersonVector* people;
	people = createPeople(10);

	struct command2 cmd;

	int exit = 0;
	int unknown_count = 1; //unknown_count

	do {
		cmd = readCommand2();

		if (strcmp("EOF", cmd.action) == 0) {
			exit = 1;
		} else if (strcmp("quit", cmd.action) == 0) {
			printf("good bye");
			exit = 1;
		} else if (strcmp("add", cmd.action) == 0) {
			puts("add command recognized");

			struct AddCommand add;
			add = parseAddCommand(cmd.argc, cmd.argv);
			if (isValidAddCommand(add) == 1) {
				puts("add command is valid");
				//char name[strlen(add.name1) + 1 + strlen("[f]")];
				//strcpy(name, add.name1);
				//strcpy(name, " [");
				//strcpy(name, "" + add.sex1);
				//strcpy(name, "]");

				Person* p1 = findPerson(people, add.name1, add.sex1);
				Person* p2 = findPerson(people, add.name2, add.sex2);
				if (p1 == NULL) { //not found
					p1 = addPerson(people, add.name1, add.sex1);
				}
				if (p2 == NULL) { //not found
					p2 = addPerson(people, add.name2, add.sex2);
				}
				if (strcmp("father", add.relation) == 0) {
					p2->father = p1;
				} else if (strcmp("mother", add.relation) == 0) {
					if (p2->mother == NULL) {
						p2->mother = p1;
					} else if (p2->mother != p1) {
						printf("%s already has a %s as mother", p2->name,
								p2->mother->name);
					}
				} else if (strcmp("mgm", add.relation) == 0) { //(mother-grandmother)
					if (p2->mother == NULL) { //create unkown person
						char name[100];
						sprintf(name, "?%d", unknown_count);
						unknown_count++;
						Person* mother = addPerson(people, name, 'f');
						p2->mother = mother;
						mother->mother = p1;
					} else { //relation already exists, check if that's true
						if (p2->mother->mother == NULL) { //add relation mother's mother
							p2->mother->mother = p1;
						} else if (p2->mother->mother != p1) {
							printf("[ERR] > invalid relation\n");
						}
					}
				}
			}
		} else if (strcmp("list", cmd.action) == 0) {
			if (cmd.argc > 0) {
				puts("[ERR] Wrong usage - list.");
			} else if (people->size == 0) {
				puts("No entries available.");
			} else {
				//TODO: sort
				for (int i = 0; i < people->size; i++) {
					printf("%s [%c]\n", people->people[i].name,
							people->people[i].sex);
				}
			}
		} else if (strcmp("print", cmd.action) == 0) { //HELPER METHOD. NOT REQUIRED
			if (people->size == 0) {
				puts("No entries available.");
			} else {
				//TODO: sort
				for (int i = 0; i < people->size; i++) {
					Person p = people->people[i];
					printf("%s [%c]\n", p.name, p.sex);
					if (p.mother != NULL)
						printf("  mother= %s [%c]\n", p.mother->name,
								p.mother->sex);
					if (p.father != NULL)
						printf(" father=%s [%c]\n", p.father->name,
								p.father->sex);
				}
			}
		}
	} while (exit == 0);

	return EXIT_SUCCESS;
}
