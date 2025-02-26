#if defined _smlib_pluginmanager_included
	#endinput
#endif
#define _smlib_pluginmanager_included

#include <sourcemod>
#include <smlib/files>

stock
	Handle:Plugin_Handle = INVALID_HANDLE,
	String:Plugin_Name[256],
	String:Plugin_Author[256],
	String:Plugin_Description[256],
	String:Plugin_Version[256],
	String:Plugin_VersionDescription[256],
	Handle:Plugin_VersionCvar,
	String:Plugin_EnableDescription[256],
	String:Plugin_Url[256],
	String:Plugin_Tag[256],
	String:Plugin_Prefix[256],
	String:Plugin_FileName[PLATFORM_MAX_PATH];


/**
* Initialize SMLib's Plugin Manager
*
* @param prefix				Prefix for console variables.
* @param tag				Tag to used when printing text.
* @param loadTranslations	When true translation file will be loaded (SetFailState will be called on error). Name of translation file has to be <prefix>.phrases.txt.
* @param autoConfig			Loads or creates an auto config file in /cfg/sourcemod/<plugin_name>.cfg
* @noreturn
*/
PluginManager_Initialize(const String:prefix[], const String:tag[]="", bool:loadTranslations=false, bool:autoConfig=true)
{
	Plugin_Handle = GetMyHandle();
	GetPluginInfo(Plugin_Handle, PlInfo_Name, Plugin_Name,					sizeof(Plugin_Name));
	GetPluginInfo(Plugin_Handle, PlInfo_Author, Plugin_Author,				sizeof(Plugin_Author));
	GetPluginInfo(Plugin_Handle, PlInfo_Description, Plugin_Description,	sizeof(Plugin_Description));
	GetPluginInfo(Plugin_Handle, PlInfo_Version, Plugin_Version,			sizeof(Plugin_Version));
	GetPluginInfo(Plugin_Handle, PlInfo_URL, Plugin_Url,					sizeof(Plugin_Url));
	strcopy(Plugin_Prefix,sizeof(Plugin_Prefix),prefix);
	strcopy(Plugin_Tag,sizeof(Plugin_Tag),tag);
	GetPluginFilename(Plugin_Handle, Plugin_FileName,						sizeof(Plugin_FileName));
	
	Format(
		Plugin_VersionDescription,
		sizeof(Plugin_VersionDescription),
		"Version of %s ('%s')",
		Plugin_Name,
		Plugin_FileName
	);
	
	decl String:baseFileName[PLATFORM_MAX_PATH];
	File_GetFileName(Plugin_FileName,baseFileName,sizeof(baseFileName));
	
	if (loadTranslations) {
		decl String:path[PLATFORM_MAX_PATH];
		Format(path, sizeof(path), "%s.phrases", baseFileName);
		File_LoadTranslations(path, true);
	}

	if (autoConfig) {
		AutoExecConfig(true,baseFileName);
	}
	
	Plugin_VersionCvar = PluginManager_CreateConVar("version", Plugin_Version, Plugin_VersionDescription, FCVAR_DONTRECORD|FCVAR_NOTIFY);
	//override old cvar in case of late load (this is needed to kick the engine in the ass, so it'll broadcast its value again)
	SetConVarString(Plugin_VersionCvar,Plugin_Version);
}
/**
* Creates a new console variable.
* Note: You only enter a name and this function will add the prefix you defined in the initialation automatically.
*
* @param name				Name of new convar.
* @param defaultValue		String containing the default value of new convar.
* @param description		Optional description of the convar.
* @param flags				Optional bitstring of flags determining how the convar should be handled. See FCVAR_* constants for more details.
* @param hasMin				Optional boolean that determines if the convar has a minimum value.
* @param min				Minimum floating point value that the convar can have if hasMin is true.
* @param hasMax				Optional boolean that determines if the convar has a maximum value.
* @param max				Maximum floating point value that the convar can have if hasMax is true.
* @return					A handle to the newly created convar. If the convar already exists, a handle to it will still be returned.
*/
stock Handle:PluginManager_CreateConVar(const String:name[], const String:defaultValue[], const String:description[]="", flags=0, bool:hasMin=false, Float:min=0.0, bool:hasMax=false, Float:max=0.0)
{
	decl String:conVarName[256];
	Format(conVarName, sizeof(conVarName), "%s_%s", Plugin_Prefix, name);

	return CreateConVar(conVarName, defaultValue, description, flags, hasMin, min, hasMax, max);
}

/**
* Creates a hook for when a game event is fired.
*
* @param name				Name of event.
* @param callback			An EventHook function pointer.
* @param mode				Optional EventHookMode determining the type of hook.
* @param setFailState		Optional boolean, if true a SetFailState triggers, when given event isn't available in the game/mod.
* @return					True if event exists and was hooked successfully, false otherwise.
*/
stock bool:PluginManager_HookEvent(const String:name[], EventHook:callback, EventHookMode:mode=EventHookMode_Post, bool:setFailState=true)
{
	if (!HookEventEx(name, callback, mode)) {
		
		if (setFailState) {
			SetFailState("can't hook event '%s'", name);
		}
		
		return false;
	}
	
	return true;
}

/**
* Searches for a console variable.  
*
* @param name				Name of convar to find.
* @param setFailState		Optional boolean, if true a SetFailState triggers, when given convar can't be found.
* @return					A handle to the convar if it is found. INVALID_HANDLE otherwise.
*/
stock Handle:PluginManager_FindConVar(const String:name[], bool:setFailState=true)
{
	new Handle:consoleVariable = FindConVar(name);
	
	if (consoleVariable == INVALID_HANDLE) {
		
		if (setFailState) {
			SetFailState("console variable '%s' can't be found", name);
		}
		
		return INVALID_HANDLE;
	}
	
	return consoleVariable;
}

