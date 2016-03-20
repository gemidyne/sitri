new Handle:g_hSoundModKeys = INVALID_HANDLE;
new Handle:g_hSoundMods = INVALID_HANDLE;

// Contains Product IDs for each Client's class.
// 0 if no sound mod is equipped in that class.
// -1 if client doesn't have account
new g_SoundModEquipmentData[MAXPLAYERS+1][9];

public SoundMods_Init()
{
	g_hSoundModKeys = CreateArray();
	g_hSoundMods = CreateTrie();
}

public SoundMods_MapStart()
{
	AddNormalSoundHook(SoundMods_SoundHook);
	AddAmbientSoundHook(SoundMods_AmbientSoundHook);

	WebAPI_RequestSoundModificationManifests();
}

public SoundMods_PluginEnd()
{
	if (g_hSoundModKeys != INVALID_HANDLE)
	{
		for (new i = 0; i < GetArraySize(g_hSoundModKeys); i++)
		{
			new productId = GetArrayCell(g_hSoundModKeys, i);
			decl String:productIdKey[8];
			IntToString(productId, productIdKey, sizeof(productIdKey));
	
			new Handle:innerTrie = INVALID_HANDLE;

			if (GetTrieValue(g_hSoundMods, productIdKey, innerTrie))
			{
				if (innerTrie != INVALID_HANDLE)
				{
					CloseHandle(innerTrie);
				}
			}
		}

		CloseHandle(g_hSoundModKeys);
	}

	if (g_hSoundMods != INVALID_HANDLE)
	{
		CloseHandle(g_hSoundMods);
	}
}

public SoundMods_OnClientAuthorized(client, const String:auth[])
{
	if (client > 0 && client <= MaxClients)
	{
		if (IsFakeClient(client))
		{
			return;
		}

		WebAPI_RequestSoundModificationEquipment(client, auth);
	}
}

public SoundMods_OnClientDisconnect(client)
{
	if (!IsFakeClient(client))
	{
		for (new i = 0; i <= 8; i++)
		{
			g_SoundModEquipmentData[client][i] = -1;
		}
	}
}

public Action:SoundMods_SoundHook(clients[64], &numClients, String:sample[PLATFORM_MAX_PATH], &entity, &channel, &Float:volume, &level, &pitch, &flags, String:soundEntry[PLATFORM_MAX_PATH], &seed)
{
	return ProcessSound(sample, entity);
}

public Action:SoundMods_AmbientSoundHook(String:sample[PLATFORM_MAX_PATH], &entity, &Float:volume, &level, &pitch, Float:pos[3], &flags, &Float:delay)
{
	return ProcessSound(sample, entity);
}

public Action:ProcessSound(String:sample[PLATFORM_MAX_PATH], &entity)
{
	if (entity <= 0 || entity > MaxClients) 
	{
		return Plugin_Continue;
	}

	if (!IsClientInGame(entity))
	{
		return Plugin_Continue;
	}

	new client = entity;
	new playerClass = GetPlayerClassByTFClassType(TF2_GetPlayerClass(client));

	if (playerClass >= 0 && playerClass <= 8)
	{
		if (g_SoundModEquipmentData[client][playerClass] == -1 || g_SoundModEquipmentData[client][playerClass] == 0)
		{
			return Plugin_Continue;
		}

		new Handle:hModifications = INVALID_HANDLE;

		decl String:productIdKey[8];
		IntToString(g_SoundModEquipmentData[client][playerClass], productIdKey, sizeof(productIdKey));

		if (GetTrieValue(g_hSoundMods, productIdKey, hModifications))
		{
			decl String:original[255];
			decl String:replacement[255];

			String_ToLower(sample, original, sizeof(original));

			if (GetTrieString(hModifications, original, replacement, sizeof(replacement)))
			{
				Format(sample, PLATFORM_MAX_PATH, replacement);
				return Plugin_Changed;
			}
		}
	}

	return Plugin_Continue;
}