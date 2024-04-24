modded class DAD_FollowComponent
{
	const protected string FOLLOW_WP_PATTERN = "Waypoint_Follow";
	const protected string GETIN_WP_PATTERN = "Waypoint_GetIn";
	const protected string WAIT_WP_PATTERN = "Waypoint_Wait";
	
	override void StopFollowing()
	{
		ResetWaypoints();
		
		m_User = null;
		UpdateIsFollowing();
	}
	
	override bool IsFollowing()
	{
		bool isFollowing;
		SCR_ChimeraCharacter owner = SCR_ChimeraCharacter.Cast(GetOwner());

		AIWaypoint wp = GetAI().GetCurrentWaypoint();
		if (!wp || !SCR_AIDamageHandling.IsAlive(m_User))
		{
			isFollowing = false;	
		}
		else
		{
			EntityPrefabData prefab = wp.GetPrefabData();
			isFollowing = prefab && prefab.GetPrefabName().Contains(FOLLOW_WP_PATTERN);
		}
		return isFollowing;
	}
	
	override void Update() {
		Print("DAD_FollowComponent: Updating");
				
		UpdateIsFollowing();

		if (!IsFollowing()) return;
			
		if (!SCR_AIDamageHandling.IsAlive(m_User))
		{
			Print("DAD_FollowComponent: Player is not alive");
			StopFollowing();
			return;
		}


		Print("Following!");		
		AIGroup ai = GetAI();
		if (!ai) 
		{
			QueueUpdate();
			return;
		}

		vector playerOrigin = m_User.GetOrigin();

		ref array<AIAgent> agents = {};
		ai.GetAgents(agents);

		if (m_User.IsInVehicle())
		{
			bool groupNeedsToGetInVehicle = false;
			foreach (AIAgent a: agents)
			{
				ChimeraCharacter character = ChimeraCharacter.Cast(a.GetControlledEntity());
				if (character && !character.IsInVehicle())
				{
					Print("At least one follower not embarked!");
					groupNeedsToGetInVehicle = true;
					break;
				}
			}
			if (groupNeedsToGetInVehicle)
			{
				Print("Adding waypoints to mount vehicle!");
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
		else
		{
			while (ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains(GETIN_WP_PATTERN))
			{
				Print("Removing Waypoint_GetIn waypoint");
				ai.RemoveWaypointAt(0);
			}
			
			while (ai.GetCurrentWaypoint().GetPrefabData().GetPrefabName().Contains(WAIT_WP_PATTERN))
			{
				Print("Removing Waypoint_Wait waypoint");
				ai.RemoveWaypointAt(0);
			}
		}
		
		QueueUpdate();

	}
	
	void ResetWaypoints()
	{
		AIGroup ai = GetAI();
		while (ai.GetCurrentWaypoint()) 
		{
			ai.CompleteWaypoint(ai.GetCurrentWaypoint());
		}
	}
	
	void QueueUpdate()
	{
		Print("Queuing Update()");
		GetGame().GetCallqueue().CallLater(Update, 2 * 1000, false);
	}
	
}
