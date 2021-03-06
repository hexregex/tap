/*  This file (termios_input.c) is part of the ta application.
    Copyright (C) 2016-2017 Aaron Calder

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include "log.h"
#include "input.h"

static struct termios tios;
static struct termios other;

/* Not a part of POSIX so implementing here.
 * Code straight from the Linux man page: termios(3). */
static inline void cfmakeraw(struct termios *termios_p)
{
           termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                                   | INLCR | IGNCR | ICRNL | IXON);
           termios_p->c_oflag &= ~OPOST;
           termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
           termios_p->c_cflag &= ~(CSIZE | PARENB);
           termios_p->c_cflag |= CS8;
}

void tios_init()
{
    log_write("tios_init--start");
    tcgetattr(STDIN_FILENO, &other);
    tios = other;
    cfmakeraw(&tios);
    tcsetattr(STDIN_FILENO, TCSANOW, &tios);
    log_write("tios_init--end");
}

/* Decode a string of characters generated by the terminal when a key is
 * pressed.  Return the enumerated value of which key was pressed. */
static inline
InCode in_str_to_code(const char *in_str)
{
    int str_len = strlen(in_str);
    /* Raw terminal input sends ESC then '[' to introduce control sequences.
     * Decode these escape sequences to capture cursor movement keypresses. */
    if (str_len >= 3
        && in_str[0] == 27 /* ESC */
        && in_str[1] == '[')
    {
        switch (in_str[2])
        {
            case 'A': return UP;
            case 'B': return DOWN;
            case 'C': return RIGHT;
            case 'D': return LEFT;
            default : return NOP;
        }
    }
    else if (str_len == 1)
    {   /* Get keypresses from keys which send single characters. */
        switch (in_str[0])
        {
            case ' ': return SPACE;
            case 27 : return ESC;
            case ';':
            case ':': return SEMICOLON;
            case 'h':
            case 'H': return H;
            case 'j':
            case 'J': return J;
            case 'k':
            case 'K': return K;
            case 'l':
            case 'L': return L;
            case 'q':
            case 'Q': return Q;
            default : return NOP;
        }
    }
    return NOP;
}

InCode tios_keypress()
{
    /* TODO: Fix bug.  If multiple single character keypresses make it to
     * stdin before the first keypress can be read the algorithm will treat
     * them as a single multi-character keypress. Need to check to make
     * sure that multiple characters in stdin are not individual keypresses
     * before processing as a single keypress. */

    char string[8];
    memset(string, 0, 8);
    int old_file_flags = fcntl(STDIN_FILENO, F_GETFL);
    /* Make sure that read blocking is already on. */
    fcntl(STDIN_FILENO, F_SETFL, old_file_flags && ~O_NONBLOCK);
    /* Wait to read one character from stdin. */
    read(STDIN_FILENO, string, 1);
    /* Set file status flags on stdin so read() won't wait for input. */
    int new_file_flags = old_file_flags | O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, new_file_flags);
    /* Read stdin if there is input else continue. */
    read(STDIN_FILENO, string + 1, 3);
    /* Reset file flags for stdin. */
    fcntl(STDIN_FILENO, F_SETFL, old_file_flags);

    log_write(string);

    return in_str_to_code(string);
}

void tios_clean()
{
    /* Restore terminal too original state. */
    tcsetattr(STDIN_FILENO, TCSANOW, &other);
}
