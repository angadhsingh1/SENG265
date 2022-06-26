/*
 * process_cal3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2021.
 * 
 * Name: Angadh Singh
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"
#include <time.h>

#define MAX_LINE_LEN 80

/*
 * Function dt_format.
 *
 * Given a date-time, creates a more readable version of the
 * calendar date by using some C-library routines. For example,
 * if the string in "dt_time" corresponds to:
 *
 *   20190520T111500
 *
 * then the string stored at "formatted_time" is:
 *
 *   May 20, 2019 (Mon).
 *
 */
void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;
    // char      temp[5];

    /*  
     * Ignore for now everything other than the year, month and date.
     * For conversion to work, months must be numbered from 0, and the 
     * year from 1900.
     */  
    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}

/*
 * Function dt_increment:
 *
 * Given a date-time, it adds the number of days in a way that
 * results in the correct year, month, and day. For example,
 * if the string in "before" corresponds to:
 *
 *   20190520T111500
 *
 * then the datetime string stored in "after", assuming that
 * "num_days" is 100, will be:
 *
 *   20190828T111500
 *
 * which is 100 days after May 20, 2019 (i.e., August 28, 2019).
 *
 */
void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time; 
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour, 
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));
    strncpy(after + 16, before + 16, MAX_LINE_LEN - 16); 
    after[MAX_LINE_LEN - 1] = '\0';
}

/*
* Function input_read :
* 
* The function takes in filename and intial count
* Reads the lines in the file and stores the events in the linked list 
* Finally the function returns the output list
*/
node_t *input_read(char *filename, int count) {
    
    char line[MAX_LINE_LEN];                                            // line stores each line in the given ics file
    char repetition_date[MAX_LINE_LEN] = "";                            // repetiton_date stores the date incremented by the dt_increment function
    node_t *list = NULL;                                                // list stores the nodes of the linked list; each node contains a cal event
    FILE *fileinfo = fopen(filename, "r");                              // opening a file

    while (fgets(line, MAX_LINE_LEN, fileinfo)) {
        if (strncmp(line, "BEGIN:VEVENT", 12)==0) {
            event_t *cal_event = (event_t*) emalloc(sizeof(event_t));   
            int checker = 0;

            while (strncmp(line, "END:VEVENT",10) != 0) {
                if (strncmp(line, "DTSTART:", 8)==0) {   sscanf(line, "DTSTART:%s", (*cal_event).dtstart);   }  // reads the dtstart from the file and stores in cal_event 
                if (strncmp(line, "DTEND:", 6)==0) {   sscanf(line, "DTEND:%s", (*cal_event).dtend);   }        // reads the dtend from the file and stores in cal_event

                if (strncmp(line, "RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=", 32)==0) {                                 // reads the until from the file and stores in cal_event
                    sscanf(line, "RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=%15s", (*cal_event).until);
                    checker = 1;
                }
                else if (strncmp(line, "RRULE:FREQ=WEEKLY", 17)==0) {                                           // reads the until from the file and stores in cal_event
                    sscanf(line, "RRULE:FREQ=WEEKLY;UNTIL=%15s", (*cal_event).until);
                    checker = 1;
                }
                if (strncmp(line, "LOCATION:", 9) == 0) {   sscanf(line, "LOCATION:%[^\t\n]", (*cal_event).location);   }   // reads the location from the file and stores in cal_event
                if (strncmp(line, "SUMMARY:", 8) == 0) {   sscanf(line, "SUMMARY:%[^\t\n]", (*cal_event).summary);   }      // reads the summary from the file and stores in cal_event

                fgets(line, 132, fileinfo);
            }
        list = add_inorder(list, new_node(cal_event));              // the event is added to the list using the add_inorder function which adds the elements inorder of the datestart
        count++;

        if(checker == 1){                                           // if the event is repeating then the new elements are added here
            dt_increment(repetition_date ,(*cal_event).dtstart, 7); // adds 7 days to datestart and adds new date to the repetition_date
            while (strncmp((*cal_event).until, repetition_date, 15)>=0) {

                event_t *cal_event1 = (event_t*) emalloc(sizeof(event_t));                  // creating a new node here to add to list
                memcpy(cal_event1->dtstart,repetition_date, strlen(cal_event->dtstart));    // stores the address of new date to new event
                memcpy(cal_event1->dtend,cal_event->dtend, strlen(cal_event->dtend));       // stores the address of datend from previous event to new event
                memcpy(cal_event1->summary,cal_event->summary, strlen(cal_event->summary)); // stores the address of summary from previous event to new event
                memcpy(cal_event1->location,cal_event->location, strlen(cal_event->location)); // stores the address of location from previous event to new event
                list = add_inorder(list, new_node(cal_event1));                             // adds the new event to the list
                count++;                        
                dt_increment(repetition_date, cal_event1->dtstart, 7);
            }
        }
        }       
    }
     fclose(fileinfo);
    return list;
}

