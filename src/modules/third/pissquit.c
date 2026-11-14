/*
 *   IRC - Internet Relay Chat, src/modules/third/pissquit.c
 *   (C) 2004-2021 The UnrealIRCd Team, pissnet team
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

#define MSG_PISSQUIT "PISSQUIT"

CMD_FUNC(cmd_pissquit);

ModuleHeader MOD_HEADER = {
    "third/pissquit",
    "0.02",
    "command /pissquit: Positively Identify Server to Server QUIT",
    "pissnet team",
    "unrealircd-6",
};

MOD_INIT()
{
	CommandAdd(modinfo->handle, MSG_PISSQUIT, cmd_pissquit, 3, CMD_USER);
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
** cmd_pissquit
**	parv[1] = server to disconnect from its uplink
**	parv[2] = remote server that server needs to be disconnected from
**	parv[parc-1] = comment
*/
CMD_FUNC(cmd_pissquit)
{
	const char *server, *server_remote;
	Client *target, *target_remote;
	const char *comment = (parc > 3 && parv[parc - 1]) ? parv[parc - 1] : client->name;

	/* This permissions check is only intended as a quick filter. The SQUIT
	 * code will do a proper check later. */
	if (!ValidatePermissionsForPath("route:local", client, NULL, NULL, NULL))
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;
	}

	if ((parc < 3) || BadPtr(parv[1]) || BadPtr(parv[2]))
	{
		sendnumeric(client, ERR_NEEDMOREPARAMS, MSG_PISSQUIT);
		return;
	}

	server = parv[1];
	server_remote = parv[2];

	target = find_server_quick(server);
	target_remote = find_server_quick(server_remote);

	if (target && IsMe(target))
	{
		sendnotice(client, "ERROR: Unable to %s myself.", MSG_PISSQUIT);
		return;
	}

	if (!target || !target->server)
	{
		sendnumeric(client, ERR_NOSUCHSERVER, server);
		return;
	}

	if (!target_remote)
	{
		sendnumeric(client, ERR_NOSUCHSERVER, server_remote);
		return;
	}

	if (target != target_remote->uplink)
	{
		sendnotice(client, "ERROR: Cannot disconnect %s (%s) from %s (%s). %s (%s) is connected to %s (%s) instead.",
		           target->name, target->id,
		           target_remote->name, target_remote->id,
		           target->name, target->id,
		           target->uplink->name, target->uplink->id);
		return;
	}

	/* Zeroing out the remote server shouldn't even be necessary since the
	 * SQUIT code ignores all but parv[1] and parv[argc-1], but it can't
	 * hurt.
	 *
	 * This does exceed the params count registered for the SQUIT command
	 * but those aren't checked in this code path so yolo.
	 * */
	parv[2] = NULL;
	do_cmd(client, recv_mtags, "SQUIT", parc, parv);
}
