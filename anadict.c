#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


//Max line size in the dictionary, max word size for the other input
#define MAX_LINE_SIZE 256

//Data used by the anagram searching algorithm
int first_primes[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};
char frequent_letters[26] = {'e', 'a', 's', 'i', 't', 'n', 'r', 'u', 'l', 'o', 'd', 'c', 'p', 'm', 'v', 'q', 'f', 'b', 'g' ,'h' ,'j', 'x', 'y', 'z', 'w', 'k'};
int letter_primes[26];      //Table associating the most used letters to the smallest primes

void generate_letter_primes() {
    for(int i = 0; i < 26; i ++ ) {
        letter_primes[frequent_letters[i] - 'a'] = first_primes[i];
    }
}

//List structure to store the results
typedef struct List {
	unsigned int x;
	struct List *next;
} List;

//Utility functions for List
List* prepend(int v, List* current) {
	List *l = malloc(sizeof(List));
	l->x = v;
	l->next = current;
	return l;
}

void list_free(List *l) {
    if(l != NULL) {
        if(l->next != NULL) {
            list_free(l->next);
        }
        free(l);
    }
}

// Multipling 1 by a different prime for each letter gives a unique hash, as long as it doesn't overflow...
// Word MUST contain only chars between 'a' and 'z' and not be NULL
unsigned long long hash(char* word) {
	unsigned long long result = 1;
	for(int i = 0; word[i] != '\0'; i ++) {
		result *= letter_primes[word[i] - 'a'];
		//if(result > 0xFFFFFFFFFFFFFFF) printf("Overflow risk!!!!\n");
	}
	return result;
}

//Formatting of the result
int print_results(char **dict, unsigned int dict_len, char **words, unsigned int words_len, List **anagrams) {
	for(unsigned int i = 0; i < words_len; i ++) {
		if(anagrams[i] != NULL) {
			printf("%s: ", words[i]);
			List *l = anagrams[i];
			while(l->next != NULL) {
				printf("%s, ", dict[l->x]);
				l = l->next;
			}
			printf("%s\n", dict[l->x]);
		}
	}
	return 0;
}

//Finding anagrams
int print_anagrams(char** dict, unsigned int dict_len, char** words, unsigned int words_len) {
	//To store the results
	List **anagrams = malloc(words_len * sizeof(List));
	unsigned long long *word_hashes = malloc(words_len * sizeof(unsigned long long));
	//Initialisation
	for(unsigned int i = 0; i < words_len; i ++) {
		anagrams[i] = NULL;
		word_hashes[i] = hash(words[i]);
	}
	//Check with hashes of all words in the dict
	for(unsigned int i = 0; i < dict_len; i ++) {
		unsigned long long current_hash = hash(dict[i]);
		for(unsigned int j = 0; j < words_len; j ++) {
			if(current_hash == word_hashes[j]) {
				anagrams[j] = prepend(i, anagrams[j]);
			}
		}
	}
	//Print results
	print_results(dict, dict_len, words, words_len, anagrams);
	//Free memory
	for(unsigned int i = 0; i < words_len; i ++) {
		list_free(anagrams[i]);
		free(words[i]);
	}
	for(unsigned int i = 0; i < dict_len; i ++) {
		free(dict[i]);
	}
	free(anagrams);
	free(word_hashes);
	free(words);
	free(dict);
	return 0;
}

//Input handling functions
unsigned int count_file_words(FILE *f) {
	unsigned int count = 0;
	rewind(f);
	char buffer[MAX_LINE_SIZE];
	while(fgets(buffer, MAX_LINE_SIZE, f) != NULL) {
		if(isalpha(buffer[0])) {		//Not a comment, an empty line or anything else
			count ++;
		}
	}
	return count;
}

unsigned int count_words(FILE **files, unsigned int files_len) {
	unsigned int total = 0;
	FILE *f;
	char c;
	int in_a_word = 0;
	for(unsigned int i = 0; i < files_len; i ++) {
		f = files[i];
		rewind(f);
		while((c = getc(f)) != EOF) {
			if(isalpha(c) && !in_a_word) {
				in_a_word = 1;
				total ++;
			} else if(!isalpha(c)) {
				in_a_word = 0;
			}
		}
	}
	return total;
}

