#pragma semicolon 1
#pragma dynamic 131072

#include <sourcemod>
#include <sdktools>
#include <smlib>
#include <tf2>
#include <tf2_stocks>
#include <morecolors>

#define AUTOLOAD_EXTENSIONS
#define REQUIRE_EXTENSIONS
#include <steamtools>

#define PLUGIN_PREFIX "\x07356AA0[ \x0744A8D9Sitri \x07356AA0] \x01"

new bool:g_bIsMapEnding = false;
new bool:g_bSteamLoaded;

#include "anarchysteven/sitri/playerskins.inc"
#include "anarchysteven/sitri/soundmods.inc"
#include "anarchysteven/sitri/InternalWebAPI.inc"

public Plugin:myinfo = 
{
	name = "Sitri",
	author = "Anarchy Steven",
	description = "A SourceMod plugin for Team Fortress 2 which allows PlayerSkins and SoundModifications to be used and controlled server-side.",
	version = "1.0",
	url = "http://www.gemini.software"
}

public OnPluginStart()
{
	PlayerSkins_Init();
	SoundMods_Init();

	HookEvent("teamplay_game_over", Event_MapOver);
}

public OnPluginEnd()
{
	PlayerSkins_PluginEnd();
}

public OnMapStart()
{
	PlayerSkins_MapStart();
	SoundMods_MapStart();

	g_bIsMapEnding = false;
}

public Action:Event_MapOver(Handle:event, const String:name[], bool:dontBroadcast)
{
	g_bIsMapEnding = true;

	for (new i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && !IsFakeClient(i))
		{
			PlayerSkins_OnClientDisconnect(i);
		}
	}
}

public OnClientAuthorized(client, const String:auth[])
{
	PlayerSkins_OnClientAuthorized(client, auth);
	SoundMods_OnClientAuthorized(client, auth);
}

public OnClientDisconnect(client)
{
	if (IsClientInGame(client) && !g_bIsMapEnding)
	{
		PlayerSkins_OnClientDisconnect(client);
		SoundMods_OnClientDisconnect(client);
	}
}