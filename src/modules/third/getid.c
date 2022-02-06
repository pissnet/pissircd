/*
 *   IRC - Internet Relay Chat, src/modules/third/getid.c
 *   (C) 2022 Noisytoot
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

#define MSG_GETID "GETID"

CMD_FUNC(cmd_getid);

ModuleHeader MOD_HEADER
  = {
	"third/getid",
	"1.0",
	"Adds a GETID command to resolve a nick or server name to a UID or SID",
	"Noisytoot",
	"unrealircd-6",
	};

MOD_INIT()
{
	CommandAdd(modinfo->handle, MSG_GETID, cmd_getid, 2, CMD_USER);
	return MOD_SUCCESS;
}

MOD_LOAD()
{
	return MOD_SUCCESS;
}

MOD_UNLOAD()
{
	return MOD_SUCCESS;
}

/*
** cmd_getid
**	parv[1] = client
**	parv[2] = optional nonce
*/
CMD_FUNC(cmd_getid)
{
	const char *target_name;
	Client *target;
	const char *nonce;

	if (!IsOper(client))
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;
	}

	if ((parc < 1) || BadPtr(parv[1]))
	{
		sendnumeric(client, ERR_NEEDMOREPARAMS, MSG_GETID);
		return;
	}

	target_name = parv[1];

	if (!(target = find_client(target_name, NULL)))
	{
		sendnumeric(client, ERR_NOSUCHNICK, target_name);
		return;
	}

	if ((parc < 2) || BadPtr(parv[2]))
	{
		nonce = NULL;
	} else
	{
		nonce = parv[2];
	}

	if (nonce)
	{
		sendnotice(client, "%s: %s is %s, nonce: %s", MSG_GETID, target->name, target->id, nonce);
	} else
	{
		sendnotice(client, "%s: %s is %s", MSG_GETID, target->name, target->id);
	}
}
