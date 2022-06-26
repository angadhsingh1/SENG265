#!/usr/bin/env python

# Angadh Singh 

import datetime
from collections import defaultdict
import re

class process_cal:
    """ process_cal does all the formatting: reading the input, appending in dict, sorting the values, storing the output in a list"""
    def __init__(self, filename):
        """ Constructor for class process_cal """
        self.filename = filename                        # filename for the whole file
        self.events_dict = defaultdict(list)            # using dictionary to store events. Each key will be a date in datetime.datetime.date() format and value is list of events
        process_cal.input_read(self, self.filename)     # calling input_read for taking input from the file

    def __repr__(self):
        """ repr function for class process_cal"""
        return "process_cal(%r)" % self.filename

    def time_format(date_time):
        """ Takes the time in the datetime format from as an argument
            returns a string containing the readable format of time """

        return_string = date_time.strftime("%-I:%M %p")

        # if the hours is single digit then return_string gets modified here
        if ( (1 <= date_time.hour < 10 ) or (12 < date_time.hour < 22)):
            return_string = date_time.strftime(" %-I:%M %p")

        return return_string

    def ical_to_datetime(year, month, date, hours = 0, minutes = 0, seconds = 0):
        """ This function takes in year, month, date, hours, minutes and seconds in integer format and 
            returns a datetime format. """
        return datetime.datetime(year, month, date, hours, minutes, seconds)

    def dt_increment(datetime_intital):
        """ The function takes in the datetime_inital in the datetime format and add a value of 7 to it; 
        returns a datetime format with 7 days added to the datetime_initials """
        return datetime_intital + datetime.timedelta(7)

    def input_read(self, filename):
        """ reads the input from the file and stores the values in dict"""
        f = open(filename, "r")
        for line in f:
            if re.findall("^BEGIN:VEVENT", line):
                event = Struct()
                
                # runs a while loop to go through each line of the file and stores the events in dict
                while not (re.findall("^END:VEVENT", line.strip())):
                    if re.findall("^DTSTART:", line.rstrip()):
                        dtst_y, dtst_m, dtst_d, dtst_h, dtst_min = int(line[8:12]), int(line[12:14]), int(line[14:16]), int(line[17:19]), int(line[19:21])
                        event.set_dtstart(process_cal.ical_to_datetime(dtst_y, dtst_m, dtst_d, dtst_h, dtst_min))

                    if re.findall("^DTEND:", line.rstrip()):
                        dtend_y, dtend_m, dtend_d, dtend_h, dtend_min = int(line[6:10]), int(line[10:12]), int(line[12:14]), int(line[15:17]), int(line[17:19])
                        event.set_dtend(process_cal.ical_to_datetime(dtend_y, dtend_m, dtend_d, dtend_h, dtend_min))

                    if re.findall("^RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=", line):
                        dtuntil_y, dtuntil_m, dtuntil_d, dtuntil_h, dtuntil_min = int(line[32:36]), int(line[36:38]), int(line[38:40]), int(line[41:43]), int(line[43:45])
                        event.set_until(process_cal.ical_to_datetime(dtuntil_y, dtuntil_m, dtuntil_d, dtuntil_h, dtuntil_min))
                        event.set_rrule(1)

                    elif re.findall("^RRULE:FREQ=WEEKLY;UNTIL=", line):
                        dtuntil_y, dtuntil_m, dtuntil_d, dtuntil_h, dtuntil_min = int(line[24:28]), int(line[28:30]), int(line[30:32]), int(line[33:35]), int(line[35:37])
                        event.set_until(process_cal.ical_to_datetime(dtuntil_y, dtuntil_m, dtuntil_d, dtuntil_h, dtuntil_min))
                        event.set_rrule(1)
                    
                    if re.findall("^SUMMARY:", line): event.set_summary(line[8:].strip())
                    if re.findall("^LOCATION:", line): event.set_location(line[9:].strip())

                    line = f.readline()

                self.events_dict[event.get_dtstart().date()].append(event)                  # appends the events in value for a given key
                if (event.get_rrule() == 1):                                                # if the event has repeating events then it enters this conditon
                    datetime_incremented = process_cal.dt_increment(event.get_dtstart())
                    last_date_until = event.get_until()

                    while (last_date_until >= datetime_incremented):
                        event1 = Struct(datetime_incremented, event.get_dtend(), event.get_until(), event.get_summary(), event.get_location(), event.get_rrule())   # creates a new instance of the Struct class to store the new element
                        self.events_dict[event1.get_dtstart().date()].append(event1)         # appends the events in value for a given key
                        datetime_incremented = process_cal.dt_increment(event1.get_dtstart())
        f.close()        
    
    def format_output_from_list(event_element_list, event_date):
        """ This function takes a list which contains the list of elements 
        it sorts the list and adds the output in appropraite form to a string
        and returns that string
        """
        output_string = ""                                                          # output string stores the final output in it 
        checker_list = []                                                           # checker list check for the duplicacy of events
        duplicate_checker = 0                                                       # duplicate_checker checks for adding a new line character after printing date with multiple events
        event_element_list.sort(key = lambda x:x.get_dtstart())                     # sorts the function according to datetime

        for event in event_element_list:
            datestart_in_list = event.get_dtstart()                                 # datestart_in_list stores the starting time of the event  
            datend_in_list = event.get_dtend()                                      # datend_in_list stores the ending time of the event
            only_date = datestart_in_list.date()                                    # stores the only date format of datestart_in_list
            duplicate_checker += 1  

            if only_date not in checker_list:                                       
                checker_list.append(only_date)
                formatted_date = datestart_in_list.strftime("%B %d, %Y (%a)")
                output_string += str(formatted_date) + "\n"
                output_string += "-"*len(formatted_date) +"\n"
            
            time_start = process_cal.time_format(datestart_in_list)                 # time_start stores the printable format of starting time
            time_end = process_cal.time_format(datend_in_list)                      # time_end stores the printable format of ending time

            output_string += time_start + " to " + time_end + ": "+ event.get_summary() + " {{" + event.get_location() + "}}"
            if (len(event_element_list) >= 2 and duplicate_checker != len(event_element_list)): output_string += "\n" 

        return output_string

    def get_events_for_day(self, event_date: datetime.datetime):
        """ Checks for events in the ics file and returns a string 
        of the event for that day
        """

        if (self.events_dict.get(event_date.date()) == None): return None                       # if there are no events found in the dict
        event_element_list = self.events_dict.get(event_date.date())                            # event_element_list gets the value for given date
        output_string = process_cal.format_output_from_list(event_element_list, event_date)     # calls the format_output_from_list function for storing the output
        return output_string
    
