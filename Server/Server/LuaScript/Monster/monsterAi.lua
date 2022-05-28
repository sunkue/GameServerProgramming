
myId = -1;


function SetObjectId(id)
	myId = id
end


function EventPlayerMove(playerId)
	local playerPosX , playerPosY = API_GetPos(playerId);
	local myX , myY = API_GetPos(myId);
	
	if playerPosX == myX and playerPosY == myY then
		API_Chat(playerId, myId, "HELLO", 0);
		API_MoveRandomly(myId, 3);
		API_Chat(playerId, myId, "BYE", 2);
	end
end

