#ifndef DEF_H
#define DEF_H

/* LOOP ERROR CODE
 * Program err loop return code
*/
#define _NONE 0
#define _FILE 1
#define _FILE_FORMAT 2

#define _FILE_STR(s) \
	fprintf(stderr, "[ft_nm]: Cannot find the file `%s`\n", s);
#define _FILE_FORMAT_STR(s) \
	fprintf(stderr, "[ft_nm]: Cannot recognize the file `%s`\n", s);

/* INTEGER DEFS
 * When i need to simplify integer integrations
*/ 
#define MIN_ARGS 2

/* PRINT STR
 * When i need to pass str to my perr or pout
*/ 
#define ERR_MIN_ARGS \
	fprintf(stderr, "[ft_nm]: Specify at least one binary to list its symbols\n");
#endif
