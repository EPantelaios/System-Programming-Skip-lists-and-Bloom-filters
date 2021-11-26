#include "../include/skip_list.h"
#include "../include/bloom_filter.h"
#include "../include/info_citizen_list.h"
#include "../include/country_list.h"
#include "../include/date_list.h"
#include "../include/check_input.h"
#include "../include/stats.h"
#include "../include/insert_entry.h"
#include "../include/initialize.h"


int main(int argc, char *argv[]){

    FILE *fp=NULL;
    char *citizen_records_file=NULL;
    int ret_val=0, bloom_size=0, cnt_of_viruses=0, number_of_args=0;
    const int size=100;
    const int const_var=1;
    int max_index_skip_list=0;
    char error_number[3]="-1";
    size_t buffersize=1000;
    int index=-1;
    const char word_separation_chars_without_dash[4] = " \n";
    int citizen_id=0;
    char *token, *buffer, *tmp_buf, *aux_buf, *citizen_id_str, *country, *virus_name, *yes_or_no;

    //to generate pseudo-random numbers
    srand(time(NULL));

    //head pointers for citizen, country and date list
    info_citizen_node *head_citizen=NULL;
    country_node *head_country=NULL;
    vaccination_date_node *head_date=NULL;

    check_arguments(argc, argv,  &citizen_records_file, &bloom_size);

    assert(!initialize_aux_vars(&buffer, &tmp_buf, &aux_buf, &citizen_id_str, &country, &virus_name, 
                    &yes_or_no, buffersize, size));

    int lines =count_lines(citizen_records_file);
    const int max_layer=(int)log2(lines)+const_var;

    cnt_of_viruses=count_viruses(lines, citizen_records_file);
    int tmp_viruses=cnt_of_viruses*2;

    char **viruses_array;
    assert(!initialize_viruses_array(&viruses_array, cnt_of_viruses, size));

    unsigned char **bloom_filter_array;
    assert(!initialize_bloom_filter(&bloom_filter_array, cnt_of_viruses));

    skip_list **skip_lists;
    assert(!initialize_skip_list(&skip_lists, tmp_viruses));


    if ((fp=fopen(citizen_records_file, "r"))==NULL){

        //if cannot open the file exit
        printf("Could not open file %s\n", citizen_records_file);
        exit(-1);
    } 
    else{

        for(int i=0;i<lines;i++){

            if(getline(&buffer, &buffersize, fp)!=(EOF)){
                
                strcpy(tmp_buf ,buffer);
                strcpy(aux_buf, buffer);

                if(buffer==NULL || buffer[0]=='\n'){
                    
                    continue;
                }

                //check the format and the number of tokens of the record
                number_of_args=correct_format_record(aux_buf);
                if(number_of_args==false){

                    print_error_record(aux_buf);
                    continue;
                }

                ret_val = insert_entry(skip_lists, viruses_array, cnt_of_viruses, bloom_filter_array, bloom_size, &head_country, &head_citizen,
                                       &head_date, max_layer, buffer, tmp_buf, number_of_args);

                if(ret_val<0){

                    continue;
                }
            }
        }
    }

    fclose(fp);


    int cnt=0, cnt_of_args=0;
    char str_yes[4]="YES";
    char str_no[3]="NO";
    skip_list_node *node;
    ssize_t read_line;

    //give commands from the keyboard
    while(true){

        print_commands();

        read_line=getline(&buffer, &buffersize, stdin);

        if(read_line==-1){

            printf("False input. Try again...\n");
            continue;
        }
        
        if(!strcmp("\n", buffer)){
            continue;
        }
       
        strcpy(tmp_buf ,buffer);
        strcpy(aux_buf, buffer);

        tmp_buf=strtok(tmp_buf, "\n");
        token=strtok(buffer, word_separation_chars_without_dash);


        //Command: vaccineStatusBloom citizenID virusName
        if(!strcmp("/vaccineStatusBloom", token)){

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);

            //correct number of args
            if(cnt_of_args!=3){

                printf("False input. Try again...\n");
                continue;
            }

            strcpy(aux_buf, tmp_buf);
            strtok(aux_buf, word_separation_chars_without_dash);

            token=strtok(NULL, word_separation_chars_without_dash);

            strcpy(citizen_id_str, token); 
            if(!check_if_contain_only_numbers_and_not_letters(citizen_id_str)){

                print_error_input(tmp_buf);
                return -1;
            }

            citizen_id=atoi(citizen_id_str);
            if(citizen_id<0){

                print_error_input(tmp_buf);
                continue;
            }

            //convert integer to string
            sprintf(citizen_id_str, "%d", citizen_id);

            strcpy(virus_name, scan_file_char_virus_name(aux_buf));
            if(!strcmp(virus_name, error_number)){

                print_error_input(tmp_buf);
                continue;
            }

            if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                continue;
            }

            int pos=find_index_virus_name(viruses_array, cnt_of_viruses, virus_name);

            if(search_bloom_filter(bloom_filter_array[pos], bloom_size, citizen_id_str)==true){

                printf("MAYBE\n");
            }
            else{

                printf("NOT VACCINATED\n");
            }
        }


        //Command: vaccineStatus citizenID [virusName]
        else if(!strcmp("/vaccineStatus", token)){

            cnt=0;

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);

            strcpy(aux_buf, tmp_buf);
            token=strtok(aux_buf, word_separation_chars_without_dash);

            //vaccineStatus citizenID
            if(cnt_of_args==2){

                citizen_id=scan_id_and_age(aux_buf);
                if(citizen_id<0){

                    print_error_input(tmp_buf);
                    continue;
                }

                max_index_skip_list=max_index_skip_lists(viruses_array, cnt_of_viruses);

                for(int i=0;i<=max_index_skip_list;i++){

                    node=find_entry_skip_list(skip_lists, i, citizen_id);

                    if(node!=NULL){ //found record

                        if(i%2==0){

                            if(node->date!=NULL){

                                printf("%s %s %d-%d-%d\n", skip_lists[i]->virus_name, str_yes, node->date->day, node->date->month, node->date->year);
                            }
                        }
                        else{

                            printf("%s %s\n", skip_lists[i]->virus_name, str_no);
                        }

                        cnt++;
                    }
                }

                if(cnt==0){

                    printf("There are not records for CitizenID: %d\n", citizen_id);
                }

            }
            //Command: vaccineStatus citizenID virusName
            else if(cnt_of_args==3){

                citizen_id=scan_id_and_age(aux_buf);
                if(citizen_id<0){

                    print_error_input(tmp_buf);
                    continue;
                }

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }

                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                index=find_index_skip_list(skip_lists, cnt_of_viruses, virus_name);

                node=find_entry_skip_list(skip_lists, index, citizen_id);
                if(node!=NULL){

                    printf("VACCINATED ON %d-%d-%d\n", node->date->day, node->date->month, node->date->year);
                }
                else{

                    printf("NOT VACCINATED\n");
                }

            }
            else{

                printf("False input. Try again...\n");
                continue;
            }
        }


        //Command: populationStatus [country] virusName date1 date2
        else if(!strcmp("/populationStatus", token)){
            
            vaccination_date_node date1;
            vaccination_date_node date2;

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);

            strcpy(aux_buf, tmp_buf);
            token=strtok(aux_buf, word_separation_chars_without_dash);

            if(cnt_of_args==2){   //Command: populationStatus virusName

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                initialize_dates_inf(&date1, &date2);

                calculate_vaccinated_persons_all_countries(skip_lists, virus_name, cnt_of_viruses, date1, date2, head_country);

            }
            else if(cnt_of_args==3){ //Command: populationStatus country virusName
 
                strcpy(country, scan_file_char(aux_buf));
                if(!strcmp(country, error_number) || search_country(head_country, country)==NULL){

                    print_error_input(tmp_buf);
                    continue;
                }

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                initialize_dates_inf(&date1, &date2);

                calculate_vaccinated_persons(skip_lists, virus_name, cnt_of_viruses, date1, date2, country);

            }
            else if(cnt_of_args==4){  //Command: populationStatus virusName date1 date2

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                date1.day=scan_file_date(aux_buf);
                date1.month=scan_file_date(aux_buf);
                date1.year=scan_file_date(aux_buf);
                if(!correct_date(date1.day, date1.month, date1.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                date2.day=scan_file_date(aux_buf);
                date2.month=scan_file_date(aux_buf);
                date2.year=scan_file_date(aux_buf);
                if(!correct_date(date2.day, date2.month, date2.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                calculate_vaccinated_persons_all_countries(skip_lists, virus_name, cnt_of_viruses, date1, date2, head_country);


            }   //Command: populationStatus country virusName date1 date2
            else if(cnt_of_args==5){
                
                strcpy(country, scan_file_char(aux_buf));
                if(!strcmp(country, error_number) || search_country(head_country, country)==NULL){

                    print_error_input(tmp_buf);
                    continue;
                }

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                date1.day=scan_file_date(aux_buf);
                date1.month=scan_file_date(aux_buf);
                date1.year=scan_file_date(aux_buf);
                if(!correct_date(date1.day, date1.month, date1.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                date2.day=scan_file_date(aux_buf);
                date2.month=scan_file_date(aux_buf);
                date2.year=scan_file_date(aux_buf);
                if(!correct_date(date2.day, date2.month, date2.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                calculate_vaccinated_persons(skip_lists, virus_name, cnt_of_viruses, date1, date2, country);

            }
            else{

                printf("False input. Try again...\n");
                continue;
            }
        }


        //Command: popStatusByAge [country] virusName date1 date2
        else if(!strcmp("/popStatusByAge", token)){

            vaccination_date_node date1;
            vaccination_date_node date2;

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);

            strcpy(aux_buf, tmp_buf);
            token=strtok(aux_buf, word_separation_chars_without_dash);

            if(cnt_of_args==2){   //Command: popStatusByAge virusName

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                initialize_dates_inf(&date1, &date2);

                calculate_vaccinated_persons_all_countries_by_age(skip_lists, virus_name, cnt_of_viruses, date1, date2, head_country);

            }
            else if(cnt_of_args==3){ //Command: popStatusByAge country virusName

                strcpy(country, scan_file_char(aux_buf));
                if(!strcmp(country, error_number) || search_country(head_country, country)==NULL){

                    print_error_input(tmp_buf);
                    continue;
                }

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                initialize_dates_inf(&date1, &date2);

                calculate_vaccinated_persons_by_age(skip_lists, virus_name, cnt_of_viruses, date1, date2, country);

            }
            else if(cnt_of_args==4){  //Command: popStatusByAge virusName date1 date2

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                date1.day=scan_file_date(aux_buf);
                date1.month=scan_file_date(aux_buf);
                date1.year=scan_file_date(aux_buf);
                if(!correct_date(date1.day, date1.month, date1.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                date2.day=scan_file_date(aux_buf);
                date2.month=scan_file_date(aux_buf);
                date2.year=scan_file_date(aux_buf);
                if(!correct_date(date2.day, date2.month, date2.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                calculate_vaccinated_persons_all_countries_by_age(skip_lists, virus_name, cnt_of_viruses, date1, date2, head_country);

            }//Command: popStatusByAge country virusName date1 date2
            else if(cnt_of_args==5){

                strcpy(country, scan_file_char(aux_buf));
                if(!strcmp(country, error_number) || search_country(head_country, country)==NULL){

                    print_error_input(tmp_buf);
                    continue;
                }

                strcpy(virus_name, scan_file_char_virus_name(aux_buf));

                if(!strcmp(virus_name, error_number)){

                    print_error_input(tmp_buf);
                    continue;
                }
                if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                    printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                    continue;
                }

                date1.day=scan_file_date(aux_buf);
                date1.month=scan_file_date(aux_buf);
                date1.year=scan_file_date(aux_buf);
                if(!correct_date(date1.day, date1.month, date1.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                date2.day=scan_file_date(aux_buf);
                date2.month=scan_file_date(aux_buf);
                date2.year=scan_file_date(aux_buf);
                if(!correct_date(date2.day, date2.month, date2.year)){
                    
                    print_error_input(tmp_buf);
                    continue;
                }

                calculate_vaccinated_persons_by_age(skip_lists, virus_name, cnt_of_viruses, date1, date2, country);

            }
            else{

                printf("False input. Try again...\n");
                continue;
            }
        }


        //Command: insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]
        else if(!strcmp("/insertCitizenRecord", token)){

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);

            if(cnt_of_args!=8 && cnt_of_args!=9){

                printf("False input. Try again...\n");
                continue;
            }
            
            strcpy(aux_buf, tmp_buf);
            token=strtok(aux_buf, word_separation_chars_without_dash);

            int ret=insert_citizen_record(skip_lists, viruses_array, cnt_of_viruses, bloom_filter_array, bloom_size, 
                                          &head_country, &head_citizen, &head_date, max_layer, aux_buf, tmp_buf, cnt_of_args);

            if(ret==0){
                
                printf("Done!\n");
            }
        }


        //Command: vaccinateNow citizenID firstName lastName country age virusName
        else if(!strcmp("/vaccinateNow", token)){

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);
            
            if(cnt_of_args!=7){

                printf("False input. Try again...\n");
                continue;
            }
            
            strcpy(aux_buf, tmp_buf);
            token=strtok(aux_buf, word_separation_chars_without_dash);

            int ret=vaccinate_now(skip_lists, viruses_array, cnt_of_viruses, bloom_filter_array, bloom_size, &head_country,
                                  &head_citizen, &head_date, max_layer, aux_buf, tmp_buf, cnt_of_args);

            if(ret==0){
                
                printf("Done!\n");
            }
        }


        //Command: list-nonVaccinated-Persons virusName
        else if(!strcmp("/list-nonVaccinated-Persons", token)){

            cnt_of_args=count_arguments_without_dash_symbol(aux_buf);

            if(cnt_of_args!=2){

                printf("False input. Try again...\n");
                continue;
            }
            
            strcpy(aux_buf, tmp_buf);
            token=strtok(aux_buf, word_separation_chars_without_dash);


            strcpy(virus_name, scan_file_char_virus_name(aux_buf));

            if(!strcmp(virus_name, error_number)){

                print_error_input(tmp_buf);
                continue;
            }
            if(is_new_virus(viruses_array, cnt_of_viruses, virus_name)==true){

                printf("ERROR INPUT. There is not virus with name: '%s'\n", virus_name);
                continue;
            }

            index = find_index_skip_list(skip_lists, cnt_of_viruses, virus_name);

            print_layer_zero_non_vaccinated(skip_lists, index+1);
            printf("\n");
        }


        //Command: exit
        else if(!strcmp("/exit", token)){

            delete_list_date(&head_date);
            delete_list_citizen(&head_citizen);
            delete_list_country(&head_country);

            delete_skip_list_structure(skip_lists, cnt_of_viruses*2);

            delete_bloom_filter(bloom_filter_array, cnt_of_viruses);

            for(int i=0;i<cnt_of_viruses;i++){

                free(viruses_array[i]);
            }
            free(viruses_array);

            free(buffer);
            free(aux_buf);
            free(tmp_buf);
            free(citizen_id_str);
            free(country);
            free(virus_name);
            free(yes_or_no);

            exit(0);
        }
        else{

            printf("Invalid input. Try again.\n");
        }
    }

    return 0;
}
