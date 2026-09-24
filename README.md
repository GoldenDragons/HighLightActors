# HighLightActors

HighLightActors 是一个适用于 Unreal Engine 5.8 的运行时插件（同时适用于UE5.0-5.8之前的所有版本），用 Custom Depth/Stencil 和后处理材质给 Actor 绘制可配置的轮廓。插件支持 C++ 与蓝图调用。

## 功能

- 高亮单个 Actor、Actor 数组、带一个或多个 Tag 的 Actor。
- 可选包含附着的子 Actor 与 Child Actor Component。
- 每次高亮调用可传入颜色和线宽，不需要单独设置颜色节点。
- 默认颜色为 UE 编辑器默认选中色橙色；默认线宽为 1 个屏幕像素。
- 清除高亮时恢复组件原来的 Custom Depth 和 Stencil 状态。

## 版本与要求

- 插件版本：**1.0.0**
- 已验证引擎：Unreal Engine **5.0-5.8**
- 插件源码模块：HighLightActors（Runtime）

## 安装到 UE 项目

### 使用压缩包

1. 从本仓库下载 [HighLightActors-1.0.0.zip](HighLightActors-1.0.0.zip) 并解压。
2. 将解压出的 HighLightActors 文件夹放到项目根目录的 Plugins/HighLightActors/。
3. 如项目尚无 Plugins 文件夹，请先创建。
4. 在项目设置的 **Rendering → Postprocessing** 中启用 **Custom Depth-Stencil Pass**，选择 **Enabled with Stencil**。
5. 在项目 Config/DefaultEngine.ini 的 RendererSettings 段加入：

~~~ini
r.CustomDepth=3
r.CustomDepthTemporalAAJitter=0
~~~

第二项关闭 Custom Depth 的时间抖动，避免后处理轮廓随 TAA/TSR 闪动。

6. 启用插件并重新生成 IDE 工程文件。C++ 项目需要重新编译。

如果直接使用源码，可将整个插件目录复制到目标项目的 Plugins/HighLightActors/。

## C++ 用法

在游戏模块的 Build.cs 中添加插件模块依赖：

~~~csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core",
    "CoreUObject",
    "Engine",
    "HighLightActors"
});
~~~
<img width="1284" height="449" alt="image" src="https://github.com/user-attachments/assets/fe1c5380-1a43-4dcd-89f4-67a010026fe5" />

然后包含函数库并调用：

~~~cpp
#include "HighLightActorsLibrary.h"

UHighLightActorsLibrary::HighlightActor(
    GetWorld(),
    TargetActor,
    true,                                    // 同时高亮子 Actor
    FLinearColor(1.0f, 0.15f, 0.15f, 1.0f), // 轮廓颜色
    2.0f                                     // 线宽（屏幕像素）
);

UHighLightActorsLibrary::ClearHighlights(GetWorld());
~~~

其他 C++ 接口：HighlightActors、HighlightActorsWithTag、HighlightActorsWithTags、ClearHighlights、SetMaxPrimitivesToHighlight 和 GetMaxPrimitivesToHighlight。

## 蓝图用法

在蓝图图表中搜索 **Highlight Actor Outline**，把 Actor 接到目标输入即可。World Context 在普通蓝图中通常自动隐藏。数组和 Tag 版本位于 HighLightActors|Highlight 分类中。

## 单 Actor 高亮蓝图节点与参数面板。
> <img width="1448" height="672" alt="image" src="https://github.com/user-attachments/assets/e07cf93c-0c46-4348-9b64-d66c1cd6ce76" />


### 蓝图节点

| 节点 | 作用 |
| --- | --- |
| Highlight Actor Outline | 高亮一个 Actor |
| Highlight Actor Outlines | 高亮 Actor 数组 |
| Highlight Actor Outlines By Tag | 按单个 Tag 查找并高亮 |
| Highlight Actor Outlines By Tags | 按多个 Tag 查找并高亮 |
| Clear Actor Outlines | 清除当前高亮并恢复组件之前的 Stencil 状态 |
| Set Max Outline Primitives | 设置每次高亮最多处理的 Primitive 数量；0 表示不限制 |
| Get Max Outline Primitives | 读取当前 Primitive 数量上限 |

### 高亮参数

| 参数 | 类型 | 默认值 | 作用 |
| --- | --- | --- | --- |
| Actor / Actors / Tag / Tags | Actor / 数组 / Name | — | 指定要高亮的对象或查找条件 |
| Include Children | Boolean | true | 同时处理附着子 Actor 与 Child Actor Component |
| Outline Color | Linear Color | (0.828, 0.364, 0.003, 1) | 本次调用使用的轮廓颜色，默认对应 UE 5.8 编辑器默认选中色 |
| Outline Width | Float | 1.0 | 轮廓宽度，单位为屏幕像素；运行时限制在 0 到 16 之间 |

### Actor 数组与 Tag 蓝图节点。
> <img width="685" height="412" alt="image" src="https://github.com/user-attachments/assets/a1caebf8-c545-4613-8371-081ffbae7353" />



### 项目插件设置页面。
> <img width="1914" height="485" alt="image" src="https://github.com/user-attachments/assets/d90e3856-7530-48bd-9aa7-920a69132748" />




### 游戏运行时选中 Actor 的轮廓效果。
> <img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/8bfeaa37-a85b-4d82-9cf4-4962963c218e" />
> <img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/542c3ce4-8045-4d90-a6a0-9efd005afa4c" />

## 压缩包内容

发布包为 Windows x64 构建的插件包，源码仍包含在包内。其他平台可将源码插件放入项目并由目标平台重新编译。
