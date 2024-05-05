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
		if (owner.GetCharacterController().IsPlayerControlled()) return false;
		return true;
	}

}