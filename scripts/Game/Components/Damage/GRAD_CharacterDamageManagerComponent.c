//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	protected override void OnDamage(notnull BaseDamageContext damageContext)
	{	
		//PrintFormat("DamageManager OnDamage HitZone: %1 (%4) Damage: %2 Type: %3", pHitZone.GetName(), damage, type, pHitZone.GetHealth());

		SCR_HitZone scriptedHz = SCR_HitZone.Cast(damageContext.struckHitZone);
		if (!scriptedHz)
			return;

		IEntity hzOwner = scriptedHz.GetOwner();
		if (!hzOwner)
			return;
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(hzOwner);
		SCR_PlayerController playerController =  SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;

		//Print(string.Format("GRAD Undead - isPossessing: %1", playerController.IsPossessing()), LogLevel.NORMAL);

		if (EntityUtils.IsPlayer(hzOwner) && !playerController.IsPossessing())
		{
			if(GetDefaultHitZone().GetHealthScaled() < 0.01)
			{
				//PrintFormat("Health HZ reaches 1% threshold: %1", GetHealthScaled());
				
				GetDefaultHitZone().SetHealthScaled(0.01);
				
				//Print("Set Health to 1%");
			}
		}

		// if damage type is 'collision' which could be for example fall damage
		// the the damage is also applied on all 6 leg parts to create treatable injuries
		
		if (damageContext.damageType == EDamageType.COLLISION)
		{
			array<HitZone> hitZones = {};
			GetAllHitZones(hitZones);
			
			foreach (HitZone hitZone : hitZones)
			{
				float fallDamage;
				
				switch (hitZone.GetName())
				{
					case "RThigh":
						fallDamage = damageContext.damageValue * 0.1;
						break;
					case "LThigh":
						fallDamage = damageContext.damageValue * 0.1;
						break;
					case "RCalf":
						fallDamage = damageContext.damageValue * 0.3;
						break;
					case "LCalf":
						fallDamage = damageContext.damageValue * 0.3;
						break;
					case "RFoot":
						fallDamage = damageContext.damageValue * 0.1;
						break;
					case "LFoot":
						fallDamage = damageContext.damageValue * 0.1;
						break;
					default:
						break;
				}
				
				float health = hitZone.GetHealth();
				
				float newHealth = health - fallDamage;
				
				if (newHealth <= 0)
					hitZone.SetHealthScaled(0.01);
				else
					hitZone.SetHealth(newHealth);
				
				// if the collision damage is above threshold then
				// the character gets unconscious for a certain amount of time
				
				// a damage of 25 is equal to a fall from a height of roughly 8-10 meters
				if (damageContext.damageValue > 25)
				{
					ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
					if (!character)
						return;
					
					CharacterControllerComponent controller = character.GetCharacterController();
					if (!controller)
						return;
		
					controller.SetUnconscious(true);
					
					// wake up after 15 sec
					GetGame().GetCallqueue().CallLater(controller.SetUnconscious, 15000, false, false);
				}
			}
		}
		
		super.OnDamage(damageContext);
	}
};
