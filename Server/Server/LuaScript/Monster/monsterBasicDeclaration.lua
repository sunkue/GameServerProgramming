
myId = nil
targetId = nil
state = nil


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

state = Idle


