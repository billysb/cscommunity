
--[[
You can use this script as an example of what functions are implemented.
Due to the complex nature of binding c++ objects to lua things are limited.
--]]


-- This runs once lua is fully setup. because code outside of this function can run too early.
function OnLoad()
	Msg("Lua finished loading but gamerules is still loading.")
	
	-- Broken function to test:
	local TestTable = GetEntitiesByName("info_*")
	
	Msg("TestTable contains: " .. #TestTable)
end

function LevelInitPostEntity()
	-- Useless right now due to bindings being broken --
	Msg("Map has been init")
end

Msg("This will run on the script loading because its in the global space.")

function RoundStart()
	Msg("Game has started")
	GameRules:BroadcastSound("music/devtest/intensity_high/1.wav", -1)
end

function WaveRespawn()
	Msg("Hello developer console! Zombies just got respawned.")
end

function MyCustomLuaFunc()
	-- Test script will teleport all zombies to a specfic part of the map
	TeleportLocation = Vector(-2,-916,65)
	for i = 1, MAX_PLAYERS do
		PlayerEnt = EdictToPlayer(i) -- This returns a CBasePlayer and not a CCSPlayer!!
		
		if (PlayerEnt and PlayerEnt:GetTeamNumber() == TEAM_ZOMBIE and PlayerEnt:IsAlive()) then
			if (PlayerEnt:IsBot()) then
				BotEnt = PlayerToBot(PlayerEnt)
				BotEnt:ForgetNose()
				BotEnt:StopAiming()
				BotEnt:StopAttacking()
				BotEnt:ClearLookAt()
				BotEnt:Panic() -- Just testing these.
			end
			PlayerEnt:SetAbsOrigin(TeleportLocation)
		end
	end
	
	GameRules:BroadcastSound("Zombies.PissedOff", -1)
end

function OnTrigger(pTrigger, pOther)
	Msg("A map trigger got triggered!")
	
	-- Annoyingly some string based functions return string_t.. this is why we use ToCStr
	if (pTrigger:GetEntityName():ToCStr() == "MyAwesomeTrigger") then
		Msg("Our awesome trigger got triggered!")
		
		-- Check CBaseEntity is a CCSPlayer entity.
		if (pOther:IsPlayer()) then
			-- Get the CCSPlayer entity from our CBaseEntity
			PlayerEnt = EntToPlayer(pOther)

			if (PlayerEnt) then
				-- Now we wanna put zombies on your ass. --
				MyCustomLuaFunc()
			end

		end

	end
end