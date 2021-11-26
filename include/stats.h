#ifndef STATS_H
#define STATS_H

#include "skip_list.h"
#include "date_list.h"


typedef struct pop_status_stats{

    int cnt_vaccinated_date_range;
    int cnt_vaccinated;
    int cnt_not_vaccinated;

}pop_status_stats;


typedef struct pop_status_stats_by_age{

    int cnt_vaccinated_date_range_0;
    int cnt_vaccinated_date_range_20;
    int cnt_vaccinated_date_range_40;
    int cnt_vaccinated_date_range_60_plus;

    int cnt_vaccinated_0;
    int cnt_vaccinated_20;
    int cnt_vaccinated_40;
    int cnt_vaccinated_60_plus;

    int cnt_not_vaccinated_0;
    int cnt_not_vaccinated_20;
    int cnt_not_vaccinated_40;
    int cnt_not_vaccinated_60_plus;

}pop_status_stats_by_age;


int initialize_stats_by_age(pop_status_stats_by_age *stats);

int compare_dates(vaccination_date_node date1, vaccination_date_node date2, vaccination_date_node *cur_date);

int calculate_vaccinated_persons_all_countries(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, country_node *head_country);


int calculate_vaccinated_persons(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, char *country);


int calculate_vaccinated_persons_all_countries_by_age(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, country_node *head_country);


int calculate_vaccinated_persons_by_age(skip_list **skip_lists, char *virus_name, int cnt_of_viruses,
                                 vaccination_date_node date1, vaccination_date_node date2, char *country);


void initialize_dates_inf(vaccination_date_node *date1, vaccination_date_node *date2);

void print_commands();



#endif