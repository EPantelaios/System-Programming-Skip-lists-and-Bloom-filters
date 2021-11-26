#include "../include/stats.h"


int compare_dates(vaccination_date_node date1, vaccination_date_node date2, vaccination_date_node *cur_date){

    int cnt=0;

    if(date1.year<cur_date->year)
        cnt++;
    else if(date1.year==cur_date->year && date1.month<cur_date->month)
        cnt++;
    else if(date1.year==cur_date->year && date1.month==cur_date->month && date1.day<=cur_date->day)
        cnt++;

    if(date2.year>cur_date->year)
        cnt++;
    else if(date2.year==cur_date->year && date2.month>cur_date->month)
        cnt++;
    else if(date2.year==cur_date->year && date2.month==cur_date->month && date2.day>=cur_date->day)
        cnt++;

    if(cnt==2)
        return true;
    else
        return false;
}



int initialize_stats_by_age(pop_status_stats_by_age *stats){

    stats->cnt_vaccinated_date_range_0=0;
    stats->cnt_vaccinated_date_range_20=0;
    stats->cnt_vaccinated_date_range_40=0;
    stats->cnt_vaccinated_date_range_60_plus=0;

    stats->cnt_vaccinated_0=0;
    stats->cnt_vaccinated_20=0;
    stats->cnt_vaccinated_40=0;
    stats->cnt_vaccinated_60_plus=0;

    stats->cnt_not_vaccinated_0=0;
    stats->cnt_not_vaccinated_20=0;
    stats->cnt_not_vaccinated_40=0;
    stats->cnt_not_vaccinated_60_plus=0;

    return 0;
}




int calculate_vaccinated_persons_all_countries(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, country_node *head_country){

    char *country=NULL;
    vaccination_date_node *cur_date;

    int index=find_index_skip_list(skip_lists, cnt_of_viruses, virus_name);
    
    skip_list_node *current_vacc=NULL;
    skip_list_node *current_non_vacc=NULL;
    country_node *current_country = head_country;


    while(current_country!=NULL){

        pop_status_stats stats;
        stats.cnt_vaccinated_date_range=0;
        stats.cnt_vaccinated=0;
        stats.cnt_not_vaccinated=0;

        country=(char *)calloc(strlen(current_country->country)+1, sizeof(char));
        strncpy(country, current_country->country, strlen(current_country->country));
        memset(country+strlen(current_country->country), '\0', 1);

        //for vaccinated skip list
        current_vacc = skip_lists[index]->head[0];
        while(current_vacc != NULL){
            
            if(!strcmp(current_vacc->info_citizen->country_list->country, country)){

                stats.cnt_vaccinated++;
                
                cur_date=current_vacc->date;
                
                if(compare_dates(date1, date2, cur_date)==true){
                    stats.cnt_vaccinated_date_range++;
                }
            }

            current_vacc = current_vacc->next;
        }

        //for non-vaccinated skip list
        current_non_vacc = skip_lists[index+1]->head[0];
        while(current_non_vacc != NULL){

            if(!strcmp(current_non_vacc->info_citizen->country_list->country, country)){

                stats.cnt_not_vaccinated++;
            }

            current_non_vacc = current_non_vacc->next;
        }

        if((stats.cnt_vaccinated + stats.cnt_not_vaccinated)!=0){

            float percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range / (float)(stats.cnt_vaccinated + stats.cnt_not_vaccinated)) * 100.00;
            printf("%s %d %.2f%%\n", country, stats.cnt_vaccinated_date_range, percentage_vaccinated);
        }

        current_country = current_country->next;
        free(country);
    }

    return 0;
}



int calculate_vaccinated_persons(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, char *country){

    vaccination_date_node *cur_date;
    
    skip_list_node *current_vacc=NULL;
    skip_list_node *current_non_vacc=NULL;

    pop_status_stats stats;
    stats.cnt_vaccinated_date_range=0;
    stats.cnt_vaccinated=0;
    stats.cnt_not_vaccinated=0;


    int index=find_index_skip_list(skip_lists, cnt_of_viruses, virus_name);

    //for vaccinated skip list
    current_vacc = skip_lists[index]->head[0];
    while(current_vacc != NULL){
        
        if(!strcmp(current_vacc->info_citizen->country_list->country, country)){

            stats.cnt_vaccinated++;
            
            cur_date=current_vacc->date;
            
            if(compare_dates(date1, date2, cur_date)==true){
                stats.cnt_vaccinated_date_range++;
            }
        }

        current_vacc = current_vacc->next;
    }

    //for non-vaccinated skip list
    current_non_vacc = skip_lists[index+1]->head[0];
    while(current_non_vacc != NULL){

        if(!strcmp(current_non_vacc->info_citizen->country_list->country, country)){

            stats.cnt_not_vaccinated++;
        }

        current_non_vacc = current_non_vacc->next;
    }


    if((stats.cnt_vaccinated + stats.cnt_not_vaccinated)!=0){

        float percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range / (float)(stats.cnt_vaccinated + stats.cnt_not_vaccinated)) * 100.00;
        printf("%s %d %.2f%%\n\n", country, stats.cnt_vaccinated_date_range, percentage_vaccinated);
    }

    return 0;
}





