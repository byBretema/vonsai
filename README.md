
<p align="center"> <img alt="Vonsai logo" src="./doc/images/Vonsai.png" height="150" width="150"> </p>
<h1 align="center">VONSAI Renderer</h1>

!["Three monkeys screenshot"](./doc/images/screenshot.png)


## Done
- [X] Allow multi-window with independent contexts
- [X] Input system exposed to the user *Unity-Like*
- [X] Lights loaded as UBO
- [X] Efficient UBO updating using glBufferSubData and caching with unordered_map
- [X] Cached uniforms locations.
- [X] Add ASSIMP to support many data filetypes; *specially FBX, DAE, DXF, BVH and glTF*.

## Work In Progress
- [ ] Material abstraction.
- [ ] Define draw workflow and responsibilities between _Renderable_, _Shader_ and _Texture_.

## Future plans
- [ ] _**E**ntitiy_ _**C**omponent_ _**S**ystem_; a.k.a. *Data oriented design*.
- [ ] Indirection layer for gpu render to allow the use of OpenGL, Vulkan, DX11/12 or Metal transparently.

## Needs a revision
- [X] BindGuard for ensure to un/bind a resource based on scope, following [RAII idiom](https://en.cppreference.com/w/cpp/language/raii).
  - Issues when a pointer-copy is passed.
