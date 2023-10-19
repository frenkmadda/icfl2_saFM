#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "factorizations.c"
#define BUFF_SIZE 28762

int *compute_mask_index(node_t* facts){
    int *mask_index = malloc(sizeof(*mask_index)*lenght_list(facts));
    node_t* fact = facts;
    mask_index[0] = 0;
    for(int j = 1; j < lenght_list(facts); j++, fact = fact->next){
        mask_index[j] = mask_index[j-1] + strlen(fact->factor);
    }
    /*for(int i = 0; i < lenght_list(facts); i++){
        printf("%d\n",mask_index[i]);
    }*/
    return mask_index;

}

positions_lists_t* build_distinct_local_suffixes(node_t* facts){
    node_t* temp_facts = facts;
    positions_lists_t* pList = initPositionsLists();

    char* temp_local_suffix;
    while(temp_facts!=NULL){
        char* temp_factor = temp_facts->factor;
        int fact_len = strlen(temp_factor);
        for(int j = 0; j<fact_len;j++){
            temp_local_suffix = malloc(sizeof(*temp_local_suffix)*(fact_len+1));
            strncpy(temp_local_suffix,&temp_factor[j],fact_len+1);
            //printf("%s\n",temp_local_suffix);
            if(add_suffix(pList,temp_local_suffix) == NULL){
                //printf("already in: %s\n",temp_local_suffix);
                free(temp_local_suffix);
            }
            
        }
        temp_facts = temp_facts->next;
    }
   free(temp_facts); //FM
    return pList;
}

int LCP_length(char* X, char* Y){
    int i = 0;
    int Xlen = strlen(X);
    int Ylen = strlen(Y);

    for(;i < Xlen && i <Ylen;i++){
        if(X[i] != Y[i])
            break;
    }
    return i;
}

int * sort_suffixes(positions_lists_t* pList){
    int len = pList->len;
    char **suffix_array = pList->suffix_array;
    int *lcp_sorted_suffixes = malloc(sizeof(*lcp_sorted_suffixes)* len);
    lcp_sorted_suffixes[0] = 0;
    for(int j=1; j < len;j++){
        lcp_sorted_suffixes[j] = LCP_length(suffix_array[j-1],suffix_array[j]);
    }
    return lcp_sorted_suffixes;
}

void initialize_hash (positions_lists_t* suffix_array, char* w, int* mask_index, int mask_len){
    int w_len = strlen(w);
    char temp_suffix[w_len+1];

    if(mask_len == 1){
        for(int j = w_len-1; j >= 0; j--){
            strcpy(temp_suffix,&w[j]);
            add_position(suffix_array,temp_suffix,j);
        }
    }
    else{
        //step 1
        for(int i = mask_len -1; i > 0; i --){
            //printf("i: %d\n",i);
            for (int j = mask_index[i]-1; j >mask_index[i-1]-1;j--){
                //printf(" -j:%d\n",j);
                int len_local_suffix = mask_index[i] - j;
                strncpy(temp_suffix,&w[j],len_local_suffix);
                temp_suffix[len_local_suffix] = '\0';

                add_position(suffix_array,temp_suffix,j);
            }
        }
        //step 2
        for(int j = w_len-1; j > mask_index[mask_len-1]-1; j--){
            int len_local_suffix = w_len - j;
            //printf(" -j:%d ,len:%d\n",j,len_local_suffix);
            strncpy(temp_suffix,&w[j],len_local_suffix);
            temp_suffix[len_local_suffix] = '\0';

            add_position(suffix_array,temp_suffix,j);
        }

    }
    free(temp_suffix);//FM
}

void printChain(array_strings_t** speed_keys_array, positions_lists_t* posList, int len){ //Funzione che si occupa di stampare le catene con stampa degli indici nella posizione relativa al suffisso nella catena
    printf("Stampa Catene ---- taglia: %d\n", len);
    for (int h=0; h<=len-1;h++) { //Mi fermo a len-1 per non cadere nel segmentation fault o nella stampa di un array vuoto superfluo in più
    printf("%d ",speed_keys_array[h]->len);
    print_array_strings(speed_keys_array[h]);
    int chainLen=speed_keys_array[h]->len;
    if(chainLen>=1){ //FM: Stampo le catene accedendo alle liste delle posizioni dei singoli fattori nelle catene
        printf(" (");
        for(int p=0;p<chainLen;p++){   
             print_array_int(get_positions_array(posList,speed_keys_array[h]->strings[p]));
        }
      
        printf(")");
    }
       
    printf("\n");
        //aggiunta
    }

}