class Struct:
    """ This class contains all the event parameters"""
    def __repr__(self):
        return "Struct(%r, %r, %r, %r, %r, %r)" % (self.__dtstart, self.__dtend, self.__until, 
                                                    self.__summary, self.__location, self.__rrule)
        

    def __init__(self, dtstart :datetime.datetime = datetime.datetime(1000, 1, 1, 00, 00, 00), 
        dtend : datetime.datetime = datetime.datetime(1000, 1, 1, 00, 00, 00),
        until : datetime.datetime = datetime.datetime(1000, 1, 1, 00, 00, 00), 
        summary = "", location = "", 
        rrule = 0):

        self.__dtstart = dtstart
        self.__dtend = dtend
        self.__rrule = rrule
        self.__until = until
        self.__summary = summary
        self.__location  = location

    # Setting up getters and setter for the given
    def get_dtstart(self): return self.__dtstart
    def set_dtstart(self, dtstart): self.__dtstart = dtstart
    
    def get_dtend(self): return self.__dtend
    def set_dtend(self, dtend): self.__dtend = dtend
    
    def get_until(self): return self.__until
    def set_until(self, until): self.__until = until

    def get_location(self): return self.__location
    def set_location(self, location): self.__location = location

    def get_summary(self): return self.__summary
    def set_summary(self, summary): self.__summary = summary

    def get_rrule(self): return self.__rrule
    def set_rrule(self, rrule): self.__rrule = rrule
