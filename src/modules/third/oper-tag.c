/*
 *   IRC - Internet Relay Chat, src/modules/third/oper-tag.c
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

ModuleHeader MOD_HEADER
  = {
	"third/oper-tag",
	"1.0",
	"solanum.chat/oper message tag and capability",
	"Noisytoot",
	"unrealircd-5",
	};

long CAP_OPER_TAG = 0L;

int opertag_mtag_is_ok(Client *client, char *name, char *value);
void mtag_add_opertag(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, char *signature);

MOD_INIT()
{
	ClientCapabilityInfo cap;
	ClientCapability *c;
	MessageTagHandlerInfo mtag;

	memset(&cap, 0, sizeof(cap));
	cap.name = "solanum.chat/oper";
	c = ClientCapabilityAdd(modinfo->handle, &cap, &CAP_OPER_TAG);

	memset(&mtag, 0, sizeof(mtag));
	mtag.name = "solanum.chat/oper";
	mtag.is_ok = opertag_mtag_is_ok;
	mtag.clicap_handler = c;
	MessageTagHandlerAdd(modinfo->handle, &mtag);

	HookAddVoid(modinfo->handle, HOOKTYPE_NEW_MESSAGE, 0, mtag_add_opertag);

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

/** This function verifies if the client sending the mtag is permitted to do so.
 */
int opertag_mtag_is_ok(Client *client, char *name, char *value)
{
	return 0;
}

void mtag_add_opertag(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, char *signature)
{
	MessageTag *m;

	if (IsOper(client) && !IsHideOper(client))
	{
		MessageTag *m = safe_alloc(sizeof(MessageTag));
		safe_strdup(m->name, "solanum.chat/oper");
		m->value = NULL;
		AddListItem(m, *mtag_list);
	}
}
