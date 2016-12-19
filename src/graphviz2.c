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

struct command {
	char* action;
	char* args;
};
struct command2 {
	char* action;

	int argc;
	char** argv;
};

struct AddCommand {
	int valid; //0=false, 1=true
	char* name1;
	char sex1;

	char* relation;

	char* name2;
	char sex2;
};

/**
 * "re-formats" / joins names in add command
 * (a name can have whitespaces, those need to be "joined")
 */
char** joinAddArguments(int argc, char* argv[]) {
	if (argc > 5) {
		puts("TODO: whitespaces in names currently not supported");
	}
	return argv;
}

char parseSex(char* token) {
	if (strcmp("[f]", token) == 0)
		return 'f';
	else if (strcmp("[m]", token) == 0)
		return 'm';
	return ' ';
}

struct AddCommand parseAddCommand(int argc, char* argv[]) {
	struct AddCommand cmd;

	char** tokens;
	tokens = joinAddArguments(argc, argv);

	if (argc < 5) {
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

	free(line);
	puts("");
//		for (int i=0;i<read;i++){
//			if (line[i]==' '){
//				puts("");
//			}else {
//				putchar(line[i]);
//			}
//		}
	//free(line);

	return cmd;

}

// copied from: https://www.quora.com/How-do-you-write-a-C-program-to-split-a-string-by-a-delimiter
char** mystrsplit(const char* str, const char* delim, size_t* numtokens) {
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
	} while (strcmp("\n",line)==0); //ignore "empty" line

	printf("you entered: %s", line);

	size_t numtokens;
	char** tokens;
	tokens = mystrsplit(line, " \n", &numtokens);
	//cmd.action=malloc(1*sizeof(char*));
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

int main(int argc, char *argv[]) {
	//puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	struct command2 cmd;
	cmd = readCommand2();

	while (strcmp("quit", cmd.action) != 0) {

		if (strcmp("add", cmd.action) == 0) {
			puts("add command recognized");

			struct AddCommand add;
			add = parseAddCommand(cmd.argc, cmd.argv);
			if (isValidAddCommand(add) == 1) {
				puts("add command is valid");
			}
			//printf("name1=%s,name2=%s\n", add.name1, add.name2);

			//char name1[100],sex1[1],relation[100],name2[100],sex2[1];
			//scanf("%s \\[%s\\] %s %s \\[%s\\]",name1,sex1,relation,name2,sex2);

//			int i = 0;
//			for (char c = line[i]; c != ' ' && c != '\n'; c = line[++i]) {
//				if (c == ' ') {
//					puts("");
//				} else {
//					putchar(c);
//				}
//			}

		}

		cmd = readCommand2();
	}

	printf("good bye");
	return EXIT_SUCCESS;
}
