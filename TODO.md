- [x] GL context up, screen clears to a solid color
- [ ] Do research on, and understand the GL context setup
- [ ] One hardcoded quad, one hardcoded shader, one draw call
- [ ] Convert to instancing
- [ ] Add the circle-discard branch
- [ ] Orthographic projection
- [ ] Renderer2D.h API cleanup

---

Sound reasoning on the SDF choice — you're picking the technique because it's the one that pays off again later, not because it's fancier for its own sake. Just keep the scope honest: right now that's _one_ per-instance shape flag (rect vs. circle-discard), not a shape library. You'll grow it when a second shape actually earns its way in.

glad is the right call too — nobody hand-rolls loaders in shipping engines, and "first third-party dependency, chosen deliberately" is a much better story than "accumulated by default."

## How to actually get started: build a walking skeleton, not the API

This answers your original question. Don't start by writing `Renderer2D.h`'s full function list and then implementing it top-down — you don't yet know what that API needs to look like, and GL fails silently (black screen, no exception) so debugging a big-bang implementation is miserable. Instead, de-risk in layers, each one independently provable before you add the next:

1. **GL context up, screen clears to a solid color.** No shaders, no geometry. Proves WGL context creation, glad function loading, and swap-buffer/present all work. This alone is a real milestone — don't skip past it quickly.
2. **One hardcoded quad, one hardcoded shader, one draw call.** Proves your VAO/VBO/shader-compile pipeline end to end, with zero abstraction — literal magic numbers. This is the "red triangle" step every graphics programmer does first for a reason: it isolates pipeline bugs from batching bugs.
3. **Convert to instancing.** One shared unit-quad mesh + a per-instance buffer (position, size, color, shape flag) you refill and upload once per frame, drawn with a single `glDrawArraysInstanced`/`glDrawElementsInstanced` call. This is where your batching design actually lives.
4. **Add the circle-discard branch** in the fragment shader, driven by the per-instance shape flag.
5. **Orthographic projection** so instance positions map to sane game-space coordinates instead of clip space.
6. **Only now** wrap it in `Renderer2D.h`'s public surface and call it from your fixed-timestep loop in `Main.cpp`.

Each step should visibly work before you touch the next. If step 3 breaks, you know it's not a shader problem because step 2 already proved the shader — that isolation is the entire point.

## The WGL landmine you're about to hit at step 1

Worth understanding _why_ this is annoying, since it trips up everyone and the reason is historical: `wglCreateContext` (the function that's always available) only ever gives you a **legacy/compatibility GL context** — on Windows that can mean as old as GL 1.1. The function that lets you request a modern **core profile** context, `wglCreateContextAttribsARB`, is itself only reachable through `wglGetProcAddress` — and `wglGetProcAddress` only resolves extension functions _while a GL context is already current_. Chicken, egg.

The canonical workaround (every Win32 GL init routine does this, glfw included): create a throwaway legacy context first, make it current just long enough to resolve `wglCreateContextAttribsARB` (and set a pixel format), then destroy that dummy context and create your real core-profile context through the ARB function. It's ugly, it's a relic of GL's extension mechanism predating any of this being designed in as first-class, and there's no way around it on Windows short of a library hiding it from you. Good news: you only ever write this once.

Go build step 1 — context creation, glad wired into CMake as your first dependency, screen clearing to a color. That's pure mechanical plumbing with a very legible pass/fail signal (you either see a colored window or you don't), so it's yours to fight through solo. Come back when it's either working or you've been stuck on it for a while — genuinely stuck, not "tried once."