/*
* Function formatting_time:
* 
* The function takes in ical format of the time 
* It returns the hours and mintues of the given time to appropriate format 
*/
void formatting_time(char input_time[MAX_LINE_LEN], char *output_time){
    int hours = 0;                                              // using the variables to store hours
    int minutes = 0;                                            // using the variables to store minutes
    char hours_string[MAX_LINE_LEN]; hours_string[0] = '\0';    // using the variables to store hours with AM or PM 
    char mins_string[MAX_LINE_LEN]; mins_string[0] = '\0';      // using the variables to store minutes with AM or PM
    char blank_space[MAX_LINE_LEN] = " ";                       // to add space in the output time
    output_time[0] = '\0'; output_time[MAX_LINE_LEN-1] = '\0';  
    char am_or_pm_array[MAX_LINE_LEN]; am_or_pm_array[0] = '\0'; strcpy(am_or_pm_array," AM");
    sscanf(input_time,"%2d%2d",&hours,&minutes);

    if(hours>=12) strcpy(am_or_pm_array," PM");                 // if hours is greater than equal to then 'PM' is assigned to it
    if(hours>12) hours-=12;                                     // assigns the PM value to hours_string
    sprintf(hours_string,"%d",hours);
    if(hours<10) strcat(output_time,strcat(blank_space,hours_string));  // if hours is greater than equal to then 'AM' is assigned to it
    if(hours>=10)strcat(output_time,hours_string);              // assigns the AM value to hours_string

    strcat(output_time,":");
    sprintf(mins_string,"%d",minutes);
    if(minutes<10){                                             // if the minutes are less than 10 then a 0 is added before to print it in eg 05 format
        strcat(output_time,"0");
        strcat(output_time,mins_string);
    }
    if(minutes>=10)strcat(output_time,mins_string);             // if the minutes are greater than 10 then mins string if printed in normal format
    strcat(output_time,am_or_pm_array);
}

/*
* Function print_last_line
* 
* Prints the last line of an event 
* eg: 
* ' 6:10 PM to  9:10 PM: Seattle Mariners vs Baltimore Orioles {{Seattle WA}}'
*/
void print_last_line(node_t *curr) {
    
    char *date_start = curr->val->dtstart;              // date_start stores the dtstart value of the event
    char *date_end = curr->val->dtend;                  // date_end stores the dtstart value of the event
    char output_start[MAX_LINE_LEN];                    // output_start will store starting time of the event which is in printable format
    char output_end[MAX_LINE_LEN];                      // output_end will store ending time of the event which is in printable format
    char time_start[MAX_LINE_LEN];                      // time_start contains the ical format of the date
    char time_end[MAX_LINE_LEN];                        // time_end contains the ical format of the date

    strncpy(time_start, &date_start[9], MAX_LINE_LEN);
    time_start[MAX_LINE_LEN] = '\0';
    strncpy(time_end, &date_end[9], MAX_LINE_LEN);
    time_end[MAX_LINE_LEN] = '\0';

    formatting_time(time_start, output_start);          // formatting time converts the ical format of date to printable format for starting time of event
    formatting_time(time_end, output_end);              // formatting time converts the ical format of date to printable format for endind time of event
    printf("%s to %s: %s {{%s}}\n",output_start, output_end, curr->val->summary, curr->val->location);      // prints the last time accordinf to given format
}

