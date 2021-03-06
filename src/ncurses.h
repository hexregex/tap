/*  This file (ncurses.h) is part of the ta application.
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
#ifndef AT_NCURSES_TA
#define AT_NCURSES_TA

#include "communicate.h"
#include "output.h"

void nc_init();
void nc_dest();

void nc_play_time_str(const char *time_str);
void nc_track_list(Track *track_list, int track_count);
void nc_track(Track *track);
void nc_operation(OutCode operation);

#endif /* AT_NCURSES_TA */
