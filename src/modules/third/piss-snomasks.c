/*
 *   IRC - Internet Relay Chat, src/modules/third/piss-snomasks.c
 *   (C) 2021 Noisytoot
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "unrealircd.h"

ModuleHeader MOD_HEADER
		= {
				"third/piss-snomasks",
				"1.0",
				"Adds extra snomasks for pissnet",
				"Noisytoot",
				"unrealircd-5",
		};

long SNO_RANDGEN = 0L;

MOD_INIT()
{
	return MOD_SUCCESS;
}

MOD_LOAD()
{
	SnomaskAdd(NULL, 'R', umode_allow_opers, &SNO_RANDGEN);
	return MOD_SUCCESS;
}

MOD_UNLOAD()
{
	return MOD_SUCCESS;
}