array_strings_t** speed_structure(positions_lists_t* posList, int* lcp_sorted_suffixes){
     int len = posList->len;
    array_strings_t** speed_keys_array = malloc(sizeof(array_strings_t*)*len);
     
    printf("Stampa lcp_sorted\n");
    for(int i = 0; i < len; i++){
        printf("%d, ", lcp_sorted_suffixes[i]);
         }
        printf("-------\n");
   
   
    int j;
    for(int i = len-1; i>= 0; i--){
        char* current_fact = posList->suffix_array[i];
        int current_len = strlen(current_fact);
        printf("fattore corrente %s\n", current_fact);
        speed_keys_array[i] = initArray_strings();
        addString(speed_keys_array[i],current_fact);

        j = i + 1;
        while ( j < len){
            if(lcp_sorted_suffixes[j] >= current_len){
                if(speed_keys_array[j]->len > 0){
                    addString(speed_keys_array[j],current_fact);
                    clear_array_strings(speed_keys_array[i]);
                }
                j = j+1;
            }
            else
                break;
        }
    }
  //int taglia=(int) sizeof(speed_keys_array);
    //printChain(speed_keys_array,posList,len); //Funzione che stampa le catene con indici non ordinati

    return speed_keys_array;
}

int fact_of(int index_to_insert, int* mask_index, int mask_len){
    for (int i = 1; i < mask_len; i++){
        if(index_to_insert >= mask_index[i-1] && index_to_insert < mask_index[i]){
            return i-1;
        }
    }
    return mask_len - 1;
}




