
myId = nil
targetId = nil
state = nil
damage = nil


function SetObjectId(id)
	myId = id
end

function Update()
	state()
end

function Idle()
	Move()
end

function Battle()
	Attack()
end

function Return()
	if API_ReturnToStartPoint(myId) then
		state = Idle
	end
end

state = Idle


