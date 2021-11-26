#!/bin/bash

args=("$@")
virusesFile="${args[0]}"
countriesFile="${args[1]}"
numLines="${args[2]}"
duplicatesAllowed="${args[3]}"


#check arguments
if [ ! -f "$virusesFile" ]; then

    printf "The file '%s' does not exist\n" $virusesFile
    exit 1
fi

if [ ! -f "$countriesFile" ]; then

    printf "The file '%s' does not exist\n" $countriesFile
    exit 1
fi

if (( $numLines < 1 )); then

    printf "Only positive numbers > 0 are allowed\n"
    exit 1
fi

if (( $duplicatesAllowed < 0 )) || (( $duplicatesAllowed > 1 )); then

    printf "duplicatesAllowed takes only 2 valid values: 0 and 1\n"
    exit 1
fi


index=0
while read line ; do
    virus_array[$index]="$line"
    index=$(($index+1))
done < <(grep "" $virusesFile)


index=0
while read line ; do
    country_array[$index]="$line"
    index=$(($index+1))
done < <(grep "" $countriesFile)


#declare variables
yes_or_no[0]="YES"
yes_or_no[1]="NO"
declare -a names_array
declare -a citizen_info
capital_chars=$(echo {A..Z} | tr -d ' ')


if [[ "$duplicatesAllowed" == 0 ]]; then

    declare -a citizen_id_array

    if (( $numLines > 10000 )); then

        print_message="\nIt is not possible to have more than 10000 unique citizenID.\n"
        print_message="${print_message}So exactly 10000 lines will be produced.\n"
        printf "$print_message"

        numLines=10000
    fi

    #create $numLines unique IDs 
    citizen_id_array=($(shuf -i 0-9999 -n "$numLines"))

    if (( $numLines > 1000 )); then

        generate_names=$((numLines/2))
    else

        generate_names=$numLines
    fi

    #Generate array with random names and surnames
    #with the first letter of the word being a capital
    for ((i = 0 ; i < $generate_names ; i++)); do

        first_letter=${capital_chars:$((RANDOM%26+1)):1}
        names_array[$i]="$first_letter$(shuf -zer -n $(((RANDOM % 10)+2))  {a..z} | tr -d '\0')"

    done


    for ((i = 0 ; i < $numLines ; i++)); do

        citizenID=${citizen_id_array[$i]}
        name=${names_array[$RANDOM % ${#names_array[@]}]}
        surname=${names_array[$RANDOM % ${#names_array[@]}]}
        country=${country_array[$RANDOM % ${#country_array[@]}]}
        age=$(( (RANDOM % 120) + 1 ))
        virus=${virus_array[$RANDOM % ${#virus_array[@]}]}
        yes_no=${yes_or_no[ $[$RANDOM % 2] ]}

        if [[ "$yes_no" == "YES" ]]; then

            day=$(( (RANDOM % 30) + 1 ))
            month=$(( (RANDOM % 12) + 1 ))
            year=$(( (RANDOM % 21) + 2000 ))

            line_record="$citizenID $name $surname $country $age $virus $yes_no $day-$month-$year"
        
        else

            line_record="$citizenID $name $surname $country $age $virus $yes_no"

        fi

        #store record at array
        citizen_info[$i]="$line_record"

    done


    #Last step write the records in the file
    if [ -f "inputFile" ]; then

        rm -f inputFile
    fi

    touch inputFile

    printf "%s\n" "${citizen_info[@]}" > inputFile

    truncate -s -1 inputFile

    echo "Done! The file has been created."


elif [[ "$duplicatesAllowed" == 1 ]]; then

    declare -a store_citizen_data

    if (( $numLines > 1000 )); then

        generate_names=$((numLines/2))
    else

        generate_names=$numLines
    fi

    #Generate array with random names and surnames
    #with the first letter of the word being a capital
    for ((i = 0 ; i < $generate_names ; i++)); do

        first_letter=${capital_chars:$((RANDOM%26+1)):1}
        names_array[$i]="$first_letter$(shuf -zer -n $(((RANDOM % 10)+2))  {a..z} | tr -d '\0')"

    done


    for ((i = 0 ; i < $numLines ; i++)); do

        citizenID=$(( (RANDOM % 9999) + 1 ))
        name=${names_array[$RANDOM % ${#names_array[@]}]}
        surname=${names_array[$RANDOM % ${#names_array[@]}]}
        country=${country_array[$RANDOM % ${#country_array[@]}]}
        age=$(( (RANDOM % 120) + 1 ))
        virus=${virus_array[$RANDOM % ${#virus_array[@]}]}
        yes_no=${yes_or_no[ $[$RANDOM % 2] ]}

        #store personal data of the citizen if the same citizenID found
        personal_info="$name $surname $country $age"


        if [[ "$yes_no" == "YES" ]]; then

            day=$(( (RANDOM % 30) + 1 ))
            month=$(( (RANDOM % 12) + 1 ))
            year=$(( (RANDOM % 21) + 2000 ))

            #get first word ($name) from array for current citizenID
            set -- ${store_citizen_data[$citizenID]}

            #if the current citizenID has not been found again
            if [[ "$1" == "" ]]; then

                store_citizen_data[$citizenID]=$personal_info
                line_record="$citizenID $name $surname $country $age $virus $yes_no $day-$month-$year"
            else

                line_record="$citizenID ${store_citizen_data[$citizenID]} $virus $yes_no $day-$month-$year"
            fi

        else

            #get first word ($name) from array for current citizenID
            set -- ${store_citizen_data[$citizenID]}

            #if the current citizenID has not been found again
            if [[ "$1" == "" ]]; then

                store_citizen_data[$citizenID]=$personal_info
                line_record="$citizenID $name $surname $country $age $virus $yes_no"
            else

                line_record="$citizenID ${store_citizen_data[$citizenID]} $virus $yes_no"
            fi

        fi

        #store record at array
        citizen_info[$i]="$line_record"

    done

    #Last step write the records in the file
    if [ -f "inputFile" ]; then

        rm -f inputFile
    fi

    touch inputFile

    printf "%s\n" "${citizen_info[@]}" > inputFile

    truncate -s -1 inputFile

    echo "Done! The file has been created."


else

    exit 2
fi