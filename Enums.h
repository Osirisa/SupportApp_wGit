// advertiserdata.h
#ifndef ENUMS_H
#define ENUMS_H



enum suppNetStatus{
    eNstat_NotSend      = 0,
    eNstat_Good         = 1,
    eNstat_Okay         = 2,
    eNstat_Error        = 3
};

enum sortingStatus{
    eSStat_None,
    eSStat_Ascending,
    eSStat_Descending
};

enum toggleTable{
    eTT_Small,
    eTT_Normal,
    eTT_Big
};

#endif  // ENUMS_H
