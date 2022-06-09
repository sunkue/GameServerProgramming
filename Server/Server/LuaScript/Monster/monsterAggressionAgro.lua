damage = 5

function EventPlayerEnterSight(playerId)
end

function EventPlayerExitSight(playerId)
	if targetId == playerId then
		state = Return
		targetId = nil
	end
end

function EventPlayerStaySight(playerId)
	local playerX , playerY = API_GetPos(playerId);
	local myX , myY = API_GetPos(myId);
	if math.abs(playerX - myX) <= 5 and math.abs(playerY - myY) <= 5 then
		if targetId == nil then
			API_Chat(myId, playerId, "!!", 0);
			targetId = playerId
			state = Battle
		end
	end
end






function EventBeAttacked(playerId)
	if targetId == nil then
		API_Chat(myId, playerId, "!!", 0);
		targetId = playerId
		state = Battle
	end
end

function Attack()
	local targetX , targetY = API_GetPos(targetId);
	local myX , myY = API_GetPos(myId);
	local navigateX, navigateY = API_NavigateAstar(myX, myY, targetX, targetY)
	
	-- fail navigate target
	if navigateX < 0 then
		targetId = nil
		state = Idle
		return
	end

	-- attackable range
	if math.abs(targetX - myX) <= 1 and math.abs(targetY - myY) <= 1 then
		API_Chat(myId, targetId, "smash!", 0)
		API_Attack(myId, targetId, damage)
		return
	-- non attackable range
	else
		API_Chat(myId, targetId, "grr..", 0)
		API_Move(myId, navigateX, navigateY);
		return
	end
end
