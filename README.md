# Gameplay Ability System Extension
This project includes the **Gameplay Ability System Extension** plugin and accompanying sample content, designed to enhance Unreal Engine's Gameplay Ability System (GAS).
The **Gameplay Ability System Extension** plugin consolidates useful features found in sample projects like Lyra and ActionRPG. It provides a simplified, easy-to-use version of these tools, making it more accessible for smaller projects. 
Unlike Lyra, which can be overly large and complex for small-scale projects, this plugin has only the essential functionality, reducing unnecessary complexity.

The project is developed with Unreal Engine 5.4

# Content
`Plugins/GameplayAbilitySystemExtension`: Core elements in this project. Copy this and dependencies to your project.

`Plugins/ModularGameplayActors`: Copied from Lyra. GameplayAbilitySystemExtension plugin depends on this.

`Plugins/CommonLoadingScreen`: Copied from Lyra. You don't need this. It is just for showing loading screen while loading experience.

`Other content like Source/ and Content/`: Example and test content. You don't need these unless you want to see examples.

# Features
- **single-player projects only**: For multiplayer projects, why don't you just use Lyra?
- Experience system from Lyra.
- InputAction-GameplayAbility binding system from Lyra (InputConfig and AbilitySet). There are also PawnData and AbilityTagRelationshipMapping.
- Custom ability cost
- Custom cooldown
- Interaction ability system
- TargetTypes from ActionRPG, which is useful to find targets for GameplayAbilities with simple logic.
- EffectContainer from ActionRPG.
- Useful AbilityTasks and AsyncTasks
- Useful GameFeatureActions
- Easy-to-use AttributeSet initializer

# Setup
## Project Settings
- Set GASXInputComponent, GASXWorldSettings, GASXAssetManager and GASXGameFeaturePolicy
- Add following types to Primary Asset Types to Scan: Map, GASXExperienceDefinition, GASXUserFacingExperienceDefinition, GASXExperienceActionSet

## Config
- set GASXGameplayCueManager in DefaultGame.ini

```
[/Script/GameplayAbilities.AbilitySystemGlobals]
GlobalGameplayCueManagerClass=/Script/GameplayAbilitySystemExtension.GASXGameplayCueManager
```

## Core Gameplay Actors/Objects
- Use AGASXPlayerController, GASXGameMode and AGASXPlayerState (or subclasses).
- Subclass AGASXBaseCharacter for your character
- Your GameplayAbilities must inherit from UGASXGameplayAbility

# About Experience
Unlike Lyra, You have to make GASXExperienceDefinitions as DATA ASSETs, not pure blueprint subclasses.

## Experience Precedence order (highest wins)
- URL Options override: Used if you use GASXUserFacingExperienceDefinition to travel.
- Command Line override
- World Settings
- Default experience from Project Settings (Gameplay Ability System Extension section)
