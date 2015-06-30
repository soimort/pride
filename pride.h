/*
  Pride -- a minimal threaded unit testing framework for C
*/
#ifndef _PRIDE_H
#define _PRIDE_H

#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Internal constraints.
 */
#define MAX_PREFIX_STRLEN            64
#define MAX_LOGFMT_STRLEN            256
#define MAX_THREAD_NUM               256

/* SGR (Select Graphic Rendition) Parameters.
 * Reference:
 *   Standard ECMA-48 (Control Functions for Coded Character Sets)
 *   <http://www.ecma-international.org/publications/standards/Ecma-048.htm>
 *   ANSI escape code
 *   <https://en.wikipedia.org/wiki/ANSI_escape_code>
 */
#define SGR(code)                    "\33["code"m"
#define SGR_RESET                    SGR("0")
#define SGR_BOLD                     SGR("1")
#define SGR_FAINT                    SGR("2")   // not widely supported
#define SGR_ITALICIZED               SGR("3")   // not widely supported
#define SGR_UNDERLINED               SGR("4")
#define SGR_BLINK                    SGR("5")
#define SGR_BLINK_RAPID              SGR("6")   // not widely supported
#define SGR_INVERSE                  SGR("7")
#define SGR_INVISIBLE                SGR("8")   // not widely supported
#define SGR_CROSSED_OUT              SGR("9")   // not widely supported
#define SGR_DOUBLY_UNDERLINED        SGR("21")  // not widely supported
#define SGR_NORMAL                   SGR("22")  // i.e. neither bold nor faint
#define SGR_NOT_ITALICIZED           SGR("23")
#define SGR_NOT_UNDERLINED           SGR("24")
#define SGR_STEADY                   SGR("25")  // i.e. not blinking
#define SGR_POSITIVE                 SGR("27")  // i.e. not inverse
#define SGR_VISIBLE                  SGR("28")
#define SGR_NOT_CROSSED_OUT          SGR("29")
#define SGR_BLACK                    SGR("30")
#define SGR_RED                      SGR("31")
#define SGR_GREEN                    SGR("32")
#define SGR_YELLOW                   SGR("33")
#define SGR_BLUE                     SGR("34")
#define SGR_MAGENTA                  SGR("35")
#define SGR_CYAN                     SGR("36")
#define SGR_WHITE                    SGR("37")
#define SGR_256(s)                   SGR("38;5;"s)
#define SGR_DEFAULT                  SGR("39")
#define SGR_BACKGROUND_BLACK         SGR("40")
#define SGR_BACKGROUND_RED           SGR("41")
#define SGR_BACKGROUND_GREEN         SGR("42")
#define SGR_BACKGROUND_YELLOW        SGR("43")
#define SGR_BACKGROUND_BLUE          SGR("44")
#define SGR_BACKGROUND_MAGENTA       SGR("45")
#define SGR_BACKGROUND_CYAN          SGR("46")
#define SGR_BACKGROUND_WHITE         SGR("47")
#define SGR_BACKGROUND_256(s)        SGR("48;5;"s)
#define SGR_BACKGROUND_DEFAULT       SGR("49")
// 16-color support: aixterm colors are the bright versions of ISO colors
#define SGR_LIGHT_BLACK              SGR("90")  // i.e. dark gray
#define SGR_LIGHT_RED                SGR("91")
#define SGR_LIGHT_GREEN              SGR("92")
#define SGR_LIGHT_YELLOW             SGR("93")
#define SGR_LIGHT_BLUE               SGR("94")
#define SGR_LIGHT_MAGENTA            SGR("95")
#define SGR_LIGHT_CYAN               SGR("96")
#define SGR_LIGHT_WHITE              SGR("97")
#define SGR_BACKGROUND_LIGHT_BLACK   SGR("100") // i.e. dark gray
#define SGR_BACKGROUND_LIGHT_RED     SGR("101")
#define SGR_BACKGROUND_LIGHT_GREEN   SGR("102")
#define SGR_BACKGROUND_LIGHT_YELLOW  SGR("103")
#define SGR_BACKGROUND_LIGHT_BLUE    SGR("104")
#define SGR_BACKGROUND_LIGHT_MAGENTA SGR("105")
#define SGR_BACKGROUND_LIGHT_CYAN    SGR("106")
#define SGR_BACKGROUND_LIGHT_WHITE   SGR("107")

