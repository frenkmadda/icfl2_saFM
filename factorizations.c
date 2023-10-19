#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.c"

// ---------------------- CFL -----------------------------------------------------------------
// CFL - Lyndon factorization - Duval's algorithm
node_t *CFL(const char* word) {

    //CFL Duval's algorithm.

	node_t *current_pointer = NULL;

    int k = 0, i, j;
    int word_len = strlen(word);

    while (k < word_len) {
        i = k + 1;
        j = k + 2;
        while (1) {
            if (j == word_len + 1 || (unsigned char) word[j - 1] < (unsigned char) word[i - 1]) {
                while (k < i) {
                	node_t *node =  malloc(sizeof(node_t));
                	node->factor = substring(word, k, k + j - i);
                	node->next = current_pointer;
                	current_pointer = node;
                    k = k + j - i;
                }
                break;
            } else {
                if ((unsigned char)word[j - 1] > (unsigned char) word[i - 1]) {
                    i = k + 1;
                } else {
                    i = i + 1;
                }
                j = j + 1;
            }
        }
    }
    return current_pointer;
}

// ----------------------- ICFL ----------------------------------------------------------------------
/**
    Usa il simbolo dei 3/4 come separatore in virt� della sua ridotta presenza nei testi da tradurre, pu� essere cambiato con un altro simbolo all'occorrenza
**/
node_t *find_pre(const char* word) {

	int word_len = strlen(word);
	node_t *fact1 = NULL, *fact2 = NULL;
    //e se la parola � lunga 0?
    if (word_len == 1) {

    	fact1 =  malloc(sizeof(node_t));

		char *new_fact;
		new_fact =  malloc(word_len + 2);
		int z;
		for (z = 0; z < word_len; z++){
			new_fact[z] = word[z];
		}
		new_fact[z] = '�';
		new_fact[z+1] = '\0';

		fact1->factor = new_fact;
		//strcat(fact1->factor, word);
		//strcat(fact1->factor, "�");

    	fact2 =  malloc(sizeof(node_t));
    	fact2->factor = malloc(1);
    	strcpy(fact2->factor, "");
    	//fact2->factor = "";
    	fact2->next = NULL;

    	fact1->next = fact2;

        return fact1;

    } else {
        int i = 0, j = 1;

        while ((j < word_len) && ((unsigned char)word[j] <= (unsigned char)word[i])) {
            if ((unsigned char) word[j] < (unsigned char) word[i])
                i = 0;
            else
                i = i + 1;

            j = j + 1;
        }

        if (j == word_len) {

        	fact1 =  malloc(sizeof(node_t));

        	char *new_fact;
        	new_fact =  malloc(word_len + 2);
        	int z;
        	for (z = 0; z < word_len; z++){
        		new_fact[z] = word[z];
        	}
        	new_fact[z] = '�';
        	new_fact[z+1] = '\0';

        	fact1->factor = new_fact;
        	//strcat(fact1->factor, word);
        	//strcat(fact1->factor, "�");

			fact2 =  malloc(sizeof(node_t));
			fact2->factor = malloc(1);
			strcpy(fact2->factor, "");
			//fact2->factor = "";
			fact2->next = NULL;

			fact1->next = fact2;

			return fact1;

        } else {

        	fact1 =  malloc(sizeof(node_t));
        	fact1->factor = substring(word, 0, j + 1);

        	fact2 =  malloc(sizeof(node_t));
        	fact2->factor = substring(word, j + 1, word_len);
        	fact2->next = NULL;

        	fact1->next = fact2;

            return fact1;
        }
    }
}

void border(char p[], int **pi) {
    int l = strlen(p);
    *pi = (int *) malloc(sizeof(int) * l);
    int k = 0;
    int i, j = 1;

    *(pi[0]) = 0;

    for (i = 1; i < l; i++) {
        while ((k > 0) && (p[k] != p[i])) {
        	k = (*pi)[k-1];
        }

        if (p[k] == p[i]) {
            k++;
        }
        (*pi)[j] = k;
        j++;
    }
}

