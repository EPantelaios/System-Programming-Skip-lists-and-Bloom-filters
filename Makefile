OBJS	= main.o skip_list.o bloom_filter.o hash_functions.o check_input.o initialize.o insert_entry.o info_citizen_list.o country_list.o date_list.o stats.o
OUT		= vaccineMonitor
CC 		= gcc
FLAGS	= -std=gnu99 -c -Wall 

$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $@ -lm



#Compile the individual files separately.
main.o: src/main.c
	$(CC) $(FLAGS) src/main.c

skip_list.o: src/skip_list.c include/skip_list.h
	$(CC) $(FLAGS) src/skip_list.c

bloom_filter.o: src/bloom_filter.c include/bloom_filter.h
	$(CC) $(FLAGS) src/bloom_filter.c

hash_functions.o: src/hash_functions.c include/hash_functions.h
	$(CC) $(FLAGS) src/hash_functions.c

check_input.o: src/check_input.c include/check_input.h
	$(CC) $(FLAGS) src/check_input.c

initialize.o: src/initialize.c include/initialize.h
	$(CC) $(FLAGS) src/initialize.c

insert_entry.o: src/insert_entry.c include/insert_entry.h
	$(CC) $(FLAGS) src/insert_entry.c

info_citizen_list.o: src/info_citizen_list.c include/info_citizen_list.h
	$(CC) $(FLAGS) src/info_citizen_list.c

country_list.o: src/country_list.c include/country_list.h
	$(CC) $(FLAGS) src/country_list.c

date_list.o: src/date_list.c include/date_list.h
	$(CC) $(FLAGS) src/date_list.c

stats.o: src/stats.c include/stats.h
	$(CC) $(FLAGS) src/stats.c




clean:
	rm -f $(OBJS) $(OUT)