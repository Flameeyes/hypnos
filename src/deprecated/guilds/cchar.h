//@{
/*!
\name Guilds
*/
	private:
		pGuild guild; //!< the guild
		pGuildMember member; //!< the guild member info

	public:
		bool	isGuilded();
		void	setGuild( pGuild guild, pGuildMember member );
		pGuild	getGuild();
		pGuildMember getGuildMember();
//@}


