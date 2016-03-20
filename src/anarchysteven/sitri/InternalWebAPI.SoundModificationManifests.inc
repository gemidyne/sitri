stock WebAPI_RequestSoundModificationManifests()
{
	if (!g_bSteamLoaded)
	{
		return;
	}

	new port = GetConVarInt(FindConVar("hostport"));

	decl String:path[128];
	Format(path, sizeof(path), "%s/Sitri/GetAvailableSoundModifications.aspx?port=%d", IWA_URL, port);

	new HTTPRequestHandle:hRequest = Steam_CreateHTTPRequest(HTTPMethod_GET, path);
	Steam_SetHTTPRequestHeaderValue(hRequest, "Pragma", "no-cache");
	Steam_SetHTTPRequestHeaderValue(hRequest, "Cache-Control", "no-cache");
	Steam_SetHTTPRequestNetworkActivityTimeout(hRequest, 60); // IWA might be updating / recompiling. Allow 60 seconds leanway - otherwise the IWA is very quick.
	Steam_SendHTTPRequest(hRequest, SoundModManifests_OnHttpRequestComplete);
}

public SoundModManifests_OnHttpRequestComplete(HTTPRequestHandle:HTTPRequest, bool:requestSuccessful, HTTPStatusCode:statusCode)
{
	if (requestSuccessful && statusCode == HTTPStatusCode_OK)
	{
		new responseSize = Steam_GetHTTPResponseBodySize(HTTPRequest);
		new String:response[responseSize];

		Steam_GetHTTPResponseBodyData(HTTPRequest, response, responseSize);

		new Handle:hJson = json_load(response);
		if (hJson == INVALID_HANDLE)
		{
			LogError("Parsing JSON from IWA failed. Unable to load server manifest.");
			LogError("IWA Response: %s", response);
		}
		else
		{
			ClearArray(g_hSoundModKeys);
			new Handle:hIterator = json_object_iter(hJson);

			while (hIterator != INVALID_HANDLE) 
			{
				new String:sKey[64];
				json_object_iter_key(hIterator, sKey, sizeof(sKey));

				new Handle:hValue = json_object_iter_value(hIterator);

				if (StrEqual(sKey, "SoundMods"))
				{
					new size = json_array_size(hValue);

					for (new iElement = 0; iElement < size; iElement++) 
					{
						new Handle:hGroup = json_array_get(hValue, iElement);

						ParseSoundModGroup(hGroup);

						CloseHandle(hGroup);
					}
				}

				hIterator = json_object_iter_next(hJson, hIterator);
			}

			CloseHandle(hJson);
		}
	}
	else
	{
		decl String:sError[256];
		FormatEx(sError, sizeof(sError), "SteamTools error (status code %i) on retrieving soundmods. Request successful: %s", _:statusCode, requestSuccessful ? "True" : "False");
		LogError(sError);
	}

	Steam_ReleaseHTTPRequest(HTTPRequest);
}

stock ParseSoundModGroup(Handle:hJson)
{
	new Handle:hIterator = json_object_iter(hJson);
	new productId = 0;
	new Handle:hModifications = CreateTrie();

	while (hIterator != INVALID_HANDLE) 
	{
		new String:sKey[64];
		json_object_iter_key(hIterator, sKey, sizeof(sKey));

		new Handle:hValue = json_object_iter_value(hIterator);

		if (StrEqual(sKey, "ProductId"))
		{
			productId = json_integer_value(hValue);
		}
		else if (StrEqual(sKey, "Modifications"))
		{
			for (new iElement = 0; iElement < json_array_size(hValue); iElement++) 
			{
				new Handle:hModificationDetails = json_array_get(hValue, iElement);

				ParseSoundModManifest(hModifications, hModificationDetails);

				CloseHandle(hModificationDetails);
			}
		}

		CloseHandle(hValue);
		hIterator = json_object_iter_next(hJson, hIterator);
	}

	decl String:productIdKey[8];
	IntToString(productId, productIdKey, sizeof(productIdKey));
	
	SetTrieValue(g_hSoundMods, productIdKey, hModifications);
	PushArrayCell(g_hSoundModKeys, productId);
}

stock ParseSoundModManifest(Handle:hModifications, Handle:hJson)
{
	new String:originalFile[255];
	new String:replacementFile[255];

	new Handle:hIterator = json_object_iter(hJson);

	while (hIterator != INVALID_HANDLE) 
	{
		new String:sKey[64];
		json_object_iter_key(hIterator, sKey, sizeof(sKey));

		new Handle:hValue = json_object_iter_value(hIterator);

		if (StrEqual(sKey, "OriginalFile"))
		{
			json_string_value(hValue, originalFile, sizeof(originalFile));

			String_ToLower(originalFile, originalFile, sizeof(originalFile));
		}
		else if (StrEqual(sKey, "ReplacementFile"))
		{
			json_string_value(hValue, replacementFile, sizeof(replacementFile));
		}

		CloseHandle(hValue);
		hIterator = json_object_iter_next(hJson, hIterator);
	}

	if (strlen(replacementFile) > 0)
	{
		PrecacheSound(replacementFile, true);

		decl String:buffer[255];
		Format(buffer, sizeof(buffer), "sound/%s", replacementFile);
		AddFileToDownloadsTable(buffer);

		SetTrieString(hModifications, originalFile, replacementFile);
	}
}