array_int_t* merge_hash_lists_prefix(array_int_t* SA_area, array_int_t* current_list, int* mask_index, int mask_len, char* w){
    array_int_t* SA_temp = init_array_int(0);
    
    int start_index_to_compare = 0;
    int start_index_to_insert = 0;

    int index_to_insert;
    int index_to_compare;

    int last_factor_index;
    int fact_index_to_insert;
    int fact_index_to_compare;


    int len_SA = SA_area->len;
    int len_current_list = current_list->len;
    while(start_index_to_compare < len_SA && start_index_to_insert < len_current_list){

        //The array's head is last position
        index_to_insert = current_list->ints[len_current_list-1-start_index_to_insert];
        index_to_compare = SA_area->ints[len_SA-1-start_index_to_compare];

        last_factor_index = mask_len - 1;
        fact_index_to_insert = fact_of(index_to_insert, mask_index,mask_len);
        fact_index_to_compare = fact_of(index_to_compare,mask_index,mask_len);

        if(fact_index_to_insert == fact_index_to_compare){
            
            if(fact_index_to_insert == last_factor_index){
                if(index_to_insert < index_to_compare){
                    add_int_array(SA_temp,index_to_compare);
                    start_index_to_compare++;
                }
            }
            else{
                if(index_to_insert < index_to_compare){
                    add_int_array(SA_temp,index_to_insert);
                    start_index_to_insert++;
                }
            }
        }
        else if(fact_index_to_insert > fact_index_to_compare){

            //giusto
            char* suffix_to_insert;
            int len_suffix;

            if(mask_len > (fact_index_to_insert + 1)){
                int insert_len_cpy = mask_index[fact_index_to_insert+1]-index_to_insert;
                len_suffix = insert_len_cpy + 1;
                suffix_to_insert = malloc(sizeof(*suffix_to_insert) * len_suffix);
                strncpy(suffix_to_insert,&w[index_to_insert],insert_len_cpy);
                suffix_to_insert[insert_len_cpy] = '\0';
            }
            else{
                int insert_len_cpy = strlen(&w[index_to_insert]);
                len_suffix = insert_len_cpy + 1;
                suffix_to_insert = malloc(sizeof(*suffix_to_insert) * len_suffix);
                strncpy(suffix_to_insert,&w[index_to_insert],insert_len_cpy);
                suffix_to_insert[insert_len_cpy] = '\0';
            }

            int compare_len_cpy = mask_index[fact_index_to_compare+1]-index_to_compare;
            char suffix_to_compare[compare_len_cpy+1];
            strncpy(suffix_to_compare,&w[index_to_compare],compare_len_cpy);
            suffix_to_compare[compare_len_cpy] = '\0';

    
            if(strncmp(suffix_to_insert, suffix_to_compare, len_suffix) == 0){
                    add_int_array(SA_temp,index_to_compare);
                    start_index_to_compare++;
            }
            else{
                int l = LCP_length(&w[index_to_compare],&w[index_to_insert]);
                int i1 = index_to_compare + l;
                int i2 = index_to_insert + l;
                
                if(i1 >= strlen(w)){
                    add_int_array(SA_temp,index_to_compare);
                    start_index_to_compare++;
                }

                else if(i2 >= strlen(w)){
                    add_int_array(SA_temp,index_to_insert);
                    start_index_to_insert++;
                }
                
                else{
                    if(w[i1] < w[i2]){
                    add_int_array(SA_temp,index_to_compare);
                    start_index_to_compare++;
                    }
                    else{
                        add_int_array(SA_temp,index_to_insert);
                        start_index_to_insert++;
                    }
                }
            }
        }
        else{
            if(fact_index_to_compare != last_factor_index){
                int insert_len_cpy = mask_index[fact_index_to_insert+1]-index_to_insert;
                int compare_len_cpy = mask_index[fact_index_to_compare+1]-index_to_compare;

                char suffix_to_insert[insert_len_cpy+1];
                strncpy(suffix_to_insert,&w[index_to_insert],insert_len_cpy);
                suffix_to_insert[insert_len_cpy] = '\0';

                char suffix_to_compare[compare_len_cpy+1];
                strncpy(suffix_to_compare,&w[index_to_compare],compare_len_cpy);
                suffix_to_compare[compare_len_cpy] = '\0';

                if(strncmp(suffix_to_compare, suffix_to_insert, strlen(suffix_to_compare)) == 0){
                    add_int_array(SA_temp,index_to_insert);
                    start_index_to_insert++;
                }
                else{
                    int l = LCP_length(&w[index_to_insert],&w[index_to_compare]);
                    int i1 = index_to_insert + l;
                    int i2 = index_to_compare + l;

                    if (w[i1] < w[i2]){
                        add_int_array(SA_temp, index_to_insert);
                        start_index_to_insert++;
                    }
                    else{
                        add_int_array(SA_temp, index_to_compare);
                        start_index_to_compare++;
                    }
                }
            }
            else{
                add_int_array(SA_temp,index_to_compare);
                start_index_to_compare++;
            }
        }
    }

    for(int i = len_SA-1-start_index_to_compare; i >= 0; i--){
        add_int_array(SA_temp,SA_area->ints[i]);
    }
    for(int i = len_current_list-1-start_index_to_insert; i >= 0; i--){
        add_int_array(SA_temp,current_list->ints[i]);
    }
    
    int *arr = SA_temp->ints;
    int size = SA_temp->len;
    
    for (int i = 0; i < size/2; i++)
    {
        int temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }

    return SA_temp;
}

int get_pos_key_lista(array_int_t* key_lista, int item){
    for(int i = 0;i<key_lista->len;i++){
        if(key_lista->ints[i] == item){
            return i;
        }
    }
    return -1;
}

int binary_search(int* array, int x, int low, int high) {
  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (array[mid] == x)
      return mid;
    if (array[mid] < x)
      low = mid + 1;
    else
      high = mid - 1;
  }
  return -1;
}



