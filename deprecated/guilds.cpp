/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's guild methods
*/

/*!
\brief check if guilded
\author Endymion
\return true if the char is guilded
*/
bool cChar::isGuilded()
{
	return guild!=NULL;
}

/*!
\brief set Guild
\author Endymion
\param guild the guild
*/
void cChar::setGuild( P_GUILD guild, P_GUILD_MEMBER member )
{
	this->guild = guild;
	this->member = member;
}

/*!
\brief get Guild
\author Endymion
\return the guild
*/
P_GUILD cChar::getGuild()
{
	return guild;
}

/*!
\brief get Guild member
\author Endymion
\return the guild member
*/
P_GUILD_MEMBER cChar::getGuildMember()
{
	return member;
}

/*!
\brief return the guild type
\author Sparhawk
\date 31/08/2002
\since 0.82
\return short type of guild. INVALID = no guild 0 = standard guild 1 = order guild 2 = chaos guild
*/
short cChar::GetGuildType()
{
	return guildType;
}

/*!
\brief set the guild type
\author Sparhawk
\date 31/08/2002
\since 0.82
\param newGuildType value must be between INVALID and MAX_GUILDTYPE
*/
void cChar::SetGuildType(short newGuildType)
{
	if ( newGuildType >= INVALID && newGuildType <= MAX_GUILDTYPE )
		guildType = newGuildType;
}

/*!
\brief return guild traitor status
\author Sparhawk
\date 31/08/2002
\since 0.82
\return bool: true is traitor
*/
bool cChar::IsGuildTraitor()
{
	return guildTraitor;
}

/*!
\brief set guild traitor status
\author Sparhawk
\date 31/08/2002
\since 0.82
*/
void cChar::SetGuildTraitor()
{
	guildTraitor = true;
}

/*!
\brief reset guild traitor status
\author Sparhawk
\date 31/08/2002
\since 0.82
*/
void cChar::ResetGuildTraitor()
{
	guildTraitor = false;
}

/*!
\brief return guild title toggle status
\author Sparhawk
\date 31/08/2002
\since 0.82
\return true is show title
*/
bool	cChar::HasGuildTitleToggle()
{
	return guildToggle;
}

/*!
\brief set guild title toggle
\author Sparhawk
\date 31/08/2002
\since 0.82
*/
void cChar::SetGuildTitleToggle()
{
	guildToggle = true;
}

/*!
\brief reset guild title toggle
\author Sparhawk
\date 31/08/2002
\since 0.82
*/
void cChar::ResetGuildTitleToggle()
{
	guildToggle = false;
}

/*!
\brief return the guild fealty
\author Sparhawk
\date 31/08/2002
\since 0.82
\return serial of guild fealty
*/
uint32_t cChar::GetGuildFealty()
{
	return guildFealty;
}

/*!
\brief set the guild fealty
\author Sparhawk
\date 31/08/2002
\since 0.82
\param newGuildFealty serial of char to set the guild fealty
*/
void cChar::SetGuildFealty(uint32_t newGuildFealty)
{
	guildFealty = newGuildFealty;
}

/*!
\brief return the guild number
\author Sparhawk
\date 31/08/2002
\since 0.82
\return guild number
*/
int32_t cChar::GetGuildNumber()
{
	return guildNumber;
}

/*!
\brief set the guild number
\author Sparhawk
\date 31/08/2002
\since 0.82
\param newGuildNumber guild number to set to the char
*/
void cChar::SetGuildNumber(int32_t newGuildNumber)
{
	guildNumber = newGuildNumber;
}

/*!
\brief return the guild title
\author Sparhawk
\date 31/08/2002
\since 0.82
\return guild title
*/
char* cChar::GetGuildTitle()
{
	return guildTitle;
}

/*!
\brief set the guild title
\author Sparhawk
\date 31/08/2002
\since 0.82
\param newGuildTitle guild title
*/
void cChar::SetGuildTitle(char* newGuildTitle)
{
	if ( strlen( newGuildTitle ) < sizeof( guildTitle ) )
		strcpy( guildTitle, newGuildTitle );
	else
	{
		strncpy( guildTitle, newGuildTitle, ( sizeof( guildTitle ) - 1 ) );
		guildTitle[ sizeof( guildTitle ) - 1 ] = '\0';
	}
}

