/* 
* File: index.h
* Author: Edward Hills
* Date: 23/03/2012
* Description: Header file for index.c. Defines all method signatures.
*/

extern void begin_indexing(void);
extern void end_indexing(void);
extern void start_tag(char const *);
extern void end_tag(char const *);
extern void word(char const *);

// end index.h
