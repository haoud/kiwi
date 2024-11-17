/**
 * Copyright (C) 2024 Romain CADILHAC
 *
 * This file is part of Kiwi
 *
 * Kiwi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kiwi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kiwi. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <kernel.h>

#define LOG_MAX_LEN             256

#define LOG_LEVEL_UNDEFINED     0
#define LOG_LEVEL_TRACE         1
#define LOG_LEVEL_DEBUG         2
#define LOG_LEVEL_INFO          3
#define LOG_LEVEL_WARN          4
#define LOG_LEVEL_ERROR         5
#define LOG_LEVEL_FATAL         6

#define log(fmt, ...)    loglevel(LOG_LEVEL_UNDEFINED, fmt, ##__VA_ARGS__)

#define trace(fmt, ...)                                 \
    loglevel(LOG_LEVEL_TRACE, "%s " fmt "\n",           \
        level_icon_colored[LOG_LEVEL_TRACE],            \
        ##__VA_ARGS__)

#define debug(fmt, ...)                                 \
    loglevel(LOG_LEVEL_DEBUG, "%s " fmt "\n",           \
        level_icon_colored[LOG_LEVEL_DEBUG],            \
        ##__VA_ARGS__)

#define info(fmt, ...)                                  \
    loglevel(LOG_LEVEL_INFO, "%s " fmt "\n",            \
        level_icon_colored[LOG_LEVEL_INFO],             \
        ##__VA_ARGS__)

#define warn(fmt, ...)                                  \
    loglevel(LOG_LEVEL_WARN, "%s " fmt "\n",            \
        level_icon_colored[LOG_LEVEL_WARN],             \
        ##__VA_ARGS__)

#define error(fmt, ...)                                 \
    loglevel(LOG_LEVEL_ERROR, "%s " fmt "\n",           \
        level_icon_colored[LOG_LEVEL_ERROR],            \
        ##__VA_ARGS__);

#define fatal(fmt, ...)                                 \
    loglevel(LOG_LEVEL_FATAL, "%s " fmt "\n",           \
        level_icon_colored[LOG_LEVEL_FATAL],            \
        ##__VA_ARGS__)

void log_setup(void);
void loglevel(int gravity, const char *fmt, ...);

extern const char *level_icon_colored[];
extern const char *level_icon[];
