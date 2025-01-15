# Gameplay Ability System Extension
This project includes the **Gameplay Ability System Extension** plugin and accompanying sample content, designed to enhance Unreal Engine's Gameplay Ability System (GAS).
The **Gameplay Ability System Extension** plugin consolidates useful features found in sample projects like Lyra and ActionRPG. It provides a simplified, easy-to-use version of these tools, making it more accessible for smaller projects. 
Unlike Lyra, which can be overly large and complex for small-scale projects, this plugin has only the essential functionality, reducing unnecessary complexity.

The project is developed with Unreal Engine 5.4

# Content
`Plugins/GameplayAbilitySystemExtension`: Core elements in this project. Copy this and dependencies to your project.

`Plugins/ModularGameplayActors`: GameplayAbilitySystemExtension plugin depends on this.

`Other content like Source/ and Content/`: Example and test content. You don't need these unless you want to see examples.

# Features
- **single-player projects only**: For multiplayer projects, why don't you just use Lyra?
- InputAction-GameplayAbility binding system from Lyra (InputConfig and AbilitySet). There are also PawnData and AbilityTagRelationshipMapping.
- Custom ability cost
- Custom cooldown
- Interaction ability system
- Useful AbilityTasks and AsyncTasks
- TargetTypes from ActionRPG, which is useful to find targets for GameplayAbilities with simple logic.
- EffectContainer from ActionRPG.

# Setup
- Set GASXInputComponent as Default Input Component Class in Project Settings 
- Use AGASXPlayerController and AGASXPlayerState, or their subclasses.
- Subclass AGASXBaseCharacter for your character
- Your GameplayAbilities must inherit from UGASXGameplayAbility

## Experience
- Add following types to Primary Asset Types to Scan in Project Settings: Map, GASXExperienceDefinition, GASXUserFacingExperienceDefinition, GASXExperienceActionSet
- Set GASXWorldSettings as Default World Settings Class.
- Use GASXGameMode or its subclass as your GameMode. 

### Experience Precedence order (highest wins)
- URL Options override: Used if you use GASXUserFacingExperienceDefinition to travel.
- Command Line override
- World Settings
- Default experience from Project Settings (Gameplay Ability System Extension section)

NOTE: Unlike Lyra, You have to make GASXExperienceDefinitions as DATA ASSETs, not pure blueprint subclasses.