array_int_t* hope_insert(int len_group,int key, int pos_key, positions_lists_t* group_dict, int* group_start_index, int* main_lista, array_int_t* key_lista ,int* group_dict_is_taken){
    array_int_t* SA = init_array_int(0);

    char int_suffix[12];
    sprintf(int_suffix, "%d\0", key);
    array_int_t* lista = get_positions_array(group_dict,int_suffix);
   //printPositionsLists(group_dict); 

    if(lista->len > 1){

        for(int j = 0; j < lista->len; j++){

            int k = lista->ints[j] - 1;
            array_int_t* not_taken_list = init_array_int(0);
            array_int_t* not_taken_list_index= init_array_int(0);//FM creo un array parallelo con gli indici

            while(1){
                if(binary_search(group_start_index,k+1,0,len_group-1) >= 0)
                    break;
                
                int item = main_lista[k];
                int index=get_pos_key_lista(key_lista,item);//FM
                int taken = group_dict_is_taken[index];
                if(taken == 0){
                    k = k - 1;
                    add_int_array(not_taken_list,item);
                    add_int_array(not_taken_list_index,index);//FM

                }
                else
                    break;

            }

            //head is at end of array
            for (k = (not_taken_list->len)-1; k>= 0; k--){
                int position_key = not_taken_list_index->ints[k];
                //int position_key = get_pos_key_lista(key_lista,not_taken_list->ints[k]);

                array_int_t* SA_temp = hope_insert(len_group, not_taken_list->ints[k], position_key, group_dict, group_start_index, main_lista, key_lista, group_dict_is_taken);

                SA = concat_new_array_int(SA_temp, SA);
                group_dict_is_taken[position_key] = 1;
            }
           
        }
        // append at end of array
        array_int_t* SA_temp = init_array_int(1);
        add_int_array(SA_temp, key);
        SA = concat_new_array_int(SA_temp, SA);
        group_dict_is_taken[pos_key] = 1;
        free(SA_temp);//FM
    }
    else if(group_dict_is_taken[pos_key] == 0){
        array_int_t* SA_temp = init_array_int(1);
        add_int_array(SA_temp, key);
        SA = concat_new_array_int(SA_temp, SA);
        group_dict_is_taken[pos_key] = 1;
        free(SA_temp);//FM
    }
    return SA;
}

array_int_t* hope_merge(array_int_t** group, int len_group,array_strings_t** speed_keys_array){
    array_int_t* SA = init_array_int(0);

    //______________________
    // Creating main_lista and group_start_index
   int len_main_lista = 0;
    int group_start_index[len_group];
    for(int i = 0; i<len_group;i++){
        group_start_index[i] = len_main_lista;
        len_main_lista += group[i]->len;
    }

   int main_lista [len_main_lista];
   //index track the main_lista array
    for(int i = 0,index = 0; i < len_group;i++){
        //the chain's head is at end of it
        int len_chain = group[i]->len;
        for(int j = 0; j < len_chain; j++,index ++){
            main_lista[index] = group[i]->ints[len_chain-1-j];
        } 
    }
    //______________________
    // Creating group_dict and keys_lista
    positions_lists_t* group_dict = initPositionsLists();
    array_int_t* key_lista = init_array_int(0);

   
    for(int j = 0; j < len_main_lista;j++){
        // the suffix are int, they are converted in chars
        char suffix_int[12];
        sprintf(suffix_int, "%d\0", main_lista[j]);
        int check = contains_suffix(group_dict, suffix_int);
        if(check >= 0){
            char *suffix = malloc(sizeof(*suffix) * 12); //Suffix è un indice 
            strncpy(suffix,suffix_int,12);
            add_suffix(group_dict,suffix);
            add_position(group_dict,suffix_int,j);
            //need be dynamically updated
            add_int_array(key_lista,main_lista[j]); 
        }
        else{
            //it is not an addition in the head
            add_position(group_dict,suffix_int,j);
        }

    }
    //  array parallel to key_lista
    int *group_dict_is_taken = calloc(key_lista->len,sizeof(int));


    add_int_array(SA,key_lista->ints[0]);
    group_dict_is_taken[0] = 1;

    //____________________
    // find key
    array_int_t* SA_temp;
    for(int i = 1; i < key_lista->len;i++){
        int key = key_lista->ints[i];

        if(group_dict_is_taken[i] == 0){
            SA_temp = hope_insert(len_group, key, i, group_dict, group_start_index, main_lista, key_lista, group_dict_is_taken);
            SA = concat_new_array_int(SA_temp, SA);
        }
    }
    printf("("); //STAMPA INDICI DEL GRUPPO COMPUTATO
    print_array_int(SA);
    printf(")\n");
    free(SA_temp);//FM
    return SA;
}


