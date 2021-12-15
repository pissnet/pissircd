/*
 *   IRC - Internet Relay Chat, src/modules/third/nosajoinpartmode.c
 *   (C) 2021 Polsaker
 *   (C) 2021 chmod/Semisol
 *   (C) 2021 Noisytoot
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "unrealircd.h"

ModuleHeader MOD_HEADER
		= {
				"third/nosajoinpartmode",
				"1.0",
				"Disables remote SAJOIN/SAPART targeted to local users, with umode +J",
				"chmod, Polsaker, Noisytoot",
				"unrealircd-5",
		};


CMD_OVERRIDE_FUNC(override_sajoin);
CMD_OVERRIDE_FUNC(override_sapart);

long UMODE_NOSAJOINPART = 0L;

MOD_INIT()
{
	return MOD_SUCCESS;
}

MOD_LOAD()
{
	if (!CommandOverrideAdd(modinfo->handle, "SAJOIN", override_sajoin))
		return MOD_FAILED;
	if (!CommandOverrideAdd(modinfo->handle, "SAPART", override_sapart))
		return MOD_FAILED;
	UmodeAdd(modinfo->handle, 'J', UMODE_LOCAL, 1, umode_allow_opers, &UMODE_NOSAJOINPART);
	return MOD_SUCCESS;
}

MOD_UNLOAD()
{
	return MOD_SUCCESS;
}

CMD_OVERRIDE_FUNC(override_sajoin)
{
	Client *target;
	if ((parc < 3) || BadPtr(parv[2]))
	{
		sendnumeric(client, ERR_NEEDMOREPARAMS, "SAJOIN");
		return;
	}

	if (!(target = find_person(parv[1], NULL)))
	{
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	if (MyUser(target) && !MyUser(client) && (target->umodes & UMODE_NOSAJOINPART))
	{
		sendnumeric(client, ERR_CANNOTDOCOMMAND, "SAJOIN", "User has umode +J, and you are not on the same server as them");
		return;
	}

	CallCommandOverride(ovr, client, recv_mtags, parc, parv);
}

CMD_OVERRIDE_FUNC(override_sapart)
{
	Client *target;
	if ((parc < 3) || BadPtr(parv[2]))
	{
		sendnumeric(client, ERR_NEEDMOREPARAMS, "SAPART");
		return;
	}

	if (!(target = find_person(parv[1], NULL)))
	{
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	if (MyUser(target) && !MyUser(client) && (target->umodes & UMODE_NOSAJOINPART))
	{
		sendnumeric(client, ERR_CANNOTDOCOMMAND, "SAPART", "User has umode +J, and you are not on the same server as them");
		return;
	}

	CallCommandOverride(ovr, client, recv_mtags, parc, parv);
}