/*
* Function print_output
* 
* prints the output as per the given specifications
* 
* makes a call to function print_last_line to print the final line
* also makes a call to dt_format to convert the ical format of the date to a more human readable format
*/
void print_output(node_t *output_list, int from_y, int from_m, int from_d,
    int to_y, int to_m, int to_d, int count) {

    char from_date[MAX_LINE_LEN] = "";                      // from_date stores the start given by the user
    char to_date[MAX_LINE_LEN] = "";                        // to_date stores the end given by the user
    int length = 0;                                         // to store length of the printed date so that length number of '-'s can be printed 
    int checker = 0;                                        // checker is there to print an extra line after printing an event
    char m1_space[2] = "";
    char m2_space[2] = "";
    char m3_space[2] = "";
    char m4_space[2] = "";
    if (from_m < 10) strcpy(m1_space, "0");                 // to add space before hours when hour is between 1 and 9 hours  
    if (from_d < 10) strcpy(m2_space, "0");                 // to add space before minute when minute is between 1 and 9 hours  
    if (to_m < 10)  strcpy(m3_space, "0");                  // to add space before hours when minute is between 1 and 9 hours
    if (to_d < 10) strcpy(m4_space, "0");                   // to add space before minute when minute is between 1 and 9 hours  

    sprintf(from_date, "%d%s%d%s%d", from_y, m1_space, from_m, m2_space, from_d);   // add data to from_date varaible 
    sprintf(to_date, "%d%s%d%s%d", to_y, m3_space, to_m, m4_space, to_d);           // add data to to_date varaible 
    
    node_t *curr;                                           // curr stores the node values from the linked list 
    char date[MAX_LINE_LEN];                                
    date[0]='\0';

    for (curr = output_list; curr != NULL; curr = curr->next) {
        if ((strncmp(from_date, curr->val->dtstart, 8) <= 0) && (strncmp(to_date, curr->val->dtstart, 8) >= 0)) {   // makes a comparison of datestart in the event 
                                                                                                                    // to print the values between from_date and to_date 
            char formatted_time[MAX_LINE_LEN];
            if (strncmp(date,curr->val->dtstart,8) != 0) {
                if (checker != 0) {
                    printf("\n");
                }
                strcpy(date,curr->val->dtstart);
                dt_format(formatted_time, curr->val->dtstart, MAX_LINE_LEN);        // dt_format converts the ical format to a more human readable format
                printf("%s\n", formatted_time);                                     // prints the date according 'May 04, 2021 (Tue)' format
                checker = 1;
                length = strlen(formatted_time);
                int j;
                for (j = 0; j < length; j++) { printf("-"); }                       // prints the dashes after the time format
                printf("\n");
            }
            print_last_line(curr);
        }
    }
 }

int main(int argc, char *argv[]) {   
    int from_y = 0, from_m = 0, from_d = 0;         
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i;
    int count = 0;

    for (i = 0; i < argc; i++) {                    // reads the input given by the user in argv
        if (strncmp(argv[i], "--start=", 8) == 0) {   sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);   }
        else if (strncmp(argv[i], "--end=", 6) == 0) {   sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);   }
        else if (strncmp(argv[i], "--file=", 7) == 0) {   filename = argv[i] + 7;   }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {                             // if any of the input given by the user is wrong it exits
        fprintf(stderr,
                "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
                argv[0]);
        exit(1);
    }
    
    node_t *output_list = input_read(filename, count);                              // input reads the file contents and stores the event in linked list and returns that list
    print_output(output_list, from_y, from_m, from_d, to_y, to_m, to_d, count);     // print_output prints the whole output

    node_t *temp_n = NULL;                                                          
    for ( ; output_list != NULL; output_list = temp_n) {
        temp_n = output_list->next;
        free((event_t*)output_list->val);                                           // frees the event_t stores in the node
        free((node_t*)output_list);                                                 // frees the node_t of the linked list
    }
    exit(0);
}
