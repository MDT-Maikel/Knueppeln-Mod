/**
	FireMan
	

	@author 
*/

#include Man

func LaunchSpecial1(object clonk, int x, int y, bool released, bool mouseclick)
{
	if(!released && !mouseclick)
		clonk->LaunchSpell(ElectroProjectile, x, y, 0, 0);
}

func LaunchSpecial2(object clonk, int x, int y, bool released, bool mouseclick)
{
	if(!released && !mouseclick)
		clonk->LaunchSpell(ElectroOrb, x, y, 0, 0);
}

func LaunchSpecial3(object clonk, int x, int y, bool released, bool mouseclick)
{
	if(!released && !mouseclick)
	{
		var props =
		{
			R = 150,
			G = 215,
			B = 255,
			Alpha = 40,
			Size = ThunderStrike.SpellRange*2 + ThunderStrike.SpellRange/50*20,
			BlitMode = GFX_BLIT_Additive,
			Rotation = PV_Step(10, 0, 1),
			Attach = ATTACH_Back | ATTACH_MoveRelative
			
		};
		clonk->ShowSpellRange(clonk, ThunderStrike, props);
	}
	
	if(released && !mouseclick)
	{
		clonk->CancelShowSpellRange();
	}

	if(!released && mouseclick)
	{
		if (Sqrt(x**2 + y**2) > ThunderStrike.SpellRange)
		{
			Sound("UI::Error", false, 50, clonk->GetOwner());
			return 1;
		}
		
		clonk->LaunchSpell(ThunderStrike, x, y, x, y);
	}
}

func JumpEffect(object clonk, dir)
{
	var from;
	var to;

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

	Sound("electric_shot", false, 30);

	for(var i = from; i < to; i+=5)
	{
		var r = 10;
		var x = clonk->GetX() + Cos(i, r);
		var y = clonk->GetY() + Sin(i, r);
		
		var angle = Angle(0,0,x,y);
		
		var trailparticles =
		{
			Size = PV_Linear(15,0),
			Rotation = angle
		};
	
		CreateParticle("BlueSpark", x, y, Cos(i, r), Sin(i, r), 10, trailparticles);
	}
}

func BlockEffect(object clonk, range)
{
	for(var i = 0; i < 360; i+=10)
	{
		var r = range;
		var x = clonk->GetX() + Cos(i, r);
		var y = clonk->GetY() + Sin(i, r);
		
		var angle = Angle(0,0,x,y) + 90;
		
		var trailparticles =
		{
			Size = PV_Linear(15,0),
			Rotation = angle
		};
	
		CreateParticle("BlueSpark", x, y, 0, 0, 10, trailparticles);
	}
	
}

func ShowRange()
{

}