void step_1(char* word, positions_lists_t** dest_suffix_dict, int** dest_lcp_sorted_suffixes, int** dest_mask_index, int* dest_mask_len){

    node_t* list = ICFL_recursive(word);
   /* printf("Lista Step 1: [");
    print_list(list);
    printf("]\n");
*/
    //mask_index
    int* mask_index = compute_mask_index(list); //funzione da poter eliminare, icfl da rivedere DELICATINO
    int mask_len = lenght_list(list);


    //position list with distinct local suffixes
    positions_lists_t* suffix_dict;
    suffix_dict = build_distinct_local_suffixes(list); //PUNTO DI SEGMENTATION ALLA DECIMA STRINGA CON LETTURA DA FILE
    //lcp_sorted_suffixes
    int posList_len = suffix_dict->len;
    int *lcp_sorted_suffixes = sort_suffixes(suffix_dict);

    //initialize position list
    initialize_hash(suffix_dict,word,mask_index,mask_len); 

    //return value
    *dest_suffix_dict = suffix_dict;
    *dest_lcp_sorted_suffixes = lcp_sorted_suffixes;
    *dest_mask_index = mask_index;
    *dest_mask_len = mask_len;
    }

array_int_t* step_2(char* word, positions_lists_t* suffix_dict, int* lcp_sorted_suffixes, int* mask_index, int mask_len){
    //printPositionsLists(suffix_dict);
    array_strings_t** speed_keys_array = speed_structure(suffix_dict,lcp_sorted_suffixes);
    //the lenght is the same
    int len_speed = suffix_dict->len;
    array_int_t* SA = init_array_int(0);
    array_int_t* SA_group = init_array_int(0);
    array_int_t** group = malloc(sizeof(*group));
    array_int_t* current_SA;
    int len_group = 0;
    for(int i=0; i < len_speed; i++){
        array_strings_t* current_speed = speed_keys_array[i];
        printf("[%d] -",len_group);
        if(lcp_sorted_suffixes[i] == 0){
            if(len_group > 1){
                array_int_t* l = hope_merge(group,len_group,speed_keys_array); //In l c'è un gruppo 
                //(SA = SA + l)    
                //head is at end of l
                SA = concat_new_array_int(l,SA);
                
            }
            else{
                if(SA_group->len!=0){ //STAMPA DEGLI INDICI DELLE CATENE NON FORMANTI GRUPPI
                printf("(");
                print_array_int(SA_group); 
                printf(")\n");
                }
                SA = concat_new_array_int(SA_group,SA);
            }
            //reset group resetting its lenght
            
            len_group = 0;
        }

        if(current_speed->len != 0){
            //current_speed it sorted in reverse so start to the end
            //current_speed is the reverse of current_key_lista[??]
            print_array_strings(current_speed); //STAMPA DELLE CATENE DEI SUFFISSI PER LA STAMPA DEI GRUPPI
            current_SA = get_positions_array(suffix_dict,current_speed->strings[current_speed->len-1]);
            SA_group = current_SA;
            for(int index = 1; index < current_speed->len;index++){
                array_int_t* current_list = get_positions_array(suffix_dict,current_speed->strings[current_speed->len-1-index]);
                SA_group = merge_hash_lists_prefix(current_SA, current_list,mask_index,mask_len,word); //Qui si fondono gli indici delle catene
                //print_array_int(SA_group);
                current_SA = SA_group;
            }
            printf("\n");
            len_group++;
            if(len_group>0)
                
            group = realloc(group, sizeof(array_int_t*)*len_group);
            group[len_group-1] = SA_group; 
            if( i == len_speed - 1){
                if(len_group == 1)
                    SA = concat_new_array_int(current_SA,SA); //Qui?-------------------------
                else{
                    array_int_t* l = hope_merge(group,len_group,speed_keys_array);
                    SA = concat_new_array_int(l,SA);
                }
               
            }
        }
        
    }
    if(len_group!=0){
     printf("(");
     print_array_int(SA_group);
     printf(")\n");
    }
  //printPositionsLists(suffix_dict); //IN SUFFIX DICT CI SONO LE LISTE DELLE POSIZIONI RISULTANTI DELLO STEP 1
    free(group);//FM
    free(current_SA);//FM
    free(SA_group);//FM
    return SA;
}

array_int_t* sorting_suffixes_via_icfl(char* word){

    positions_lists_t* suffix_dict;
    int* lcp_sorted_suffixes;
    int* mask_index;
    int mask_len;
    step_1(word, &suffix_dict, &lcp_sorted_suffixes, &mask_index, &mask_len);
    array_int_t* SA_w = step_2(word,suffix_dict, lcp_sorted_suffixes, mask_index, mask_len);

    free(lcp_sorted_suffixes);//FM
    free(mask_index); //FM
    free(suffix_dict);//FM
    
    return SA_w;
}