int calculate_vaccinated_persons_all_countries_by_age(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, country_node *head_country){

    char *country=NULL;
    float percentage_vaccinated=0.0;
    vaccination_date_node *cur_date;

    int index=find_index_skip_list(skip_lists, cnt_of_viruses, virus_name);
    
    skip_list_node *current_vacc=NULL;
    skip_list_node *current_non_vacc=NULL;
    country_node *current_country = head_country;

    while(current_country!=NULL){

        //declare struct with population stats by age
        pop_status_stats_by_age stats;
        
        initialize_stats_by_age(&stats);

        country=(char *)calloc(strlen(current_country->country)+1, sizeof(char));
        strncpy(country, current_country->country, strlen(current_country->country));
        memset(country+strlen(current_country->country), '\0', 1);

        //for vaccinated skip list
        current_vacc = skip_lists[index]->head[0];
        while(current_vacc != NULL){
            
            if(!strcmp(current_vacc->info_citizen->country_list->country, country)){

                if(current_vacc->info_citizen->age<=20)
                    stats.cnt_vaccinated_0++;
                else if(current_vacc->info_citizen->age<=40)
                    stats.cnt_vaccinated_20++;
                else if(current_vacc->info_citizen->age<=60)
                    stats.cnt_vaccinated_40++;
                else if(current_vacc->info_citizen->age>=61)
                    stats.cnt_vaccinated_60_plus++;


                cur_date=current_vacc->date;
                if(compare_dates(date1, date2, cur_date)==true){
                    
                    if(current_vacc->info_citizen->age<=20)
                        stats.cnt_vaccinated_date_range_0++;
                    else if(current_vacc->info_citizen->age<=40)
                        stats.cnt_vaccinated_date_range_20++;
                    else if(current_vacc->info_citizen->age<=60)
                        stats.cnt_vaccinated_date_range_40++;
                    else if(current_vacc->info_citizen->age>=61)
                        stats.cnt_vaccinated_date_range_60_plus++;
                }
            }

            current_vacc = current_vacc->next;
        }

        //for non-vaccinated skip list
        current_non_vacc = skip_lists[index+1]->head[0];
        while(current_non_vacc != NULL){

            if(!strcmp(current_non_vacc->info_citizen->country_list->country, country)){

                if(current_non_vacc->info_citizen->age<=20)
                    stats.cnt_not_vaccinated_0++;
                else if(current_non_vacc->info_citizen->age<=40)
                    stats.cnt_not_vaccinated_20++;
                else if(current_non_vacc->info_citizen->age<=60)
                    stats.cnt_not_vaccinated_40++;
                else if(current_non_vacc->info_citizen->age>=61)
                    stats.cnt_not_vaccinated_60_plus++;
            }

            current_non_vacc = current_non_vacc->next;
        }

        printf("%s\n", country);
        
        if((stats.cnt_vaccinated_0 + stats.cnt_not_vaccinated_0)!=0){

            percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_0 / (float)(stats.cnt_vaccinated_0 + stats.cnt_not_vaccinated_0)) * 100.00;
            printf("0-20 %d %.2f%%\n", stats.cnt_vaccinated_date_range_0, percentage_vaccinated);
        }

        if((stats.cnt_vaccinated_20 + stats.cnt_not_vaccinated_20)!=0){

            percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_20 / (float)(stats.cnt_vaccinated_20 + stats.cnt_not_vaccinated_20)) * 100.00;
            printf("21-40 %d %.2f%%\n", stats.cnt_vaccinated_date_range_20, percentage_vaccinated);
        }

        if((stats.cnt_vaccinated_40 + stats.cnt_not_vaccinated_40)!=0){

            percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_40 / (float)(stats.cnt_vaccinated_40 + stats.cnt_not_vaccinated_40)) * 100.00;
            printf("41-60 %d %.2f%%\n", stats.cnt_vaccinated_date_range_40, percentage_vaccinated);
        }

        if((stats.cnt_vaccinated_60_plus + stats.cnt_not_vaccinated_60_plus )!=0){

            percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_60_plus  / (float)(stats.cnt_vaccinated_60_plus  + stats.cnt_not_vaccinated_60_plus )) * 100.00;
            printf("60+ %d %.2f%%\n", stats.cnt_vaccinated_date_range_60_plus, percentage_vaccinated);
        }

        printf("\n\n");

        current_country = current_country->next;
        free(country);
    }

    return 0;
}




