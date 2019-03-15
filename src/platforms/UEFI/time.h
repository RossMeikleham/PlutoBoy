#ifndef GET_TIME_H
#define GET_TIME_H

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} Time;


EFI_STATUS get_time_now(Time* time);

#endif
