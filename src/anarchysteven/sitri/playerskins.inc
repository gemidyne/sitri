#define TF_TEAM_RED 2
#define TF_TEAM_BLUE 3

#define TF_CLASS_SCOUT 0
#define TF_CLASS_SOLDIER 1
#define TF_CLASS_PYRO 2
#define TF_CLASS_DEMOMAN 3
#define TF_CLASS_HEAVY 4
#define TF_CLASS_ENGINEER 5
#define TF_CLASS_MEDIC 6
#define TF_CLASS_SNIPER 7
#define TF_CLASS_SPY 8

new bool:g_bIsPlayerSkinsReady = false;
new bool:g_bPlayerSkinsManfiestsLoaded = false;

new bool:bIsAnyTeamAllowed = false;
new bool:bIsRedTeamAllowed = false;
new bool:bIsBlueTeamAllowed = false;

new Handle:g_hPlayerSkins = INVALID_HANDLE;

// Contains Skin IDs for each Client's class.
// 0 if no Skin is equipped in that class.
// -1 if Client doesn't have account
new g_PlayerSkinData[MAXPLAYERS+1][9];

public PlayerSkins_Init()
{
	HookEvent("player_spawn", Event_PlayerSpawn, EventHookMode_PostNoCopy);
	HookEvent("post_inventory_application", Event_InventoryApplication, EventHookMode_Post);

	g_hPlayerSkins = CreateTrie();
}

public PlayerSkins_MapStart()
{
	WebAPI_RequestPlayerSkinManifests();
	CreateTimer(2.0, PlayerSkins_OnMapStartDelayed);
}

public PlayerSkins_PluginEnd()
{
	if (g_hPlayerSkins != INVALID_HANDLE)
	{
		CloseHandle(g_hPlayerSkins);
	}
}

public PlayerSkins_OnClientAuthorized(client, const String:auth[])
{
	if (!g_bPlayerSkinsManfiestsLoaded || !g_bIsPlayerSkinsReady || !g_bSteamLoaded)
	{
		// Wait 5 seconds, for everything to get ready.
		if (client > 0 && client <= MaxClients)
		{
			CreateTimer(5.0, PlayerSkins_DelayedClientApiQuery, client);
		}

		return;
	}

	if (!bIsAnyTeamAllowed)
	{
		return;
	}

	if (client > 0 && client <= MaxClients)
	{
		if (IsFakeClient(client))
		{
			return;
		}

		WebAPI_RequestPlayerSkinEquipment(client, auth);
	}
}

public Action:PlayerSkins_OnMapStartDelayed(Handle:timer)
{
	// Delaying the below actions allows Framework to get ready.

	// In StSv, Sitri would interact with our Gamemode Manager to figure out whether 
	// only certain teams could have skins. Currently, Gamemode Manager is not open-source
	// so I am going to hardcode these values to true.
	bIsRedTeamAllowed = true;
	bIsBlueTeamAllowed = true;
	bIsAnyTeamAllowed = bIsRedTeamAllowed || bIsBlueTeamAllowed;

	LogMessage("Skins availability - RED: %s, BLUE: %s", bIsRedTeamAllowed ? "Yes" : "No", bIsBlueTeamAllowed ? "Yes" : "No");

	// This only really concerns if Framework has just loaded and Gamemode Manager is ready.
	g_bIsPlayerSkinsReady = true;

	return Plugin_Handled;
}

public Action:PlayerSkins_DelayedClientApiQuery(Handle:timer, any:client)
{
	if (client > 0 && client <= MaxClients)
	{
		if (IsFakeClient(client))
		{
			return Plugin_Handled;
		}

		if (!g_bPlayerSkinsManfiestsLoaded || !g_bIsPlayerSkinsReady)
		{
			if (IsClientInGame(client) && !IsFakeClient(client))
			{
				CPrintToChat(client, "%sPlease note that you will not have any Player Skins equipped during this map due to server issues.", PLUGIN_PREFIX);
				return Plugin_Handled;
			}
		}

		decl String:auth[32];
		GetClientAuthId(client, AuthId_Steam2, auth, sizeof(auth));

		WebAPI_RequestPlayerSkinEquipment(client, auth);
	}

	return Plugin_Handled;
}

public PlayerSkins_OnClientDisconnect(client)
{
	if (!IsFakeClient(client))
	{
		for (new i = 0; i <= 8; i++)
		{
			g_PlayerSkinData[client][i] = -1;
		}
	}
}

