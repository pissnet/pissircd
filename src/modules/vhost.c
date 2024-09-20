/*
 *   Unreal Internet Relay Chat Daemon, src/modules/vhost.c
 *   (C) 2000-2024 Carsten V. Munk, Bram Matthys and the UnrealIRCd Team
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

/* Defines */
#define MSG_VHOST       "VHOST"

/* Structs */
ModuleHeader MOD_HEADER
  = {
	"vhost",	/* Name of module */
	"5.0", /* Version */
	"command /vhost", /* Short description of module */
	"UnrealIRCd Team",
	"unrealircd-6",
    };

/* Variables */
ConfigItem_vhost *conf_vhost = NULL;

/* Forward declarations */
void free_vhost_config(void);
int vhost_config_test(ConfigFile *conf, ConfigEntry *ce, int type, int *errs);
int vhost_config_run(ConfigFile *conf, ConfigEntry *ce, int type);
static int stats_vhost(Client *client, const char *flag);
ConfigItem_vhost *find_vhost(const char *name);
void do_vhost(Client *client, ConfigItem_vhost *vhost);
CMD_FUNC(cmd_vhost);

MOD_TEST()
{
	MARK_AS_OFFICIAL_MODULE(modinfo);
	HookAdd(modinfo->handle, HOOKTYPE_CONFIGTEST, 0, vhost_config_test);
	return MOD_SUCCESS;
}

MOD_INIT()
{
	MARK_AS_OFFICIAL_MODULE(modinfo);
	HookAdd(modinfo->handle, HOOKTYPE_CONFIGRUN, 0, vhost_config_run);
	HookAdd(modinfo->handle, HOOKTYPE_STATS, 0, stats_vhost);
	CommandAdd(modinfo->handle, MSG_VHOST, cmd_vhost, MAXPARA, CMD_USER);
	return MOD_SUCCESS;
}

MOD_LOAD()
{
	return MOD_SUCCESS;
}


MOD_UNLOAD()
{
	free_vhost_config();
	return MOD_SUCCESS;
}

void free_vhost_config(void)
{
	ConfigItem_vhost *e, *e_next;

	for (e = conf_vhost; e; e = e_next)
	{
		SWhois *s, *s_next;

		e_next = e->next;

		safe_free(e->login);
		Auth_FreeAuthConfig(e->auth);
		safe_free(e->virthost);
		safe_free(e->virtuser);
		free_security_group(e->match);
		for (s = e->swhois; s; s = s_next)
		{
			s_next = s->next;
			safe_free(s->line);
			safe_free(s->setby);
			safe_free(s);
		}
		DelListItem(e, conf_vhost);
		safe_free(e);
	}
	conf_vhost = NULL;
}
/** Test a vhost { } block in the configuration file */
int vhost_config_test(ConfigFile *conf, ConfigEntry *ce, int type, int *errs)
{
	ConfigEntry *cep;
	char has_vhost = 0, has_login = 0, has_password = 0, has_mask = 0, has_match = 0;
	int errors = 0;

	/* We are only interested in vhost { } blocks */
	if ((type != CONFIG_MAIN) || strcmp(ce->name, "vhost"))
		return 0;

	for (cep = ce->items; cep; cep = cep->next)
	{
		if (!strcmp(cep->name, "vhost"))
		{
			char *at, *tmp, *host;
			if (has_vhost)
			{
				config_warn_duplicate(cep->file->filename,
					cep->line_number, "vhost::vhost");
				continue;
			}
			has_vhost = 1;
			if (!cep->value)
			{
				config_error_empty(cep->file->filename,
					cep->line_number, "vhost", "vhost");
				errors++;
				continue;
			}
			if (!valid_vhost(cep->value))
			{
				config_error("%s:%i: oper::vhost contains illegal characters or is too long: '%s'",
					     cep->file->filename, cep->line_number, cep->value);
				errors++;
			}
		}
		else if (!strcmp(cep->name, "login"))
		{
			if (has_login)
			{
				config_warn_duplicate(cep->file->filename,
					cep->line_number, "vhost::login");
			}
			has_login = 1;
			if (!cep->value)
			{
				config_error_empty(cep->file->filename,
					cep->line_number, "vhost", "login");
				errors++;
				continue;
			}
		}
		else if (!strcmp(cep->name, "password"))
		{
			if (has_password)
			{
				config_warn_duplicate(cep->file->filename,
					cep->line_number, "vhost::password");
			}
			has_password = 1;
			if (!cep->value)
			{
				config_error_empty(cep->file->filename,
					cep->line_number, "vhost", "password");
				errors++;
				continue;
			}
			if (Auth_CheckError(cep, 0) < 0)
				errors++;
		}
		else if (!strcmp(cep->name, "mask"))
		{
			has_mask = 1;
			test_match_block(conf, cep, &errors);
		}
		else if (!strcmp(cep->name, "match"))
		{
			has_match = 1;
			test_match_block(conf, cep, &errors);
		}
		else if (!strcmp(cep->name, "swhois"))
		{
			/* multiple is ok */
		}
		else
		{
			config_error_unknown(cep->file->filename, cep->line_number,
				"vhost", cep->name);
			errors++;
		}
	}
	if (!has_vhost)
	{
		config_error_missing(ce->file->filename, ce->line_number,
			"vhost::vhost");
		errors++;
	}
	if (!has_login)
	{
		config_error_missing(ce->file->filename, ce->line_number,
			"vhost::login");
		errors++;

	}
	if (!has_password)
	{
		config_error_missing(ce->file->filename, ce->line_number,
			"vhost::password");
		errors++;
	}
	if (!has_mask && !has_match)
	{
		config_error_missing(ce->file->filename, ce->line_number,
			"vhost::match");
		errors++;
	}
	if (has_mask && has_match)
	{
		config_error("%s:%d: You cannot have both ::mask and ::match. "
		             "You should only use %s::match.",
		             ce->file->filename, ce->line_number, ce->name);
		errors++;
	}

	*errs = errors;
	return errors ? -1 : 1;
}

