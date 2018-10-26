#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned char* arena;
uint32_t arenaSize;
unsigned char* arenaHelper;

void parse_command(char* cmd)
{
    const char* delims = " \n";

    char* cmd_name = strtok(cmd, delims);
    if (!cmd_name) {
        goto invalid_command;
    }

    if (strcmp(cmd_name, "INITIALIZE") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        // TODO - INITIALIZE
		arena = (unsigned char*) calloc(size, sizeof(unsigned char));
		arenaHelper = (unsigned char*) calloc(size, sizeof(unsigned char));
		arenaSize = size;

    } else if (strcmp(cmd_name, "FINALIZE") == 0) {
        // TODO - FINALIZE
		free(arena);
		free(arenaHelper);

    } else if (strcmp(cmd_name, "DUMP") == 0) {
        // TODO - DUMP
		int i = 0;
		int j = 0;
		for ( j = 0; j < arenaSize; j++ ) {
			if (j % 16 == 0 && j != 0 )
				printf("\n");			
			if (j % 16 == 0) {
				printf("%07X0\t", i);
				i++;
			}	
			printf("%02X ", arena[j]);
			if ( (j - 7) % 16 == 0 )
				printf(" ");
		}
		printf("\n");
		

    } else if (strcmp(cmd_name, "ALLOC") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        // TODO - ALLOC
		if ( size + 12 > arenaSize ) {
			printf("0\n");
			return;
		}
		int destroyer; //daca e true (1), nu s-a putut face alocarea
		int setter; //pentru a face mai putine parcurgeri
		int i = 0;
		int j = 0;
		for ( i = 0; i <= arenaSize - (size + 12); i++ ) {
			destroyer = 0;
			for ( j = i; j < i + size + 12; j++ ) {
				if ( arenaHelper[j] != 0 ) {
					destroyer = 1; //daca e pe 1, nu am gasit nimic bun
					setter = j; //pornim de aici in cautarile viitoare
				}	
			}
			if ( destroyer == 1 ) {
				i = setter; //o sa se faca i+1 dupa for => ajungem la urmatorul
				continue;
			}
			break;
		}
		if ( destroyer == 1 ) {
			printf("0\n");			
			return;	
		}
		int helper = i; //i este index-ul de inceput de bloc (+gestiune)
		for ( helper = i; helper > 0; helper-- ) {
			if ( arenaHelper[helper] == 1 ) {
				if ( i < 256 )
					arena[helper - 3] = i; //setam next-ul anteriorului
				//if ( i > 255 && < 65536 ) {		
				//}
				if ( helper < 256 )
					arena[i+4] = helper - 3; //setam anteriorul pentru next
				//if .. todo pentru restu
				break;
			}
		}
		for ( helper = i; helper < arenaSize; helper++ ) {
			if ( arenaHelper[helper] == 2 ) {//daca avea urmatorul alt anterior
				if ( i < 256 )
					arena[helper] = i;
				//todo restul
				if ( helper - 4 < 256 )
					arena[i] = helper - 4;
				//if.. todo
				break;
			}
		}

		arenaHelper[i] = 1; //prea putine atribuiri pentru for
		arenaHelper[i+1] = 1; //1 pentru primul octet de next
		arenaHelper[i+2] = 1;
		arenaHelper[i+3] = 1;
		arenaHelper[i+4] = 2; //2 pentru al 2-lea octet de anterior
		arenaHelper[i+5] = 2;
		arenaHelper[i+6] = 2;
		arenaHelper[i+7] = 2;
		arenaHelper[i+8] = 3; //3 pentru al 3-lea octet de size
		arenaHelper[i+9] = 3;
		arenaHelper[i+10] = 3;
		arenaHelper[i+11] = 3;

		arena[i+8] = size;		
		int k = 0;
		for ( k = 0; k < size; k++ ) {
			arenaHelper[i + 12 + k] = 4; //4 pentru spatiu propriu-zis alocat
		}
		
		printf("%d\n", i+12); //printeaza index-ul de dupa gestiune

    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        // TODO - FREE
		int j = index;
		while ( arenaHelper[j] == 4 ) { //sterge continutul de dupa index
			arenaHelper[j] = 0;
			arena[j] = 0;
			j++;
		}
		int i = index - 1;
		while ( arenaHelper[i] != 1 ) { //sterge gestiunea dinainte de index
			arenaHelper[i] = 0;
			arena[i] = 0;
			i--;
		}
		arenaHelper[i] = 0; //sterge restul gestiunii
		arena[i] = 0; //din nou, prea putine atribuiri pentru for-uri
		arenaHelper[i-1] = 0;
		arena[i-1] = 0;
		arenaHelper[i-2] = 0;
		arena[i-2] = 0;
		arenaHelper[i-3] = 0;
		arena[i-3] = 0;

		int minSaver, maxSaver; //stocare index index superior/inferior
		int minVerif = 0, maxVerif = 0; //verificare daca are bloc sup/inf
		for (i = index - 1; i >= 0; i--) {
			if (arenaHelper[i] == 1) {
				minSaver = i - 3; //setam index-ul inferior
				minVerif = 1; //are bloc inferior
				break;
			}
		}
		for (i = index - 1; i < arenaSize; i++ ) {
			if (arenaHelper[i] == 1) {
				maxSaver = i; //setam index-ul superior
				maxVerif = 1; //are bloc superior
				break;
			}
		}

		if ( maxVerif == 1 && minVerif == 1) { //are ambii tipi de vecini
			for (i = index - 1; i >= 0; i--) {
				if (arenaHelper[i] == 1) {
					arena[i-3] = maxSaver; //setam next-ul anteriorului
					break;
				}
			}
		} else if (maxVerif == 0 && minVerif == 1) { //n-are next
			for (i = index - 1; i >= 0; i--) {
				if (arenaHelper[i] == 1) {
					arena[i-3] = 0; //setam next-ul anteriorului la 0
					break;
				}
			}			
		}

		if ( minVerif == 1  && maxVerif == 1) { //are ambii tipi de vecini
			for (i = index - 1; i < arenaSize; i++) {
				if (arenaHelper[i] == 2) {
					arena[i] = minSaver; //setam anteriorul
					break;
				}
			}
		} else if (minVerif == 0 && maxVerif == 1) { //n-are anterior
			for (i = index - 1; i < arenaSize; i++) {
				if (arenaHelper[i] == 2) {
					arena[i] = 0; //setam anteriorul next-ului la 0
					break;
				}
			}
		}

    } else if (strcmp(cmd_name, "FILL") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* value_str = strtok(NULL, delims);
        if (!value_str) {
            goto invalid_command;
        }
        uint32_t value = atoi(value_str);
        // TODO - FILL
		int count = 0; //auxiliar pentru a vedea daca atingem size
		int i = index;
		while ( count != size ) {
			if ( i >= arenaSize ) {
				break; //out of bounds
			}
			if ( arenaHelper[i] == 4 ) { //nu umplem gestiunea
				arena[i] = value;
				count++; //am mai adaugat unu
			}
			i++; //avansam cu o pozitie
		}

    } else if (strcmp(cmd_name, "ALLOCALIGNED") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* align_str = strtok(NULL, delims);
        if (!align_str) {
            goto invalid_command;
        }
        uint32_t align = atoi(align_str);
        // TODO - ALLOCALIGNED

    } else if (strcmp(cmd_name, "REALLOC") == 0) {
        printf("Found cmd REALLOC\n");
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        // TODO - REALLOC

    } else {
        goto invalid_command;
    }

    return;

invalid_command:
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(void)
{
    ssize_t read;
    char* line = NULL;
    size_t len;

    /* parse input line by line */
    while ((read = getline(&line, &len, stdin)) != -1) {
        /* print every command to stdout */
        printf("%s", line);

        parse_command(line);
    }

    free(line);

    return 0;
}