/* Type Definitions */

typedef void *p_unit;

/* Global Variables */

struct {
    bool sgr;
    bool sgr_f;
    bool p_stdout;                // true if use stdout for normal messages
    bool v_stdout;                // true if use stdout for VERBOSE messages
    bool d_stdout;                // true if use stdout for DEBUG messages
    bool i_stdout;                // true if use stdout for INFO messages
    bool w_stdout;                // true if use stdout for WARN messages
    bool e_stdout;                // true if use stdout for ERROR messages
    bool wtf_stdout;              // true if use stdout for WTF messages
    char p_prefix[MAX_PREFIX_STRLEN + 1];
    char v_prefix[MAX_PREFIX_STRLEN + 1];
    char d_prefix[MAX_PREFIX_STRLEN + 1];
    char i_prefix[MAX_PREFIX_STRLEN + 1];
    char w_prefix[MAX_PREFIX_STRLEN + 1];
    char e_prefix[MAX_PREFIX_STRLEN + 1];
    char wtf_prefix[MAX_PREFIX_STRLEN + 1];
} LOG_SETTINGS = {
    .sgr        = true,
    .sgr_f      = false,
    .p_stdout   = false,
    .v_stdout   = false,
    .d_stdout   = false,
    .i_stdout   = false,
    .w_stdout   = false,
    .e_stdout   = false,
    .wtf_stdout = false,
    .p_prefix   = SGR_RESET,
    .v_prefix   = SGR_RESET,
    .d_prefix   = SGR_BLUE,
    .i_prefix   = SGR_GREEN SGR_BOLD,
    .w_prefix   = SGR_YELLOW SGR_BOLD,
    .e_prefix   = SGR_RED SGR_BOLD,
    .wtf_prefix = SGR_RED SGR_BOLD,
};

struct {
    int  thread_num;
    char unit_fmt[MAX_LOGFMT_STRLEN + 1];
    char assertion_fmt[MAX_LOGFMT_STRLEN + 1];
    char assertion_passed_str[MAX_LOGFMT_STRLEN + 1];
    char assertion_failed_str[MAX_LOGFMT_STRLEN + 1];
} P_SETTINGS = {
    .thread_num           = 0,
    .unit_fmt             = "---\tStart testing: %s\n",
    .assertion_fmt        = "[%s]\t%s\n",
    .assertion_passed_str = "✔",
    .assertion_failed_str = "✘",
};

struct {
    pthread_t threads[MAX_THREAD_NUM + 1];
    int       thread_index;
    int       unit_num;
    int       assertion_num;
    int       assertion_passed_num;
    int       assertion_failed_num;
} P_STATUS = {
    .threads              = {NULL},
    .thread_index         = 0,
    .unit_num             = 0,
    .assertion_num        = 0,
    .assertion_passed_num = 0,
    .assertion_failed_num = 0,
};

/* Functions: Logging */

bool log_set_sgr(bool t)
{
    return LOG_SETTINGS.sgr = t;
}

bool log_set_sgr_f(bool t)
{
    return LOG_SETTINGS.sgr_f = t;
}

bool log_set_p_stdout(bool t)
{
    return LOG_SETTINGS.p_stdout = t;
}

bool log_set_v_stdout(bool t)
{
    return LOG_SETTINGS.v_stdout = t;
}

bool log_set_d_stdout(bool t)
{
    return LOG_SETTINGS.d_stdout = t;
}

bool log_set_i_stdout(bool t)
{
    return LOG_SETTINGS.i_stdout = t;
}

bool log_set_w_stdout(bool t)
{
    return LOG_SETTINGS.w_stdout = t;
}

bool log_set_e_stdout(bool t)
{
    return LOG_SETTINGS.e_stdout = t;
}

bool log_set_wtf_stdout(bool t)
{
    return LOG_SETTINGS.wtf_stdout = t;
}

char *log_set_p_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.p_prefix, prefix);
    }
    return NULL;
}

char *log_set_v_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.v_prefix, prefix);
    }
    return NULL;
}

char *log_set_d_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.d_prefix, prefix);
    }
    return NULL;
}

char *log_set_i_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.i_prefix, prefix);
    }
    return NULL;
}

char *log_set_w_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.w_prefix, prefix);
    }
    return NULL;
}

char *log_set_e_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.e_prefix, prefix);
    }
    return NULL;
}

