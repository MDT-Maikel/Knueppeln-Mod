public func SaveScenarioObject() { return false; }

local home_block;
local hook;
local line;

public func Construction()
{
	var particles = Particles_Dust();
	CreateParticle("Smoke", 0, 0, PV_Random(-10, 10), PV_Random(-10, 10), PV_Random(0, 20), particles, 40);
	
	hook = CreateObject(Tile_ConveyorArmHook, 0, 0, NO_OWNER);
	hook.base = this;
	
	line = CreateObject(ElevatorRope, 0, 0, NO_OWNER);
	line->SetAction("Be", hook);
	line.Plane = 9;
}

public func Destruction()
{
	if (line) line->RemoveObject();
	if (hook) hook->RemoveObject();
	
	var particles = Particles_Dust();
	CreateParticle("Smoke", 0, 0, PV_Random(-5, 5), PV_Random(-5, 5), PV_Random(0, 10), particles, 40);
}

local MoveEffect = new Effect
{
	Timer = func()
	{
		if (!this.final_target) return FX_Execute_Kill;
		if (!this.current_block) return FX_Execute_Kill;
		var current_x = this.Target->GetX();
		var current_y = this.Target->GetY();
		
		if (!this.current_target)
		{
			var final_target_x = this.final_target->GetX();
			var final_target_y = this.final_target->GetY();
			var dir_x = BoundBy(final_target_x - current_x, -1, 1);
			var dir_y = BoundBy(final_target_y - current_y, -1, 1);
			var points = [];
			if (dir_x != 0) PushBack(points, [dir_x, 0]);
			if (dir_y != 0) PushBack(points, [0, dir_y]);
			
			PushBack(points, [0, -1]);
			PushBack(points, [0, 1]);
			PushBack(points, [1, 0]);
			PushBack(points, [-1, 0]);
			var neighbours = this.current_block->GetNeighboursAsMatrix();
			
			for (var point in points)
			{
				var off_x = point[0];
				var off_y = point[1];
				if (off_x == nil || off_y == nil) continue;
				var neighbour = neighbours[1 + off_x][1 + off_y];
				if (!neighbour || neighbour == this.last_target) continue;
				this.current_target = neighbour;
				break;
			}
			if (!this.current_target) return FX_Execute_Kill;
		}
		var target_x = this.current_target->GetX();
		var target_y = this.current_target->GetY();
		var move_x = BoundBy(target_x - current_x, -2, 2);
		var move_y = BoundBy(target_y - current_y, -2, 2);
		var new_x = this.Target->GetX() + move_x;
		var new_y = this.Target->GetY() + move_y;
		this.Target->SetPosition(new_x, new_y);
		this.Target.hook->SetPosition(new_x, new_y);
		
		if (move_x == 0 && move_y == 0)
		{
			if (this.current_target == this.final_target)
			{
				this.Target->RemoveObject();
				return FX_Execute_Kill;
			}
			this.last_target = this.current_block;
			this.current_block = this.current_target;
			this.current_target = nil;
		}
		
		return FX_OK;
	}
};

public func StartMoving(object payload)
{
	if (!payload || !home_block) return;
	
	var target_block = DiscoverTarget(home_block, payload);
	if (!target_block)
	{
		ScheduleCall(this, "StartMoving", 30, nil, payload);
		return;
	}
	
	if (this.line)
		this.line->RemoveObject();
	
	var fx = CreateEffect(MoveEffect, 1, 1);
	fx.final_target = target_block;
	fx.current_block = home_block;
}

private func DiscoverTarget(object from, object payload)
{
	from.already_found = true;
	var blocks = [from];
	var target = nil;
	for (var i = 0; i < GetLength(blocks); ++i)
	{
		if (target) break;
		var block = blocks[i];
		for (var neighbour in block->GetNeighbours(true))
		{
			neighbour.already_found = true;
			PushBack(blocks, neighbour);
			
			if (neighbour->IsTarget(from, payload))
			{
				target = neighbour;
				break;
			}
		}
	}
	
	for (var block in blocks)
		block.already_found = false;
	return target;
}
