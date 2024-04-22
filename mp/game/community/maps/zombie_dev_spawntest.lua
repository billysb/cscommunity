
--[[
You can use this script as an example of what functions are implemented.
Due to the complex nature of binding c++ objects to lua things are limited.
--]]

Msg("zombie_dev_spawntest.lua script is running")
Msg("zombie_dev_spawntest.lua script is running")
Msg("zombie_dev_spawntest.lua script is running")
Msg("zombie_dev_spawntest.lua script is running")
Msg("zombie_dev_spawntest.lua script is running")

-- Obvious.
function RoundStart()
	Msg("The lua script got roundstart()!!!!")
	if (IsTerrorStrike()) then
		Msg("Yep this is terror strike alright.")
	else
		Msg("This shouldnt run but eh")
	end
	
	if (GameRules:IsBombDefuseMap()) then
		Msg("This is a bomb defuse map")
	end
	GameRules:BroadcastSound("Zombies.PissedOff", -1)
end

-- Called when zombies respawn.
function WaveStart()
	Msg("Zombie Respawn wave ran.")
end

function OnBoomerSpawn(pPlayer)
	Msg("We got a boomer! " .. pPlayer:GetTeamNumber())
	
	-- Here we could manipulate things for this special infected. --
end