char *log_set_wtf_prefix(const char *prefix)
{
    if (strlen(prefix) <= MAX_PREFIX_STRLEN) {
        return strcpy(LOG_SETTINGS.wtf_prefix, prefix);
    }
    return NULL;
}

/* Print a prefixed message from variable argument list to string.
 */
int log_vsprintf(char *buf, const char *prefix, const char *format, va_list ap)
{
    char logfmt_s[MAX_LOGFMT_STRLEN + 1], *logfmt = logfmt_s;
    int logfmt_strlen = strlen(prefix) + strlen(format) + strlen(SGR_RESET);
    if (logfmt_strlen > MAX_LOGFMT_STRLEN) {
        logfmt = malloc(logfmt_strlen + 1);
    }
    if (LOG_SETTINGS.sgr) {
        strcpy(logfmt, prefix);
    } else {
        for (int i = 0, j = 0, is_sgr = false; i <= strlen(prefix); i++) {
            if (!is_sgr) {
                if (prefix[i] == 033) {
                    is_sgr = true;
                } else {
                    logfmt[j++] = prefix[i];
                }
            } else if (prefix[i] == 'm') {
                is_sgr = false;
            }
        }
    }
    strcat(strcat(logfmt, format), SGR_RESET);
    int ret = vsprintf(buf, logfmt, ap);
    if (logfmt_strlen > MAX_LOGFMT_STRLEN) {
        free(logfmt);
    }
    return ret;
}

/* Print a prefixed message to string.
 */
int log_sprintf(char *buf, const char *prefix, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vsprintf(buf, prefix, format, args);
    va_end(args);
    return ret;
}

/* Print a prefixed message from variable argument list to stream.
 */
int log_fvprintf(FILE *stream, const char *prefix, const char *format, va_list ap)
{
    char logfmt_s[MAX_LOGFMT_STRLEN + 1], *logfmt = logfmt_s;
    int logfmt_strlen = strlen(prefix) + strlen(format) + strlen(SGR_RESET);
    if (logfmt_strlen > MAX_LOGFMT_STRLEN) {
        logfmt = malloc(logfmt_strlen + 1);
    }
    bool is_tty = isatty(fileno(stream));
    if ((is_tty && LOG_SETTINGS.sgr) || (!is_tty && LOG_SETTINGS.sgr_f)) {
        strcpy(logfmt, prefix);
    } else {
        for (int i = 0, j = 0, is_sgr = false; i <= strlen(prefix); i++) {
            if (!is_sgr) {
                if (prefix[i] == 033) {
                    is_sgr = true;
                } else {
                    logfmt[j++] = prefix[i];
                }
            } else if (prefix[i] == 'm') {
                is_sgr = false;
            }
        }
    }
    strcat(strcat(logfmt, format), SGR_RESET);
    int ret = vfprintf(stream, logfmt, ap);
    if (logfmt_strlen > MAX_LOGFMT_STRLEN) {
        free(logfmt);
    }
    return ret;
}

/* Print a prefixed message to stream.
 */
int log_fprintf(FILE *stream, const char *prefix, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvprintf(stream, prefix, format, args);
    va_end(args);
    return ret;
}

/* Print a normal log message from variable argument list to stream.
 */
int log_fvp(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.p_prefix, format, ap);
}

/* Print a VERBOSE log message from variable argument list to stream.
 */
int log_fvv(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.v_prefix, format, ap);
}

/* Print a DEBUG log message from variable argument list to stream.
 */
int log_fvd(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.d_prefix, format, ap);
}

/* Print an INFO log message from variable argument list to stream.
 */
int log_fvi(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.i_prefix, format, ap);
}

/* Print a WARN log message from variable argument list to stream.
 */
int log_fvw(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.w_prefix, format, ap);
}

/* Print an ERROR log message from variable argument list to stream.
 */
int log_fve(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.e_prefix, format, ap);
}

/* What a Terrible Failure: print a WTF message from variable argument list to stream.
 */
int log_fvwtf(FILE *stream, const char *format, va_list ap)
{
    return log_fvprintf(stream, LOG_SETTINGS.wtf_prefix, format, ap);
}

/* Print a normal log message from variable argument list.
 */
