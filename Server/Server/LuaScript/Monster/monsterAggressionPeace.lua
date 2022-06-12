damage = 3

function EventPlayerEnterSight(playerId)
	API_Chat(myId, playerId, "(@)Hi~", 0);
end

function EventPlayerExitSight(playerId)
	if targetId == playerId then
		state = Idle
		targetId = nil
	end
end

function EventPlayerStaySight(playerId)
end

function EventBeAttacked(playerId)
	if targetId == nil then
		API_Chat(myId, playerId, "(@)!!", 0);
		targetId = playerId
		state = Battle
	end
end

onNavigate = false

function EventTargetMissing()
	API_Chat(myId, targetId, "(@)..?", 0)
	if targetId ~= nil then
		API_Chat(myId, targetId, "(@).><", 0)
		targetId = nil
		state = Idle
		onNavigate = false;
	end
end

function EventNaviagateDone()
	onNavigate = false
end

function Attack()
	local targetX , targetY = API_GetPos(targetId);
	local myX , myY = API_GetPos(myId);
	-- attackable range
	if math.abs(targetX - myX) <= 1 and math.abs(targetY - myY) <= 1 then
		API_Chat(myId, targetId, "(@)smash!", 0)
		API_Attack(myId, targetId, damage)
	-- non attackable range
	else
		if onNavigate or not moveable then
			API_Chat(myId, targetId, "(@)grr..", 0)
		else
			API_NavigateAstar(myId, targetX, targetY, 3)
			onNavigate = true
		end
	end
end

