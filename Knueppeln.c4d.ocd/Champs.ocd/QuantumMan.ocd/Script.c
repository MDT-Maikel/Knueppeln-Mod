/**
	Quantum Man
	@author 
*/

#include Man


/*-- Spells --*/

local Special1Spell = EntanglementProjectile;
local Special2Spell = TunnellingBarrier;
local Special3Spell = nil;

local Special1Cooldown = 8;
local Special2Cooldown = 180;
local Special3Cooldown = 540;

public func Special1(object clonk, int x, int y, bool released, bool mouseclick, bool abletocast, bool cooldown)
{
	if (!released && !mouseclick && abletocast && !cooldown)
	{
		if (clonk->LaunchSpell(Special1Spell, x, y, 0, 0))
			return  true;
	}
	return false;
}

public func Special2(object clonk, int x, int y, bool released, bool mouseclick, bool abletocast, bool cooldown)
{
	var props =
	{
		R = 50,
		G = 255,
		B = 50,
		Alpha = 40,
		Size = Special2Spell.SpellRange * 2,
		BlitMode = GFX_BLIT_Additive,
		Rotation = PV_Step(10, 0, 1),
		Attach = ATTACH_Back | ATTACH_MoveRelative
	};
	return CastSpellWithSpellRange(clonk, x, y, released, mouseclick, abletocast, cooldown, props, Special2Spell);
}

public func Special3(object clonk, int x, int y, bool released, bool mouseclick, bool abletocast, bool cooldown)
{
	return false;
}

public func JumpEffect(object clonk, string dir)
{
	var from, to;
	if (dir == "Up")
	{
		from = 50; 
		to = 130;
	}
	if (dir == "Left")
	{
		from = -30; 
		to = 50;
	}
	if (dir == "Right")
	{
		from = 130; 
		to = 210;
	}
	if (dir == "Down")
	{
		from = 230; 
		to = 310;
	}

	for (var angle = from; angle <= to; angle += 5)
	{
		var r = 10;
		var x = clonk->GetX() + Cos(angle, r);
		var y = clonk->GetY() + Sin(angle, r);
		var trailparticles =
		{
			R = 240, G = 240, B = 240,
			Stretch = PV_Random(8000, 13000),
			Size = PV_Linear(3, 2),
			Alpha = PV_Linear(255, 0),
			Rotation = angle + 90,
			OnCollision = PC_Bounce(),
			BlitMode = GFX_BLIT_Additive
		};
		CreateParticle("StarSpark", x, y, Cos(angle, r), Sin(angle, r), 10, trailparticles);
	}

	return;
}

public func BlockEffect(object clonk, int radius)
{
	var border = 
	{
		R = 240, G = 240, B = 240,
		Stretch = PV_Random(8000, 13000),
		Size = PV_Linear(2, 1),
		Alpha = PV_Linear(255, 0),
		OnCollision = PC_Die(),
		CollisionVertex = 0,
		ForceX = PV_Linear(0, PV_Random(-10, 10, 10)),
		ForceY = PV_Linear(0, PV_Random(-10, 10, 10)),
		DampingX = 900, DampingY = 900,
		BlitMode = GFX_BLIT_Additive
	};
	
	for (var angle = 0; angle < 360; angle += 5)
	{
		border.Rotation = angle;
		clonk->CreateParticle("StarSpark", Sin(angle + RandomX(-5, 5), radius), -Cos(angle + RandomX(-5, 5), radius), 0, 0, PV_Random(24, 30), border, 12);
	}
	return;
}

public func InitChamp(object clonk)
{
	clonk->CreateEffect(FxDamageTracker, 100);
	clonk->CreateEffect(FxSuperposition, 100, 1);
	return;
}

public func CleanUp(object clonk)
{
	RemoveEffect("FxDamageTracker", clonk);
	return;
}


/*-- Entanglement Damage --*/

local FxDamageTracker = new Effect
{
	Damage = func(int damage)
	{
		//Log("[%d]%v damaged %d", FrameCounter(), this.Target, -damage);
		for (var obj in FindObjects(Find_OCF(OCF_Alive), Find_Exclude(this.Target)))
		{
			var fx, index = 0;
			while (fx = GetEffect("*Entanglement*", obj, index++))
				fx->~OnDamage(damage, Target);
		}
		return damage;
	}
};


/*-- Superposition Effect --*/

