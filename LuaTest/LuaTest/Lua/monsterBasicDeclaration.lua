
myId = nil
targetId = nil
state = nil
damage = nil

print("Basic");

function SetObjectId(id)
	myId = id
end

function Update()
	print("U")
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

print("Basic");

state = Idle