public Event_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
	if (!bIsAnyTeamAllowed)
	{
		return;
	}

	new client = GetClientOfUserId(GetEventInt(event, "userid"));
	
	if (client > 0 && client <= MaxClients && IsClientInGame(client) && !IsFakeClient(client))
	{
		new userId = GetClientUserId(client);
		CreateTimer(0.2, Timer_UpdatePlayerModel, userId);
		CreateTimer(2.0, Timer_UpdatePlayerModel, userId, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
	}
}

public Event_InventoryApplication(Handle:event, const String:name[], bool:dontBroadcast)
{
	if (!bIsAnyTeamAllowed)
	{
		return;
	}

	new client = GetClientOfUserId(GetEventInt(event, "userid"));
	if (client > 0 && client <= MaxClients && IsClientInGame(client) && !IsFakeClient(client))
	{
		CreateTimer(0.2, Timer_UpdatePlayerModel, GetClientUserId(client));
	}
}

public Action:Timer_UpdatePlayerModel(Handle:hTimer, any:data)
{
	if (!bIsAnyTeamAllowed)
	{
		return Plugin_Stop;
	}

	new iClient = GetClientOfUserId(data);
	if (iClient <= 0 || iClient > MaxClients) 
	{
		return Plugin_Stop;
	}
	
	if (!IsClientInGame(iClient)) 
	{
		return Plugin_Stop;
	}

	if (IsFakeClient(iClient))
	{
		return Plugin_Stop;
	}

	if (!IsPlayerAlive(iClient))
	{
		return Plugin_Stop;
	}

	new iTeam = GetClientTeam(iClient);

	if (iTeam != TF_TEAM_RED && iTeam != TF_TEAM_BLUE)
	{
		return Plugin_Stop;
	}

	if (!bIsRedTeamAllowed && iTeam == TF_TEAM_RED)
	{
		return Plugin_Stop;
	}

	if (!bIsBlueTeamAllowed && iTeam == TF_TEAM_BLUE)
	{
		return Plugin_Stop;
	}

	new class = GetPlayerClassByTFClassType(TF2_GetPlayerClass(iClient));

	if (class >= 0 && class <= 8)
	{
		new bool:removeCustomModel = false;
		decl String:buffer[256];
		decl String:buffer2[64];

		if (g_PlayerSkinData[iClient][class] == -1 || g_PlayerSkinData[iClient][class] == 0)
		{
			removeCustomModel = true;
		}

		if (!removeCustomModel)
		{
			Format(buffer2, sizeof(buffer2), "%d_modelfilename", g_PlayerSkinData[iClient][class]);

			if (!GetTrieString(g_hPlayerSkins, buffer2, buffer, sizeof(buffer)))
			{
				removeCustomModel = true;
			}
		}

		if (removeCustomModel)
		{
			buffer = "";
		}

		SetVariantString(buffer);
		AcceptEntityInput(iClient, "SetCustomModel");
		SetEntProp(iClient, Prop_Send, "m_nSkin", iTeam - 2);

		if (!removeCustomModel)
		{
			SetEntProp(iClient, Prop_Send, "m_bUseClassAnimations", 1);
			SetEntProp(iClient, Prop_Send, "m_nBody", 0);
		}
		else
		{
			SetEntProp(iClient, Prop_Send, "m_bUseClassAnimations", 1);
			SetEntProp(iClient, Prop_Send, "m_nBody", 1);

			return Plugin_Stop;
		}
	}

	return Plugin_Continue;
}

stock GetPlayerClassByTFClassType(TFClassType:class)
{
	switch (class)
	{
		case TFClass_Scout: return TF_CLASS_SCOUT;
		case TFClass_Soldier: return TF_CLASS_SOLDIER;
		case TFClass_Pyro: return TF_CLASS_PYRO;
		case TFClass_DemoMan: return TF_CLASS_DEMOMAN;
		case TFClass_Heavy: return TF_CLASS_HEAVY;
		case TFClass_Engineer: return TF_CLASS_ENGINEER;
		case TFClass_Medic: return TF_CLASS_MEDIC;
		case TFClass_Sniper: return TF_CLASS_SNIPER;
		case TFClass_Spy: return TF_CLASS_SPY;
	}
	return 0;
}