//Reads the contents of f into dst, which must be allocated
int read_file_words(FILE *f, char ***dst) {
	char buffer[MAX_LINE_SIZE];
	unsigned int i = 0;
	rewind(f);
	while(fgets(buffer, MAX_LINE_SIZE, f) != NULL) {
		if(isalpha(buffer[0])) {		//On a word
			(*dst)[i] = malloc( (strlen(buffer) + 1) * sizeof(char));
			strcpy((*dst)[i], buffer);
			for(unsigned int j = 0; j < strlen((*dst)[i]); j ++) {
				if(isalpha((*dst)[i][j])) {
					(*dst)[i][j] = tolower((*dst)[i][j]);	//Ensuring all words are lowercase, otherwise the hash function will fail
				} else {
					(*dst)[i][j] = '\0';		//Stopping on the first non-alpha character
					break;
				}
			}
			i ++;
		}
	}
	return 0;
}

int read_words(FILE **files, unsigned int files_len, char ***dst) {
	unsigned int pos = 0;
	char buf[MAX_LINE_SIZE];
	unsigned int buffer_pos = 0;
	int in_a_word = 0;
    char c;
	FILE *f;
	for(unsigned int i = 0; i < files_len; i ++) {
		f = files[i];
        rewind(f);
		while((c = getc(f)) != EOF) {
			if(isalpha(c) && !in_a_word) {
				in_a_word = 1;
                buffer_pos = 0;
            }
            if(isalpha(c)) {
                buf[buffer_pos] = tolower(c);
                buffer_pos ++;
            } else if(in_a_word) {
				in_a_word = 0;
                buf[buffer_pos] = '\0';
                (*dst)[pos] = malloc((buffer_pos + 1) * sizeof(char));
                strcpy((*dst)[pos], buf);
                pos ++;
			}
		}
        if(in_a_word) {
            buf[buffer_pos] = '\0';
            (*dst)[pos] = malloc((buffer_pos + 1) * sizeof(char));
            strcpy((*dst)[pos], buf);
        }
	}
	return 0;
}

int parse_dict(char *filename, char ***dict, unsigned int *len) {
	FILE *f = fopen(filename, "r");
	if(!f) {
		return 1;
	}
	*len = count_file_words(f);
	*dict = malloc(*len * sizeof(char*));
	read_file_words(f, dict);
	fclose(f);
	return 0;
}

int parse_words(int argc, char **argv, char ***words, unsigned int *len) {
	FILE **files = malloc((argc - 2) * sizeof(FILE*));
	for(int i = 2; i < argc; i ++) {
		files[i-2] = fopen(argv[i], "r");
		if(!files[i-2]) {
			return 1;
		}
	}
	*len = count_words(files, (unsigned int) (argc - 2));
	*words = malloc((*len) * sizeof(char*));
    read_words(files, argc - 2, words);
	for(int i = 2; i < argc; i ++) {
		fclose(files[i-2]);
	}
	free(files);
	return 0;
}

int scan_stdin(char ***words, unsigned int *words_len) {
    char *line = NULL;
    unsigned long s;
    int in_a_word = 0;
    char buffer[MAX_LINE_SIZE];
    unsigned int line_pos = 0;
    unsigned int buf_pos = 0;
    *words_len = 0;
    getline(&line, &s, stdin);
    for(unsigned int i = 0; i < s; i ++) {
    	if(isalpha(line[i]) && ! in_a_word) {
    		in_a_word = 1;
    		*words_len = *words_len + 1;
    	} else if(!isalpha(line[i])) {
			in_a_word = 0;
		}
    }
    *words = malloc((*words_len) * sizeof(char*));
    for(unsigned int i = 0; i < *words_len; i ++) {
    	while(line_pos < s && !isalpha(line[line_pos])) {
    		line_pos ++;
    	}
    	buf_pos = 0;
    	while(line_pos + buf_pos < s && isalpha(line[line_pos + buf_pos])) {
    		buffer[buf_pos] = line[line_pos + buf_pos];
    		buf_pos ++;
    	}
    	buffer[buf_pos] = '\0';
    	(*words)[i] = malloc((buf_pos + 1) * sizeof(char*));
    	strcpy((*words)[i], buffer);
    	line_pos += buf_pos;
    }
	return 0;
}

//Entry point
int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Usage: anadict dictfile [inputfile ...]\n");
		return EXIT_SUCCESS;
	}
	generate_letter_primes();
	char **dict = NULL;
	unsigned int dict_len = 0;
	if(parse_dict(argv[1], &dict, &dict_len)) {
		printf("Error: can't read from dictionary\n");
		return EXIT_FAILURE;
	}
	char **words = NULL;
	unsigned int words_len = 0;
	if(argc >= 3) {
		if(parse_words(argc, argv, &words, &words_len)) {
			printf("Error: can't read words\n");
			return EXIT_FAILURE;
		}
	} else {
		scan_stdin(&words, &words_len);
	}
	print_anagrams(dict, dict_len, words, words_len);
	return EXIT_SUCCESS;
}
