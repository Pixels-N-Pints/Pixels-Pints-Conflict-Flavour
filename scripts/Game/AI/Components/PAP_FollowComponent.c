modded class DAD_FollowComponent
{
	const protected string FOLLOW_WP_PATTERN = "Waypoint_Follow";
	const protected string GETIN_WP_PATTERN = "Waypoint_GetIn";
	const protected string WAIT_WP_PATTERN = "Waypoint_Wait";
	
	
	void ResetWaypoints()
	{
		AIGroup ai = GetAI();
		if (!ai) return;
		while (ai.GetCurrentWaypoint()) 
		{
			ai.CompleteWaypoint(ai.GetCurrentWaypoint());
		}
	}
	
	override void StopFollowing()
	{
		ResetWaypoints();
		
		m_User = null;
		UpdateIsFollowing();
	}
	
	override bool IsFollowing()
	{
		
		if (m_User && !SCR_AIDamageHandling.IsAlive(m_User)) 
		{
			Print("PAP_FollowComponent:IsFollowing() | User is dead... units unfollowing");
			return false;
		}
		
		AIGroup aiGroup = GetAI();
		if (!aiGroup)
		{
			Print("PAP_FollowComponent:IsFollowing() | Ai group not found");
			return false;
		} 
		
		ref array<AIWaypoint> aiWaypoints = {};
		aiGroup.GetWaypoints(aiWaypoints);
		
		if (!aiWaypoints || aiWaypoints.IsEmpty()) return false;
		else
		{
			foreach (AIWaypoint wp : aiWaypoints)
			{
				EntityPrefabData prefab = wp.GetPrefabData();
				if (prefab && prefab.GetPrefabName().Contains(FOLLOW_WP_PATTERN))
				{
					Print("PAP_FollowComponent:IsFollowing() | At least one unit is still following");
					return true;
				} 
			}
		}
		Print("PAP_FollowComponent:IsFollowing() | Units assumed to not be following");
		return false;
	}
	
	override void UpdateIsFollowing()
	{
		AIGroup aiGroup = GetAI();
		
		if (!aiGroup)
		{	
			Print("PAP_FollowComponent:UpdateIsFollowing() | Ai group not found");
			return;
		}
		
		bool isFollowing = IsFollowing();
		ref array<AIAgent> agents = {};
		aiGroup.GetAgents(agents);
		foreach (AIAgent agent : agents)
		{
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
			char.SetIsFollowing(isFollowing);
		}
	}
	
	override void Update() {
		Print("PAP_FollowComponent:Update() | Updating...");
				
		UpdateIsFollowing();

		if (!IsFollowing())
		{
			Print("PAP_FollowComponent:Update() | No units following... ending queue");
			return;
		}
			
		if (m_User && !SCR_AIDamageHandling.IsAlive(m_User))
		{
			Print("PAP_FollowComponent:Update() | Player is not alive... ending queue");
			StopFollowing();
			return;
		}

		AIGroup ai = GetAI();
		if (!ai) return;

		Print("PAP_FollowComponent:Update() | Units are following");		

		ref array<AIAgent> agents = {};
		ai.GetAgents(agents);
		
		vector playerOrigin = m_User.GetOrigin();

		if (m_User.IsInVehicle())
		{
			bool groupNeedsToGetInVehicle = false;
			foreach (AIAgent a: agents)
			{
				ChimeraCharacter character = ChimeraCharacter.Cast(a.GetControlledEntity());
				if (character && !character.IsInVehicle())
				{
					Print("PAP_FollowComponent:Update() | At least one follower not embarked!");
					groupNeedsToGetInVehicle = true;
					break;
				}
			}
			if (groupNeedsToGetInVehicle)
			{
				Print("PAP_FollowComponent:Update() | Adding waypoints to mount vehicle if required!");
				if (!ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains(GETIN_WP_PATTERN) &&
				 	!ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains(WAIT_WP_PATTERN))
				{
					AIWaypoint getInWaypoint = AIWaypoint.Cast(SpawnHelpers.SpawnEntity(
						Resource.Load("{0A2A37B4A56D74DF}PrefabsEditable/Auto/AI/Waypoints/E_AIWaypoint_GetInNearest.et"),
						playerOrigin
					));
					AIWaypoint stayInCar = AIWaypoint.Cast(SpawnHelpers.SpawnEntity(
						Resource.Load("{90E783A161383314}PrefabsEditable/Auto/AI/Waypoints/E_AIWaypoint_Wait.et"),
						playerOrigin
					));
					ai.AddWaypointAt(stayInCar, 0);
					ai.AddWaypointAt(getInWaypoint, 0);
				}
			}
			
		}
		else
		{
			Print("PAP_FollowComponent:Update() | User not in a vehicle");
			while (ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains(GETIN_WP_PATTERN))
			{
				Print("PAP_FollowComponent:Update() | Removing Waypoint_GetIn waypoint");
				ai.RemoveWaypointAt(0);
			}
			
			while (ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains(WAIT_WP_PATTERN))
			{
				Print("PAP_FollowComponent:Update() | Removing Waypoint_Wait waypoint");
				ai.RemoveWaypointAt(0);
			}
		}
		
		QueueUpdate();

	}
	
	
	
	void QueueUpdate()
	{
		GetGame().GetCallqueue().CallLater(Update, 2 * 1000, false);
	}
	
}
