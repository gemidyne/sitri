stock WebAPI_RequestPlayerSkinManifests()
{
	if (!g_bSteamLoaded)
	{
		return;
	}

	// The reason for passing the server's port number is that you may 
	new port = GetConVarInt(FindConVar("hostport"));

	decl String:path[128];
	Format(path, sizeof(path), "%s/Sitri/GetAvailableSkinManifests.aspx?port=%d", IWA_URL, port);

	new HTTPRequestHandle:hRequest = Steam_CreateHTTPRequest(HTTPMethod_GET, path);
	Steam_SetHTTPRequestHeaderValue(hRequest, "Pragma", "no-cache");
	Steam_SetHTTPRequestHeaderValue(hRequest, "Cache-Control", "no-cache");
	Steam_SetHTTPRequestNetworkActivityTimeout(hRequest, 60); // IWA might be updating / recompiling. Allow 60 seconds leanway - otherwise the IWA is very quick.
	Steam_SendHTTPRequest(hRequest, PlayerSkinManifests_OnHttpRequestComplete);
}

public PlayerSkinManifests_OnHttpRequestComplete(HTTPRequestHandle:HTTPRequest, bool:requestSuccessful, HTTPStatusCode:statusCode)
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
			for (new iElement = 0; iElement < json_array_size(hJson); iElement++) 
			{
				new Handle:hValue = json_array_get(hJson, iElement);

				ParseSkinManifest(hValue);

				CloseHandle(hValue);
			}

			CloseHandle(hJson);

			g_bPlayerSkinsManfiestsLoaded = true;
		}
	}
	else
	{
		decl String:sError[256];
		FormatEx(sError, sizeof(sError), "SteamTools error (status code %i) on retrieving skin manifests. Request successful: %s", _:statusCode, requestSuccessful ? "True" : "False");
		LogError(sError);
	}

	Steam_ReleaseHTTPRequest(HTTPRequest);
}

stock ParseSkinManifest(Handle:hJson)
{
	new skinID = 0;
	new String:modelFileName[64];
	decl String:buffer[255];

	new Handle:hIterator = json_object_iter(hJson);

	while (hIterator != INVALID_HANDLE) 
	{
		new String:sKey[64];
		json_object_iter_key(hIterator, sKey, sizeof(sKey));

		new Handle:hValue = json_object_iter_value(hIterator);

		if (StrEqual(sKey, "SkinId"))
		{
			skinID = json_integer_value(hValue);
		}
		else if (StrEqual(sKey, "FullModelPath"))
		{
			json_string_value(hValue, modelFileName, sizeof(modelFileName));
		}
		else if (StrEqual(sKey, "Files"))
		{
			for (new iElement = 0; iElement < json_array_size(hValue); iElement++) 
			{
				decl String:file[255];

				json_array_get_string(hValue, iElement, file, sizeof(file));

				if (strlen(file) > 0)
				{
					AddFileToDownloadsTable(file);
				}
			}
		}

		CloseHandle(hValue);
		hIterator = json_object_iter_next(hJson, hIterator);
	}

	Format(buffer, sizeof(buffer), "%d_modelfilename", skinID);
	SetTrieString(g_hPlayerSkins, buffer, modelFileName);

	if (strlen(modelFileName) > 0)
	{
		PrecacheModel(modelFileName);		
	}
}