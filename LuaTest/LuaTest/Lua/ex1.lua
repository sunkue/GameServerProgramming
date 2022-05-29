print("ex1 -")

myId = -1

function SetObjectId(id)
	myId = id
end

function EventPlayerMove(playerId)
	API_Chat(playerId, myId, "TT");
	local playerPosX , playerPosY = API_GetPos(playerId);
	local myX , myY = API_GetPos(myId);
	
	if playerPosX == myX and playerPosY == myY then
		API_Chat(playerId, myId, "HELLO");
	end
end

state = nil

function Update()
	state()
end

function Idle()
	Move()
end

function Battle()
	Chase()
end

print("- ex1")