/**
	Entanglement Projectile
	@author Maikel 
*/

#include Projectile

local pR = 240;
local pG = 240;
local pB = 240;
local Speed = 125;
local IsStraight = true;
local SpellDamage = 5;
local Size = 15;
local ManaCost = 15;
local ox;
local oy;

local trailparticles;

public func InitEffect()
{
	SetLightRange(30, 70);
	SetLightColor(RGB(240, 240, 240));
	
	ox = GetX();
	oy = GetY();
	
	trailparticles =
	{
		Size = PV_Linear(5,0),
		BlitMode = GFX_BLIT_Additive,
		R = 240,
		G = 240,
		B = 240,
	};
}

public func TravelEffect(int time)
{
	DrawParticleLine("Flash", 0, 0, ox - GetX(), oy - GetY(), 2, 0, 0, 10, trailparticles);
	ox = GetX();
	oy = GetY();
}

public func HitEffect()
{
	var sphereparticle =
	{
		Alpha = PV_Linear(255, 0),
		Size = Size * 2,
		R = pR,
		G = pG,
		B = pB,
		BlitMode = GFX_BLIT_Additive,
	};
	CreateParticle("StarSpark", 0, 0, 0, 0, 7, sphereparticle, 4);
}

public func Hit()
{
	HitEffect();
	for (var o in FindObjects(Find_Distance(Size), Find_Func("CanBeHit", this)))
	{
		if (!o)
			continue;
		o->CreateEffect(FxEntanglement, 100, nil, this.shooter);	
		WeaponDamage(o, SpellDamage);
	}
	RemoveObject();
}


/*-- Entanglement Effect --*/

local FxEntanglement = new Effect
{
	Construction = func(object with)
	{
		if (this.Target == with)
			return this->Remove();	
		//Log("[%d]%v entangled with %v", FrameCounter(), this.Target, with);
		this.entangled_with = with;
		return;
	},	
	OnDamage = func(int damage, object by_obj)
	{
		if (by_obj != this.entangled_with || damage >= 0)
			return;		
		this.entangled_with->WeaponDamage(this.Target, -damage / 1000);
		//Log("[%d]%v received entanglement damage %d", FrameCounter(), this.Target, -damage);
		this->Remove();
		return;
	}
};

