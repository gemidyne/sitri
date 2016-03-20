stock WebAPI_RequestPlayerSkinEquipment(client, const String:auth[])
{
	if (!g_bSteamLoaded)
	{
		return;
	}

	decl String:path[128];
	Format(path, sizeof(path), "%s/Sitri/GetSkinEquipmentForPlayer.aspx?steamid=%s", IWA_URL, auth);

	new HTTPRequestHandle:hRequest = Steam_CreateHTTPRequest(HTTPMethod_GET, path);
	Steam_SetHTTPRequestHeaderValue(hRequest, "Pragma", "no-cache");
	Steam_SetHTTPRequestHeaderValue(hRequest, "Cache-Control", "no-cache");
 	Steam_SetHTTPRequestNetworkActivityTimeout(hRequest, 60); // IWA might be updating / recompiling. Allow 60 seconds leanway - otherwise the IWA is very quick.
	Steam_SendHTTPRequest(hRequest, PlayerSkinEquipment_OnHttpRequestComplete, client);
}

public PlayerSkinEquipment_OnHttpRequestComplete(HTTPRequestHandle:HTTPRequest, bool:requestSuccessful, HTTPStatusCode:statusCode, any:client)
{
	if (requestSuccessful && statusCode == HTTPStatusCode_OK)
	{
		new responseSize = Steam_GetHTTPResponseBodySize(HTTPRequest);
		new String:response[responseSize];

		Steam_GetHTTPResponseBodyData(HTTPRequest, response, responseSize);

		new Handle:hJson = json_load(response);
		if (hJson == INVALID_HANDLE)
		{
			LogError("Parsing JSON from IWA failed. Unable to load skin equipment.");
			LogError("IWA Response: %s", response);
		}
		else
		{
			for (new iElement = 0; iElement < json_array_size(hJson); iElement++) 
			{
				new Handle:hValue = json_array_get(hJson, iElement);

				ParseSkinEquipmentData(hValue, client);

				CloseHandle(hValue);
			}

			CloseHandle(hJson);
		}
	}
	else
	{
		decl String:sError[256];
		FormatEx(sError, sizeof(sError), "SteamTools error (status code %i) on retrieving skin equipment. Request successful: %s", _:statusCode, requestSuccessful ? "True" : "False");
		LogError(sError);
	}

	Steam_ReleaseHTTPRequest(HTTPRequest);
}

stock ParseSkinEquipmentData(Handle:hJson, client)
{
	new skinID = -1;
	new class = -1;

	new Handle:hIterator = json_object_iter(hJson);

	while (hIterator != INVALID_HANDLE) 
	{
		new String:sKey[128];
		
		json_object_iter_key(hIterator, sKey, sizeof(sKey));

		new Handle:hValue = json_object_iter_value(hIterator);

		if (StrEqual(sKey, "SkinId"))
		{
			skinID = json_integer_value(hValue);
		}
		else if (StrEqual(sKey, "Class"))
		{
			class = json_integer_value(hValue);
		}

		CloseHandle(hValue);
		hIterator = json_object_iter_next(hJson, hIterator);
	}

	if (class >= 0)
	{
		g_PlayerSkinData[client][class] = skinID;
	}
}