int calculate_vaccinated_persons_by_age(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, char *country){


    float percentage_vaccinated=0.0;
    vaccination_date_node *cur_date;

    int index=find_index_skip_list(skip_lists, cnt_of_viruses, virus_name);
    
    skip_list_node *current_vacc=NULL;
    skip_list_node *current_non_vacc=NULL;

    //declare struct with population stats by age
    pop_status_stats_by_age stats;
    
    initialize_stats_by_age(&stats);

    //for vaccinated skip list
    current_vacc = skip_lists[index]->head[0];
    while(current_vacc != NULL){
        
        if(!strcmp(current_vacc->info_citizen->country_list->country, country)){

            if(current_vacc->info_citizen->age<=20)
                stats.cnt_vaccinated_0++;
            else if(current_vacc->info_citizen->age<=40)
                stats.cnt_vaccinated_20++;
            else if(current_vacc->info_citizen->age<=60)
                stats.cnt_vaccinated_40++;
            else if(current_vacc->info_citizen->age>=61)
                stats.cnt_vaccinated_60_plus++;


            cur_date=current_vacc->date;
            if(compare_dates(date1, date2, cur_date)==true){
                
                if(current_vacc->info_citizen->age<=20)
                    stats.cnt_vaccinated_date_range_0++;
                else if(current_vacc->info_citizen->age<=40)
                    stats.cnt_vaccinated_date_range_20++;
                else if(current_vacc->info_citizen->age<=60)
                    stats.cnt_vaccinated_date_range_40++;
                else if(current_vacc->info_citizen->age>=61)
                    stats.cnt_vaccinated_date_range_60_plus++;
            }
        }

        current_vacc = current_vacc->next;
    }

    //for non-vaccinated skip list
    current_non_vacc = skip_lists[index+1]->head[0];
    while(current_non_vacc != NULL){

        if(!strcmp(current_non_vacc->info_citizen->country_list->country, country)){

            if(current_non_vacc->info_citizen->age<=20)
                stats.cnt_not_vaccinated_0++;
            else if(current_non_vacc->info_citizen->age<=40)
                stats.cnt_not_vaccinated_20++;
            else if(current_non_vacc->info_citizen->age<=60)
                stats.cnt_not_vaccinated_40++;
            else if(current_non_vacc->info_citizen->age>=61)
                stats.cnt_not_vaccinated_60_plus++;
        }

        current_non_vacc = current_non_vacc->next;
    }


    printf("%s\n", country);

    if((stats.cnt_vaccinated_0 + stats.cnt_not_vaccinated_0)!=0){

        percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_0 / (float)(stats.cnt_vaccinated_0 + stats.cnt_not_vaccinated_0)) * 100.00;
        printf("0-20 %d %.2f%%\n", stats.cnt_vaccinated_date_range_0, percentage_vaccinated);
    }

    if((stats.cnt_vaccinated_20 + stats.cnt_not_vaccinated_20)!=0){

        percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_20 / (float)(stats.cnt_vaccinated_20 + stats.cnt_not_vaccinated_20)) * 100.00;
        printf("21-40 %d %.2f%%\n", stats.cnt_vaccinated_date_range_20, percentage_vaccinated);
    }

    if((stats.cnt_vaccinated_40 + stats.cnt_not_vaccinated_40)!=0){

        percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_40 / (float)(stats.cnt_vaccinated_40 + stats.cnt_not_vaccinated_40)) * 100.00;
        printf("41-60 %d %.2f%%\n", stats.cnt_vaccinated_date_range_40, percentage_vaccinated);
    }

    if((stats.cnt_vaccinated_60_plus + stats.cnt_not_vaccinated_60_plus )!=0){

        percentage_vaccinated = (float)(stats.cnt_vaccinated_date_range_60_plus  / (float)(stats.cnt_vaccinated_60_plus  + stats.cnt_not_vaccinated_60_plus )) * 100.00;
        printf("60+ %d %.2f%%\n", stats.cnt_vaccinated_date_range_60_plus, percentage_vaccinated);
    }

    return 0;
}




void initialize_dates_inf(vaccination_date_node *date1, vaccination_date_node *date2){

    date1->day=1;
    date1->month=1;
    date1->year=1;

    date2->day=1;
    date2->month=1;
    date2->year=3000;
}




void print_commands(){

    printf("\n\n--->  Enter one of the following commands  <---\n\n");
    printf("/vaccineStatusBloom citizenID virusName\n");
    printf("/vaccineStatus citizenID [virusName]\n");
    printf("/populationStatus [country] virusName [date1 date2]\n");
    printf("/popStatusByAge [country] virusName [date1 date2]\n");
    printf("/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]\n");
    printf("/vaccinateNow citizenID firstName lastName country age virusName\n");
    printf("/list-nonVaccinated-Persons virusname\n");
    printf("/exit\n\n");
}