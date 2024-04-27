modded class DAD_FollowAction {

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (IsFollowing())
			outName = "Stop following me";
		else
			outName = "Follow me";

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(user);
		
		if (!owner.GetFaction().IsFactionFriendly(player.GetFaction())) return false;
		if (owner.GetID() == player.GetID()) return false;
		// TODO: fix
		/*
		switch (SCR_CharacterRankComponent.GetCharacterRank(user)) {
			case SCR_ECharacterRank.CORPORAL:
			case SCR_ECharacterRank.SERGEANT:
			case SCR_ECharacterRank.LIEUTENANT:
			case SCR_ECharacterRank.CAPTAIN:
			case SCR_ECharacterRank.MAJOR:
			case SCR_ECharacterRank.COLONEL: // Experimental ranks
	  		case SCR_ECharacterRank.GENERAL: // *
				return true;
			default:
				return false;
				
		}
		*/
		return true;
	}

}