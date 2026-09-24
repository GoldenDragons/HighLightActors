# HighLightActors

English | [简体中文](README.zh-CN.md)

HighLightActors is a runtime Unreal Engine plugin that draws configurable actor outlines using Custom Depth/Stencil and a post-process material. It provides both C++ and Blueprint APIs.

## Features

- Highlight one actor, an actor array, or actors found by one or more tags.
- Optionally include attached child actors and Child Actor Components.
- Set the outline color and width on each highlight call.
- Default to Unreal Editor's selection orange and a one-pixel outline.
- Restore each component's previous Custom Depth and Stencil state when highlights are cleared.

## Version and Requirements

- Plugin version: **1.0.0**
- Supported Unreal Engine versions: **5.0–5.8**
- Module: **HighLightActors** (Runtime)

## Install in an Unreal Project

### Install from the ZIP package

1. Download and extract [HighLightActors-1.0.0.zip](HighLightActors-1.0.0.zip).
2. Place the extracted HighLightActors folder in your project at Plugins/HighLightActors/.
3. Create the Plugins folder if the project does not have one yet.
4. In **Project Settings → Rendering → Postprocessing**, set **Custom Depth-Stencil Pass** to **Enabled with Stencil**.
5. Add the following lines to the RendererSettings section of Config/DefaultEngine.ini:

~~~ini
r.CustomDepth=3
r.CustomDepthTemporalAAJitter=0
~~~

The second setting disables temporal jitter in Custom Depth so the post-process outline stays stable with TAA/TSR.

6. Enable the plugin, regenerate IDE project files, and rebuild C++ projects.

You can also copy this repository's plugin directory into Plugins/HighLightActors/ in the target project.

## C++ Usage

Add the plugin module to your game module's Build.cs dependencies:

~~~csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core",
    "CoreUObject",
    "Engine",
    "HighLightActors"
});
~~~

<img width="1284" height="449" alt="HighLightActors C++ module dependency" src="https://github.com/user-attachments/assets/fe1c5380-1a43-4dcd-89f4-67a010026fe5" />

Include the function library and call it from your actor or game code:

~~~cpp
#include "HighLightActorsLibrary.h"

UHighLightActorsLibrary::HighlightActor(
    GetWorld(),
    TargetActor,
    true,                                    // Include child actors
    FLinearColor(1.0f, 0.15f, 0.15f, 1.0f), // Outline color
    2.0f                                     // Width in screen pixels
);

UHighLightActorsLibrary::ClearHighlights(GetWorld());
~~~

Other C++ functions include HighlightActors, HighlightActorsWithTag, HighlightActorsWithTags, ClearHighlights, SetMaxPrimitivesToHighlight, and GetMaxPrimitivesToHighlight.

## Blueprint Usage

Search for **Highlight Actor Outline** in a Blueprint graph and connect the target actor. The World Context pin is usually hidden automatically. Array and tag variants are in the HighLightActors|Highlight category.

<img width="1448" height="672" alt="Single actor outline Blueprint node and parameters" src="https://github.com/user-attachments/assets/e07cf93c-0c46-4348-9b64-d66c1cd6ce76" />

### Blueprint Nodes

| Node | Description |
| --- | --- |
| Highlight Actor Outline | Highlight one actor |
| Highlight Actor Outlines | Highlight an actor array |
| Highlight Actor Outlines By Tag | Find and highlight actors with one tag |
| Highlight Actor Outlines By Tags | Find and highlight actors with multiple tags |
| Clear Actor Outlines | Clear outlines and restore captured Stencil state |
| Set Max Outline Primitives | Set the per-call Primitive limit; 0 means unlimited |
| Get Max Outline Primitives | Read the current Primitive limit |

### Highlight Parameters

| Parameter | Type | Default | Description |
| --- | --- | --- | --- |
| Actor / Actors / Tag / Tags | Actor / Array / Name | — | Selects the actors to outline or the tag search criteria |
| Include Children | Boolean | true | Also process attached child actors and Child Actor Components |
| Outline Color | Linear Color | (0.828, 0.364, 0.003, 1) | Color for this call; the default matches Unreal Engine 5.8's default editor selection color |
| Outline Width | Float | 1.0 | Width in screen pixels; runtime values are clamped to 0–16 |

<img width="685" height="412" alt="Blueprint actor-array and tag nodes" src="https://github.com/user-attachments/assets/a1caebf8-c545-4613-8371-081ffbae7353" />

## Screenshots

<img width="1914" height="485" alt="Project plugin settings" src="https://github.com/user-attachments/assets/d90e3856-7530-48bd-9aa7-920a69132748" />
<img width="1920" height="1032" alt="Runtime actor outline example" src="https://github.com/user-attachments/assets/542c3ce4-8045-4d90-a6a0-9efd005afa4c" />
<img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/a7e63c71-85c6-4338-993f-69eee2614456" />


## Package

The 1.0.0 ZIP contains the source and a Win64 package. For other target platforms, place the source plugin in the project and rebuild it for that platform.