int log_vp(const char *format, va_list ap)
{
    return log_fvp(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* Print a VERBOSE log message from variable argument list.
 */
int log_vv(const char *format, va_list ap)
{
    return log_fvv(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* Print a DEBUG log message from variable argument list.
 */
int log_vd(const char *format, va_list ap)
{
    return log_fvd(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* Print an INFO log message from variable argument list.
 */
int log_vi(const char *format, va_list ap)
{
    return log_fvi(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* Print a WARN log message from variable argument list.
 */
int log_vw(const char *format, va_list ap)
{
    return log_fvw(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* Print an ERROR log message from variable argument list.
 */
int log_ve(const char *format, va_list ap)
{
    return log_fve(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* What a Terrible Failure: print a WTF message from variable argument list.
 */
int log_vwtf(const char *format, va_list ap)
{
    return log_fvwtf(LOG_SETTINGS.p_stdout ? stdout : stderr, format, ap);
}

/* Print a normal log message to stream.
 */
int log_fp(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvp(stream, format, args);
    va_end(args);
    return ret;
}

/* Print a VERBOSE log message to stream.
 */
int log_fv(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvv(stream, format, args);
    va_end(args);
    return ret;
}

/* Print a DEBUG log message to stream.
 */
int log_fd(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvd(stream, format, args);
    va_end(args);
    return ret;
}

/* Print an INFO log message to stream.
 */
int log_fi(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvi(stream, format, args);
    va_end(args);
    return ret;
}

/* Print a WARN log message to stream.
 */
int log_fw(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvw(stream, format, args);
    va_end(args);
    return ret;
}

/* Print an ERROR log message to stream.
 */
int log_fe(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fve(stream, format, args);
    va_end(args);
    return ret;
}

/* What a Terrible Failure: print a WTF message to stream.
 */
int log_fwtf(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_fvwtf(stream, format, args);
    va_end(args);
    return ret;
}

/* Print a normal log message.
 */
int log_p(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vp(format, args);
    va_end(args);
    return ret;
}

/* Print a VERBOSE log message.
 */
int log_v(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vv(format, args);
    va_end(args);
    return ret;
}

/* Print a DEBUG log message.
 */
int log_d(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vd(format, args);
    va_end(args);
    return ret;
}

/* Print an INFO log message.
 */
int log_i(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vi(format, args);
    va_end(args);
    return ret;
}

/* Print a WARN log message.
 */
int log_w(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vw(format, args);
    va_end(args);
    return ret;
}

/* Print an ERROR log message.
 */
int log_e(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_ve(format, args);
    va_end(args);
    return ret;
}

/* What a Terrible Failure: print a WTF message.
 */
int log_wtf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = log_vwtf(format, args);
    va_end(args);
    return ret;
}

/* Functions: Unit Testing */

int p_set_thread_num(int thread_num)
{
    return P_SETTINGS.thread_num = thread_num;
}

/* Make an assertion.
 */
void p_assert(const char *assertion_msg, bool assertion_t)
{
    P_STATUS.assertion_num++;
    if (assertion_t) {
        // assertion passed
        P_STATUS.assertion_passed_num++;
        log_i(P_SETTINGS.assertion_fmt, P_SETTINGS.assertion_passed_str, assertion_msg);
    } else {
        // assertion failed
        P_STATUS.assertion_failed_num++;
        log_e(P_SETTINGS.assertion_fmt, P_SETTINGS.assertion_failed_str, assertion_msg);
    }
}

/* Start a testing unit.
 */
void p_test(const char *unit_msg, p_unit (*unit_func)())
{
    log_p(P_SETTINGS.unit_fmt, unit_msg);
    if (P_SETTINGS.thread_num) {
        // threaded
        if (P_STATUS.threads[P_STATUS.thread_index]) {
            pthread_join(P_STATUS.threads[P_STATUS.thread_index], NULL);
        }
        pthread_create(&P_STATUS.threads[P_STATUS.thread_index], NULL,
                       *unit_func, (void *)unit_msg);
        P_STATUS.thread_index = (P_STATUS.thread_index + 1) % P_SETTINGS.thread_num;
    } else {
        // non-threaded
        P_STATUS.unit_num++;
        P_STATUS.assertion_num = 0;
        unit_func();
    }
}

/* Finish all testing units.
 */
void p_finish()
{
    for (int i = 0; i < P_SETTINGS.thread_num; i++) {
        if (P_STATUS.threads[i]) {
            pthread_join(P_STATUS.threads[i], NULL);
        }
    }
}

#endif /* _PRIDE_H */