/** Process a vhost { } block in the configuration file */
int vhost_config_run(ConfigFile *conf, ConfigEntry *ce, int type)
{
	ConfigEntry *cep, *cepp;
	ConfigItem_vhost *vhost;

	/* We are only interested in vhost { } blocks */
	if ((type != CONFIG_MAIN) || strcmp(ce->name, "vhost"))
		return 0;

	vhost = safe_alloc(sizeof(ConfigItem_vhost));
	vhost->match = safe_alloc(sizeof(SecurityGroup));

	for (cep = ce->items; cep; cep = cep->next)
	{
		if (!strcmp(cep->name, "vhost"))
		{
			char *user, *host;
			user = strtok(cep->value, "@");
			host = strtok(NULL, "");
			if (!host)
				safe_strdup(vhost->virthost, user);
			else
			{
				safe_strdup(vhost->virtuser, user);
				safe_strdup(vhost->virthost, host);
			}
		}
		else if (!strcmp(cep->name, "login"))
			safe_strdup(vhost->login, cep->value);
		else if (!strcmp(cep->name, "password"))
			vhost->auth = AuthBlockToAuthConfig(cep);
		else if (!strcmp(cep->name, "match") || !strcmp(cep->name, "mask"))
		{
			conf_match_block(conf, cep, &vhost->match);
		}
		else if (!strcmp(cep->name, "swhois"))
		{
			SWhois *s;
			if (cep->items)
			{
				for (cepp = cep->items; cepp; cepp = cepp->next)
				{
					s = safe_alloc(sizeof(SWhois));
					safe_strdup(s->line, cepp->name);
					safe_strdup(s->setby, "vhost");
					AddListItem(s, vhost->swhois);
				}
			} else
			if (cep->value)
			{
				s = safe_alloc(sizeof(SWhois));
				safe_strdup(s->line, cep->value);
				safe_strdup(s->setby, "vhost");
				AddListItem(s, vhost->swhois);
			}
		}
	}
	AppendListItem(vhost, conf_vhost);
	return 1;
}

static int stats_vhost(Client *client, const char *flag)
{
	ConfigItem_vhost *vhosts;
	NameValuePrioList *m;

	if (strcmp(flag, "S") && strcasecmp(flag, "vhost"))
		return 0; /* Not for us */

	for (vhosts = conf_vhost; vhosts; vhosts = vhosts->next)
	{
		for (m = vhosts->match->printable_list; m; m = m->next)
		{
			sendtxtnumeric(client, "vhost %s%s%s %s %s",
			               vhosts->virtuser ? vhosts->virtuser : "",
			               vhosts->virtuser ? "@" : "",
			               vhosts->virthost,
			               vhosts->login,
			               namevalue_nospaces(m));
		}
	}

	return 1;
}



ConfigItem_vhost *find_vhost(const char *name)
{
	ConfigItem_vhost *vhost;

	for (vhost = conf_vhost; vhost; vhost = vhost->next)
	{
		if (!strcmp(name, vhost->login))
			return vhost;
	}

	return NULL;
}

