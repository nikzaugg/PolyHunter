#ifndef B_START_SCREEN_RENDERER_H
#define B_START_SCREEN_RENDERER_H

#include "bRenderer.h"

class StartScreenRenderer
{
	StartScreenRenderer();
	StartScreenRenderer(Renderer & renderer);

	void bindBlurFbo();

private:
	Renderer _renderer;
};
typedef std::shared_ptr< StartScreenRenderer >  StartScreenRendererPtr;
#endif /* defined(B_START_SCREEN_RENDERER_H) */