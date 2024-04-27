modded class DAD_FollowComponent
{
	const protected string FOLLOW_WP_PATTERN = "Waypoint_Follow";
	const protected string GETIN_WP_PATTERN = "Waypoint_GetIn";
	const protected string WAIT_WP_PATTERN = "Waypoint_Wait";
	
	private int m_Semaphore = 1;
	private int m_delay = 2;
	
	array<AIWaypoint> GetWaypoints(AIGroup aiGroup)
	{
		if (!aiGroup) return null;
		array<AIWaypoint> waypoints = {};
		aiGroup.GetWaypoints(waypoints);
		return waypoints;
	}
	
	void ResetFormation()
	{
		AIGroup ai = GetAI();
		if (!ai) return;	
		
		AIFormationComponent formationComponent = ai.GetFormationComponent();
		
		if (formationComponent)
		{
			Print("PAP_FollowComponent:ResetFormation() | Resetting displacement...");
			formationComponent.SetFormationDisplacement(0);
		}
			
	}
	
	void ResetWaypoints()
	{
		AIGroup ai = GetAI();
		if (!ai) return;
		
		array<AIWaypoint> currentWaypoints = {};
		ai.GetWaypoints(currentWaypoints);
		
		foreach (AIWaypoint currentwp : currentWaypoints)
		{
			ai.CompleteWaypoint(currentwp);
		}
		
		ResetFormation();
		
	}
	
	void AddFollowWaypoint()
	{
		AIGroup ai = GetAI();
		if (!ai) return;
		Resource wpRes = Resource.Load(m_WaypointType);
		SCR_EntityWaypoint followWaypoint = SCR_EntityWaypoint.Cast(SpawnHelpers.SpawnEntity(wpRes, m_User.GetOrigin()));
		followWaypoint.SetEntity(m_User);
		ai.AddWaypointAt(followWaypoint, 0);
		
	}
	
	override void Follow(SCR_ChimeraCharacter char)
	{
		RplComponent rplC = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rplC.IsOwner()) return;

		ResetWaypoints();
		
		m_User = SCR_ChimeraCharacter.Cast(char);

		AddFollowWaypoint();
		Update();
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
		
		array<AIWaypoint> aiWaypoints = GetWaypoints(aiGroup);
		
		if (!aiWaypoints || aiWaypoints.IsEmpty()) return false;
		else
		{
			Print("PAP_FollowComponent:IsFollowing() | AI group waypoints:");
			Print(aiWaypoints);
			foreach (AIWaypoint wp : aiWaypoints)
			{
				EntityPrefabData prefab = wp.GetPrefabData();
				if (prefab && prefab.GetPrefabName().Contains(FOLLOW_WP_PATTERN))
				{
					return true;
				} 
			}
		}
		Print("PAP_FollowComponent:IsFollowing() | Units assumed to not be following");
		return false;
	}
	
	bool SemaphoreWait()
	{
		m_Semaphore--;
		return m_Semaphore >= 0;
	}
	
	void SemaphoreRelease()
	{
		m_Semaphore++;
	}
	
	override void UpdateIsFollowing()
	{
		
			
		AIGroup aiGroup = GetAI();
			
		if (!aiGroup)
		{	
			Print("PAP_FollowComponent:UpdateIsFollowing() | Ai group not found");
			return;
		}
		
		bool canAccess = SemaphoreWait();
		if (!canAccess) return;
		
		bool isFollowing = IsFollowing();
		ref array<AIAgent> agents = {};
		aiGroup.GetAgents(agents);
		
		foreach (AIAgent agent : agents)
		{
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
			char.SetIsFollowing(isFollowing);
		}
		
		SemaphoreRelease();
	}
	
	override void Update() {
		Print("PAP_FollowComponent:Update() | Updating...");
		m_delay = 2;
				
		UpdateIsFollowing();

		if (!m_User || !IsFollowing())
		{
			Print("PAP_FollowComponent:Update() | No units following or nobody to folow... ending queue");
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
					ResetFormation();
					
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
			bool groupIsOutOfVehicle = false;
			foreach (AIAgent a: agents)
			{
				ChimeraCharacter character = ChimeraCharacter.Cast(a.GetControlledEntity());
				if (character && !character.IsInVehicle())
				{
					// Print("PAP_FollowComponent:Update() | At least one follower is disembarked!");
					groupIsOutOfVehicle = true;
					break;
				}
			}
			if (!groupIsOutOfVehicle)
			{
				ResetWaypoints();
				AddFollowWaypoint();
				m_delay = 5;
			}
		}
		
		
		QueueUpdate();
	}
	
	
	

	void QueueUpdate()
	{
		GetGame().GetCallqueue().CallLater(Update, m_delay * 1000, false);
	}
	
}
