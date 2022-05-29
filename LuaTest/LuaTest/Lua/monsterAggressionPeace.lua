print("Peace");

function EventPlayerEnterSight(playerId)
	API_Chat(playerId, myId, "Hi~!", 0);
end

function EventBeAttacked(playerId)
	if targetId == nil then
		API_Chat(playerId, myId, speechOnBattleBegin, 0);
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
		API_Chat(playerId, myId, speechOnChaseFail, 0)
		return
	end

	-- attackable range
	if abs(playerPosX - myX) < 1 and abs(playerPosY - myY) < 1 then
		API_Attack(myId, playerId, damage)
		return
	-- non attackable range
	else
		API_Move(myId, navigateX, navigateY);
		return
	end
end

print("Peace");
