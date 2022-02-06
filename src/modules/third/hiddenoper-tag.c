/*
 *   IRC - Internet Relay Chat, src/modules/third/hiddenoper-tag.c
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
	"third/hiddenoper-tag",
	"1.0",
	"letspiss.net/hiddenoper message tag and capability",
	"Noisytoot",
	"unrealircd-5",
	};

long CAP_HIDDENOPER_TAG = 0L;

int hiddenopertag_mtag_is_ok(Client *client, char *name, char *value);
void mtag_add_hiddenopertag(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, char *signature);
int hiddenopertag_mtag_can_send(Client *target);

MOD_INIT()
{
	ClientCapabilityInfo cap;
	ClientCapability *c;
	MessageTagHandlerInfo mtag;

	memset(&cap, 0, sizeof(cap));
	cap.name = "letspiss.net/hiddenoper";
	c = ClientCapabilityAdd(modinfo->handle, &cap, &CAP_HIDDENOPER_TAG);

	memset(&mtag, 0, sizeof(mtag));
	mtag.name = "letspiss.net/hiddenoper";
	mtag.is_ok = hiddenopertag_mtag_is_ok;
	mtag.can_send = hiddenopertag_mtag_can_send;
	mtag.clicap_handler = c;
	MessageTagHandlerAdd(modinfo->handle, &mtag);

	HookAddVoid(modinfo->handle, HOOKTYPE_NEW_MESSAGE, 0, mtag_add_hiddenopertag);

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
int hiddenopertag_mtag_is_ok(Client *client, char *name, char *value)
{
	return 0;
}

void mtag_add_hiddenopertag(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, char *signature)
{
	MessageTag *m;

	if (IsHideOper(client))
	{
		MessageTag *m = safe_alloc(sizeof(MessageTag));
		safe_strdup(m->name, "letspiss.net/hiddenoper");
		m->value = NULL;
		AddListItem(m, *mtag_list);
	}
}

/** Outgoing filter for this message tag */
int hiddenopertag_mtag_can_send(Client *target)
{
	if (IsOper(target))
		return 1;
	return 0;
}