/* MAIN CON LETTURA DA FILE FASTA
int main(int argc, char **argv) { 
    char * word=NULL;
    char buff[BUFF_SIZE];
    char *ch;
    FILE *fp;
    size_t len =0;
    long read;


    struct timeval stop, start;
    

   fp=fopen("sampled_read.fasta","r");
    if(fp==NULL)
        exit(EXIT_FAILURE); 
    int wordN=0;

    while((len=getline(&word, &len, fp))){
        if(word[0] == '>')
            fprintf(stdout, "Non mi serve\n");
        else {
            fprintf(stdout,"Mi serve\n");
            word[len-1]='\0';
            fprintf(stdout,"Word Number: %d ----\n",wordN);
            fprintf(stdout,"%s\n",word);
            fflush(stdout);

          
            wordN++;
            gettimeofday(&start, NULL);
            array_int_t * SA_w = sorting_suffixes_via_icfl(word);

            int *arr = SA_w->ints;
            int size = SA_w->len;
    
            for (int i = 0; i < size/2; i++){
                 int temp = arr[i];
                 arr[i] = arr[size - 1 - i];
                 arr[size - 1 - i] = temp;
            }

            gettimeofday(&stop, NULL);
            printf("time: %lu\n", ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec) )/ 1000 );

  

             int len_w = strlen(word);
             printf("Lunghezza input: %d\n",len_w);
             printf("Lunghezza suffissi: %d\n", SA_w->len);

             printf("FINAL SA:\n[");

            for(int i = 0; i < SA_w->len;i++){
                fprintf(stderr,"%d, ",SA_w->ints[i]);
             }
            printf("]\n");
            word=NULL;
            
            /* char temp_string_1[len_w+1];
             char temp_string_2[len_w+1];
             int flag = 1;
            
             for(int i = 0; i < (SA_w->len) - 1 ;i++){
                int cheaj = strcmp( strcpy(temp_string_1,&word[SA_w->ints[i]]), strcpy(temp_string_2,&word[SA_w->ints[i+1]]) );
                if( cheaj > 0 ){
                    flag = 0;
                }
            }
             // rimuovere in fase di test 
             if (flag)
                printf("SA ordinato: True\n");
             else
                printf("SA ordinato: False\n");
            //rimettere end comment
                    
            fflush(stdout);
            free(SA_w);
            free(speed_structure);
        } 
    }  

}
*/

// MAIN BASE
int main(int argc, char **argv) {
    char * word;
    if(argc == 2){
        word = argv[1];
    }
    //default word
    else{
        //word = "zabazacazabazava";
        word="aaabcaabcadcaabca";
        //word="dabadabdabdadac";
    }
    
    struct timeval stop, start;
    gettimeofday(&start, NULL);


    array_int_t * SA_w = sorting_suffixes_via_icfl(word);

    int *arr = SA_w->ints;
    int size = SA_w->len;
    
    for (int i = 0; i < size/2; i++)
    {
        int temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }

    gettimeofday(&stop, NULL);
    printf("time: %lu\n", ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec) )/ 1000 );

  

    int len_w = strlen(word);
    printf("Lunghezza input: %d\n",len_w);
    printf("Lunghezza suffissi: %d\n", SA_w->len);

     printf("FINAL SA:\n[");
    for(int i = 0; i < SA_w->len;i++){
        fprintf(stderr,"%d, ",SA_w->ints[i]); //ATTENZIONE La stampa del suffix è sullo stderr per non sporcare stdout
    }
    printf("]\n");

  /* char temp_string_1[len_w+1]; //Controllo sulla correttezza dell'ordine del suffix Array, commentato perchè pesante su input molto grandi
    char temp_string_2[len_w+1];
    int flag = 1;
    for(int i = 0; i < (SA_w->len) - 1 ;i++){
        int cheaj = strcmp( strcpy(temp_string_1,&word[SA_w->ints[i]]), strcpy(temp_string_2,&word[SA_w->ints[i+1]]) );
        if( cheaj > 0 ){
            flag = 0;
        }
    }// rimuovere in fase di test
    if (flag)
        printf("SA ordinato: True\n");
    else
        printf("SA ordinato: False\n");
   */
    
  free(SA_w); //FM
  free(arr);  //FM   
}


