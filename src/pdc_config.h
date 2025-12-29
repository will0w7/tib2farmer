/*
    This header is needed to avoid conflict between MOUSE_MOVED from Windows.h and MOUSE_MOVED from curses.h
    At the same time, configures curses.h to use wide characteres and UTF-8
    It must be included after Windows.h and before curses.h
*/
#ifndef PDC_CONFIG_H
#define PDC_CONFIG_H

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif
#define PDC_WIDE 1
#define PDC_FORCE_UTF8 1

#endif
