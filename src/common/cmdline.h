/*
 * Command line public interface.
 */
#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#include "strgs.h"

typedef enum {
    // place holder
    CMD_NONE = 0,

    // needed by getarg()
    CMD_ARGS = 0x01, // args required

    // data type helpers
    CMD_STR = 0x02,  // type is a string
    CMD_NUM = 0x04,  // type is a number
    CMD_BOOL = 0x08, // type is bool, requires "true" or "false"
    CMD_LIST = 0x10, // a list is accepted by the arg. implies CMD_ARGS.
    CMD_ANON = 0x20, // an anonymous list

    // arg attributes
    CMD_REQD = 0x40, // item is required
    CMD_DIV = 0x80,  // item is a divider for the help screen

    // internal flags, do not use
    CMD_SEEN = 0x100,
} cmdline_type_t;

typedef void (*cmdline_callback_t)(void);

void init_cmdline(const char* name,
                  const char* preamble,
                  const char* vers);

void add_cmdline(int short_opt,
                 const char* long_opt,
                 const char* name,
                 const char* help,
                 const char* def_val,
                 cmdline_callback_t cb,
                 cmdline_type_t type);

void destroy_cmdline(void);
void parse_cmdline(int argc, char** argv);
const string_t* get_cmd_opt(const char* name);
const string_t* iterate_cmd_opt(const char* name, int* mark);

void cb_cmdline_help(void);
void cb_cmdline_vers(void);

#endif /* _CMDLINE_H_ */