local FxSuperposition = new Effect
{
	Construction = func()
	{
		this.superposition = nil;
		this.fake = nil;
		this.super_time = 0;
		return;
	},	
	
	Timer = func(int time)
	{
		if (!this.superposition)
		{
			if (!Random(40))
			{
				this.superposition = CreateSuperposition();
				this.fake = CreateFake();
				this.Target.Visibility = VIS_All; //VIS_Allies;
				Effects();
			}
			return FX_OK;
		}
		this.super_time += 2;
		this.superposition->SetClrModulation(RGBa(255, 255, 255, Max(0, 255 - this.super_time)), GFX_Overlay);
		this.fake->SetClrModulation(RGBa(255, 255, 255, Min(255, this.super_time)), GFX_Overlay);
		this.Target->SetClrModulation(RGBa(255, 255, 255, Min(255, this.super_time)));
		if (this.super_time >= 255)
		{
			this.superposition->RemoveObject();
			this.fake->RemoveObject();
			this.super_time = 0;
			this.Target.Visibility = VIS_All;
		}
		return FX_OK;
	},
	
	Destruction = func()
	{
		if (this.superposition)	
			this.superposition->RemoveObject();
		if (this.fake)	
			this.fake->RemoveObject();
		this.Target.Visibility = VIS_All;	
		return;
	},
	
	CreateSuperposition = func()
	{
		var dummy = this.Target->CreateObject(Dummy);
		dummy.Plane = this.Target.Plane;
		dummy->SetCategory(C4D_StaticBack);
		dummy.Visibility = VIS_All;
		dummy->SetShape(-5, -5, 10, 10);
		dummy->SetVertexXY(0, this.Target->GetVertex(0, VTX_X), this.Target->GetVertex(0, VTX_Y));
		dummy->SetGraphics(nil, nil, GFX_Overlay, GFXOV_MODE_Object, nil, nil, this.Target);
		dummy->SetClrModulation(RGBa(255, 255, 255, 255), GFX_Overlay);
		return dummy;
	},
	
	CreateFake = func()
	{
		var dummy = this.Target->CreateObject(Dummy);
		dummy.ActMap = 
		{
			Attach = 
			{
				Name = "Attach",
				Procedure = DFA_ATTACH
			}
		};
		dummy.Plane = this.Target.Plane - 1;
		dummy.Visibility = VIS_All;//VIS_Enemies;
		dummy->SetShape(-5, -5, 10, 10);
		dummy->SetVertexXY(0, this.Target->GetVertex(0, VTX_X), this.Target->GetVertex(0, VTX_Y));
		dummy->SetAction("Attach", this.Target);		
		dummy->SetGraphics(nil, nil, GFX_Overlay, GFXOV_MODE_Object, nil, nil, this.Target);
		dummy->SetClrModulation(RGBa(255, 255, 255, 0), GFX_Overlay);
		return dummy;	
	},
	
	Effects = func()
	{
		var radius = 8;
		var border = 
		{
			R = 240, G = 240, B = 240,
			Stretch = PV_Random(8000, 13000),
			Size = PV_Linear(2, 1),
			Alpha = PV_Linear(185, 0),
			OnCollision = PC_Die(),
			CollisionVertex = 0,
			ForceX = PV_Linear(0, PV_Random(-10, 10, 10)),
			ForceY = PV_Linear(0, PV_Random(-10, 10, 10)),
			DampingX = 900, DampingY = 900,
			BlitMode = GFX_BLIT_Additive
		};
		
		for (var angle = 0; angle < 360; angle += 5)
		{
			border.Rotation = angle;
			this.Target->CreateParticle("StarSpark", Sin(angle + RandomX(-5, 5), radius), -Cos(angle + RandomX(-5, 5), radius), 0, 0, PV_Random(10, 16), border, 8);
		}
		return;	
	}
};


/*-- AI --*/

public func IsSpecial1Shot() { return true; }
public func IsSpecial1ShotStraight() { return true; }
public func IsSpecial1ShotSpeed() { return Special1Spell.Speed; }

public func ExecuteAISpecial2Spell(effect fx)
{
	if (!fx.Target || !fx.target)
		return false;
	var x = fx.Target->GetX(), y = fx.Target->GetY();
	var tx = fx.target->GetX(), ty = fx.target->GetY();
	var dt = 5;
	tx += fx.control->GetTargetXDir(fx.target, dt);
	ty += fx.control->GetTargetYDir(fx.target, dt);
	if (fx.Target->LaunchSpecial2(tx - x, ty - y, false, true, fx.Target->CanCast() && this->CanCastSpecial3(fx.Target)))
		return true;
	return false;
}


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local Spell1Name = "$Spell1Name$";
local Spell1Description = "$Spell1Description$";
local Spell2Name = "$Spell2Name$";
local Spell2Description = "$Spell2Description$";
local Spell3Name = "$Spell3Name$";
local Spell3Description = "$Spell3Description$";
local PassiveName = "$PassiveName$";
local PassiveDescription = "$PassiveDescription$";