CMD_FUNC(cmd_vhost)
{
	ConfigItem_vhost *vhost;
	char login[HOSTLEN+1];
	const char *password;

	if (!MyUser(client))
		return;

	if ((parc < 2) || BadPtr(parv[1]))
	{
		sendnumeric(client, ERR_NEEDMOREPARAMS, "VHOST");
		return;

	}

	/* cut-off too long login names. HOSTLEN is arbitrary, we just don't want our
	 * error messages to be cut off because the user is sending huge login names.
	 */
	strlcpy(login, parv[1], sizeof(login));

	password = (parc > 2) ? parv[2] : "";

	if (!(vhost = find_vhost(login)))
	{
		unreal_log(ULOG_WARNING, "vhost", "VHOST_FAILED", client,
		           "Failed VHOST attempt by $client.details [reason: $reason] [vhost-block: $vhost_block]",
		           log_data_string("reason", "Vhost block not found"),
		           log_data_string("fail_type", "UNKNOWN_VHOST_NAME"),
		           log_data_string("vhost_block", login));
		sendnotice(client, "*** [\2vhost\2] Login for %s failed - password incorrect", login);
		return;
	}

	if (!user_allowed_by_security_group(client, vhost->match))
	{
		unreal_log(ULOG_WARNING, "vhost", "VHOST_FAILED", client,
		           "Failed VHOST attempt by $client.details [reason: $reason] [vhost-block: $vhost_block]",
		           log_data_string("reason", "Host does not match"),
		           log_data_string("fail_type", "NO_HOST_MATCH"),
		           log_data_string("vhost_block", login));
		sendnotice(client, "*** No vHost lines available for your host");
		return;
	}

	if (!Auth_Check(client, vhost->auth, password))
	{
		unreal_log(ULOG_WARNING, "vhost", "VHOST_FAILED", client,
		           "Failed VHOST attempt by $client.details [reason: $reason] [vhost-block: $vhost_block]",
		           log_data_string("reason", "Authentication failed"),
		           log_data_string("fail_type", "AUTHENTICATION_FAILED"),
		           log_data_string("vhost_block", login));
		sendnotice(client, "*** [\2vhost\2] Login for %s failed - password incorrect", login);
		return;
	}

	do_vhost(client, vhost);
}

void do_vhost(Client *client, ConfigItem_vhost *vhost)
{
	char olduser[USERLEN+1];

	/* Authentication passed, but.. there could still be other restrictions: */
	switch (UHOST_ALLOWED)
	{
		case UHALLOW_NEVER:
			if (MyUser(client))
			{
				sendnotice(client, "*** /vhost is disabled");
				return;
			}
			break;
		case UHALLOW_ALWAYS:
			break;
		case UHALLOW_NOCHANS:
			if (MyUser(client) && client->user->joined)
			{
				sendnotice(client, "*** /vhost can not be used while you are on a channel");
				return;
			}
			break;
		case UHALLOW_REJOIN:
			/* join sent later when the host has been changed */
			break;
	}

	/* All checks passed, now let's go ahead and change the host */

	userhost_save_current(client);

	safe_strdup(client->user->virthost, vhost->virthost);
	if (vhost->virtuser)
	{
		strlcpy(olduser, client->user->username, sizeof(olduser));
		strlcpy(client->user->username, vhost->virtuser, sizeof(client->user->username));
		sendto_server(client, 0, 0, NULL, ":%s SETIDENT %s", client->id,
		    client->user->username);
	}
	client->umodes |= UMODE_HIDE;
	client->umodes |= UMODE_SETHOST;
	sendto_server(client, 0, 0, NULL, ":%s SETHOST %s", client->id, client->user->virthost);
	sendto_one(client, NULL, ":%s MODE %s :+tx", client->name, client->name);
	if (vhost->swhois)
	{
		SWhois *s;
		for (s = vhost->swhois; s; s = s->next)
			swhois_add(client, "vhost", -100, s->line, &me, NULL);
	}
	sendnotice(client, "*** Your vhost is now %s%s%s",
		vhost->virtuser ? vhost->virtuser : "",
		vhost->virtuser ? "@" : "",
		vhost->virthost);

	/* Only notify on logins, not on auto logins (should we make that configurable?)
	 * (if you do want it for auto logins, note that vhost->login will be NULL
	 *  in the unreal_log() call currently below).
	 */
	if (vhost->login)
	{
		if (vhost->virtuser)
		{
			/* virtuser@virthost */
			unreal_log(ULOG_INFO, "vhost", "VHOST_SUCCESS", client,
				   "$client.details is now using vhost $virtuser@$virthost [vhost-block: $vhost_block]",
				   log_data_string("virtuser", vhost->virtuser),
				   log_data_string("virthost", vhost->virthost),
				   log_data_string("vhost_block", vhost->login));
		} else {
			/* just virthost */
			unreal_log(ULOG_INFO, "vhost", "VHOST_SUCCESS", client,
				   "$client.details is now using vhost $virthost [vhost-block: $vhost_block]",
				   log_data_string("virthost", vhost->virthost),
				   log_data_string("vhost_block", vhost->login));
		}
	}

	userhost_changed(client);
}