node_t *find_bre(char *w, char *v) {

    node_t *fact1 = NULL, *fact2 = NULL, *fact3 = NULL, *last_index = NULL;

    if ((v[0] == '\0') && (strchr(w, '�') != NULL)) {

    	fact1 =  malloc(sizeof(node_t));
    	fact2 =  malloc(sizeof(node_t));
    	fact3 =  malloc(sizeof(node_t));
    	last_index =  malloc(sizeof(node_t));

    	fact1->factor = malloc(strlen(w) + 1);
    	strcpy(fact1->factor, w);
    	fact2->factor = malloc(1);
    	strcpy(fact2->factor, "");
		//fact2->factor = "";
    	fact3->factor = malloc(1);
		strcpy(fact3->factor, "");
		//fact3->factor = "";
		last_index->factor = malloc(2);
		strcpy(last_index->factor, "0");
    	//last_index->factor = "0";

    	last_index->next = NULL;
    	fact3->next = last_index;
    	fact2->next = fact3;
    	fact1->next = fact2;

    	return fact1;

    } else {
        int n = strlen(w) - 1;
        int *f = NULL;
        char *sub = substring(w, 0, n);
        border(sub, &f);

        int i = n;
        int last = f[i-1];

        while (i > 0) {
            if ((unsigned char) w[f[i-1]] < (unsigned char) w[n])
                last = f[i-1];
            i = f[i-1];
        }

        free(sub);
        free(f);

        sub = NULL;
        f = NULL;

        fact1 =  malloc(sizeof(node_t));
		fact2 =  malloc(sizeof(node_t));
		fact3 =  malloc(sizeof(node_t));

		//count digits in last
		i = last;
		int digit_count = 0;

		if(i == 0) {
			digit_count = 1;
		} else {
			while(i != 0) {
		    	i /= 10;
		    	digit_count += 1;
			}
		}

		last_index =  malloc(sizeof(node_t));
		last_index->factor =  malloc(digit_count + 1);

		fact1->factor = substring(w, 0, n - last);
		fact2->factor = substring(w, n - last, n + 1);
		fact3->factor = malloc(strlen(v) + 1);
		strcpy(fact3->factor, v);
		sprintf(last_index->factor, "%d", last);

		last_index->next = NULL;
		fact3->next = last_index;
		fact2->next = fact3;
		fact1->next = fact2;

		return fact1;
    }
}

void compute_icfl_recursive(char word[], node_t **curr_pointer_icfl) {

    // At each step compute the current bre
    node_t *pre_pair = find_pre(word);
    char *fact1 = malloc(strlen(pre_pair->factor) + 1);
    char *fact2 = malloc(strlen(pre_pair->next->factor) + 1);
    strcpy(fact1, pre_pair->factor);
    strcpy(fact2, pre_pair->next->factor);
    node_t *current_bre_quad = find_bre(fact1, fact2);

    free(fact1);
    free(fact2);
    fact1 = NULL;
    fact2 = NULL;
    free_list(pre_pair);

    if ((current_bre_quad->next->factor[0] == '\0') && (strchr(current_bre_quad->factor, '�') != NULL)) {
        char *w = current_bre_quad->factor;
        node_t * icfl_node =  malloc(sizeof(node_t));
        icfl_node->factor = substring(w, 0, strlen(w) - 1);

        if (*curr_pointer_icfl == NULL) {
        	 icfl_node->next = NULL;
        	 *curr_pointer_icfl = icfl_node;
        } else {
        	icfl_node->next = *curr_pointer_icfl;
        	*curr_pointer_icfl = icfl_node;
        }
        free_list(current_bre_quad);
        return;

    } else {
    	char *fact1_fact2 = malloc(strlen(current_bre_quad->next->factor) + strlen(current_bre_quad->next->next->factor) + 1);
    	fact1_fact2[0] = '\0';
    	strcat(fact1_fact2, current_bre_quad->next->factor);
    	strcat(fact1_fact2, current_bre_quad->next->next->factor);

        compute_icfl_recursive(fact1_fact2, curr_pointer_icfl);
        if (strlen((*curr_pointer_icfl)->factor) > atoi(current_bre_quad->next->next->next->factor)) {

        	node_t * icfl_node =  malloc(sizeof(node_t));
        	icfl_node->factor = malloc(strlen(current_bre_quad->factor) + 1);
        	strcpy(icfl_node->factor, current_bre_quad->factor);

        	if (*curr_pointer_icfl == NULL) {
        		icfl_node->next = NULL;
        	    *curr_pointer_icfl = icfl_node;
        	} else {
        	    icfl_node->next = *curr_pointer_icfl;
        	    *curr_pointer_icfl = icfl_node;
        	}

        } else {
        	node_t *new_icfl_node =  malloc(sizeof(node_t));
        	new_icfl_node->factor = malloc(strlen(current_bre_quad->factor) + 1);
        	strcpy(new_icfl_node->factor, current_bre_quad->factor);
        	//strcat(new_icfl_node->factor, current_bre_quad->factor);
        	strcat(new_icfl_node->factor, (*curr_pointer_icfl)->factor);
        	new_icfl_node->next = (*curr_pointer_icfl)->next;
        	(*curr_pointer_icfl)->next = NULL;
        	*curr_pointer_icfl = new_icfl_node;
        }
        free(fact1_fact2);
        fact1_fact2 = NULL;
        free_list(current_bre_quad);
        return;
    }
}


//ICFL recursive (without using of compute_br)- Inverse Lyndon factorization
node_t *ICFL_recursive(char word[]) {
    //In this version of ICFL, we don't execute compute_br - one only O(n) scanning of word

	node_t *curr_pointer_icfl = NULL;

    compute_icfl_recursive(word, &curr_pointer_icfl);

    return curr_pointer_icfl;
}

