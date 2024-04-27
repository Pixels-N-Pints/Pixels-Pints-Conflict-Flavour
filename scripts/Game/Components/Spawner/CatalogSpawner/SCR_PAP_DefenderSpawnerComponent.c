modded class SCR_DefenderSpawnerComponentClass
{
	[Attribute(defvalue: "{531EC45063C1F57B}Prefabs/AI/Waypoints/AIWaypoint_Wait.et", UIWidgets.ResourceNamePicker, desc: "Default waypoint prefab", "et", category: "Defender Spawner")]
	protected ResourceName m_sDefaultWaypointPrefab;
	
	[Attribute(defvalue :"StaggeredColumn", UIWidgets.Auto, desc: "Group formation created on rally point. Available formations are found on SCR_AIWorld entity", category: "Defender Spawner")]
	protected string m_sExitFormation;

}