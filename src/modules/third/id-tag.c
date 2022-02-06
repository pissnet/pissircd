/*
 *   IRC - Internet Relay Chat, src/modules/third/id-tag.c
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
	"third/id-tag",
	"1.0",
	"letspiss.net/id message tag and capability",
	"Noisytoot",
	"unrealircd-6",
	};

long CAP_ID_TAG = 0L;

int idtag_mtag_is_ok(Client *client, const char *name, const char *value);
void mtag_add_idtag(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, const char *signature);
int idtag_mtag_should_send_to_client(Client *target);

MOD_INIT()
{
	ClientCapabilityInfo cap;
	ClientCapability *c;
	MessageTagHandlerInfo mtag;

	memset(&cap, 0, sizeof(cap));
	cap.name = "letspiss.net/id";
	c = ClientCapabilityAdd(modinfo->handle, &cap, &CAP_ID_TAG);

	memset(&mtag, 0, sizeof(mtag));
	mtag.name = "letspiss.net/id";
	mtag.is_ok = idtag_mtag_is_ok;
	mtag.should_send_to_client = idtag_mtag_should_send_to_client;
	mtag.clicap_handler = c;
	MessageTagHandlerAdd(modinfo->handle, &mtag);

	HookAddVoid(modinfo->handle, HOOKTYPE_NEW_MESSAGE, 0, mtag_add_idtag);

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
int idtag_mtag_is_ok(Client *client, const char *name, const char *value)
{
	return 0;
}

void mtag_add_idtag(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, const char *signature)
{
	MessageTag *m = safe_alloc(sizeof(MessageTag));
	safe_strdup(m->name, "letspiss.net/id");
	safe_strdup(m->value, client->id);
	AddListItem(m, *mtag_list);
}

/** Outgoing filter for this message tag */
int idtag_mtag_should_send_to_client(Client *target)
{
	if (IsOper(target))
		return 1;
	return 0;
}
