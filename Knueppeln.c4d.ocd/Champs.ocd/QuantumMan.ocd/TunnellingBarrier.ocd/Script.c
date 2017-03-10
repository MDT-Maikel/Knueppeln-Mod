/**
	Tunnelling Barrier
	@author Maikel 
*/

local ManaCost = 40;
local SpellRange = 100;
local Duration = 120;


public func Launch(object clonk, int x, int y)
{
	this->CreateEffect(FxTunnellingBarrier, 1, 1, x, y);
	return true;
}

public func IsProjectileTarget(object target, object shooter)
{
	var fx = GetEffect("FxTunnellingBarrier", this);
	if (fx)
		fx->OnBarrierHit(target);
	// Don't let the projectile know this is a target.
	return false;
}

public func IsBarrierFor(int plr)
{
	return Hostile(plr, GetController());
}

local FxTunnellingBarrier = new Effect
{
	Construction = func(int x, int y)
	{
		this.angle = Angle(0, 0, x, y);
		this.length = (this.Target->GetRight() - this.Target->GetLeft()) / 2;
		this.x1 = Cos(this.angle, this.length);
		this.y1 = Sin(this.angle, this.length);
		this.x2 = -Cos(this.angle, this.length);
		this.y2 = -Sin(this.angle, this.length);
		this.Target->SetR(this.angle);
		
		this.particle_barrier = 
		{
			Stretch = PV_Random(8000, 13000),
			Size = PV_Linear(3, 1),
			Alpha = PV_Random(80, 0, 3),
			BlitMode = GFX_BLIT_Additive,
			Rotation = this.angle,
			R = 240, G = 240, B = 240,
			OnCollision = PC_Die(),
		};
		return FX_OK;
	},
	
	Timer = func(int time)
	{
		if (time > this.Target.Duration)
			return FX_Execute_Kill;
		this->Effects();
		return FX_OK;
	},
	
	Destruction = func()
	{
		this.Target->RemoveObject();	
		return FX_OK;
	},
	
	OnBarrierHit = func(object obj)
	{
		if (!Hostile(obj->GetController(), this.Target->GetController()))
			return;
		var xdir = obj->GetXDir(100);
		var ydir = obj->GetYDir(100);
		var speed = Sqrt(xdir**2 + ydir**2);
		var obj_angle = Angle(0, 0, xdir, ydir);
		var angle_diff = GetTurnDirection(obj_angle - 180, this.angle);
		var new_angle = this.angle + angle_diff;
		obj->SetVelocity(new_angle, speed, 1, 100);
		obj->SetController(this.Target->GetController());
		return;
	},
	
	Effects = func()
	{
		// Create some particles.
		for (var cnt = 0; cnt < 16; cnt++)
		{
			var d = Random(this.length);
			var x = this.x1 + (this.x2 - this.x1) * d / this.length;
			var y = this.y1 + (this.y2 - this.y1) * d / this.length;
			if (!this.Target->GBackSemiSolid(x, y))
				this.Target->CreateParticle("StarSpark", PV_Random(x - 4, x + 4), PV_Random(y - 4, y + 4), 0, 0, 10, this.particle_barrier, 8);
		}
		return;
	}
};