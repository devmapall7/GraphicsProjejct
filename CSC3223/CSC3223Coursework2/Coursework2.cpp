#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"
#include "../../Common/Maths.h"

#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"

#include "Renderer.h"

using namespace NCL;
using namespace CSC3223;

RenderObject* Coursework2(Renderer &renderer) {
	OGLMesh* m = new OGLMesh("cube.msh");
	m->SetPrimitiveType(GeometryPrimitive::Triangles);
	m->UploadToGPU();

	RenderObject* object = new RenderObject(m);
	renderer.AddRenderObject(object);

	return object;
}
int main() {
	Window*w = Window::CreateGameWindow("CSC3223 Coursework 2!");

	if (!w->HasInitialised()) {
		return -1;
	}

	Renderer*	renderer = new Renderer(*w);

	RenderObject* cube = Coursework2(*renderer);

	TextureBase* tex = OGLTexture::RGBATextureFromFilename("checkerboard.png");
	cube->SetBaseTexture(tex);

	renderer->SetProjectionMatrix(Matrix4::Perspective(1, 1000, w->GetScreenAspect(), 45.0f));
	Vector3 viewPosition(0, 0, 0);
	float rotation = 0.0f;
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer->EnableDepthBuffer(true);
		renderer->EnableBlending(true);
		float time = w->GetTimer()->GetTimeDelta();
		rotation += time * 0.1f;
		renderer->Update(time);

		Matrix4 modelMat = Matrix4::Translation(Vector3(0, 0, -10));

		modelMat = modelMat * Matrix4::Rotation(rotation, Vector3(1, 1, 1));

		cube->SetTransform(modelMat);
		// reset cube
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_SPACE)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Triangles);
			OGLShader* newShader = new OGLShader("RasterisationVert.glsl", "RasterisationFrag.glsl");
			cube->SetShader(newShader);
		}
		// shrinking shader
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F1)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Triangles);
			GameTimer a;
			renderer->setGameTimer(a);
			OGLShader* newShader = new OGLShader("ShrinkVertex.glsl", "RasterisationFrag.glsl");
			cube->SetShader(newShader);
		}
		// fading shader
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F2)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Triangles);
			GameTimer a;
			renderer->setGameTimer(a);
			OGLShader* newShader = new OGLShader("RasterisationVert.glsl", "FadeFragment.glsl");
			cube->SetShader(newShader);
		}
		// clean to destroyed texture
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F3)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Triangles);
			GameTimer a;
			renderer->setGameTimer(a);
			OGLShader* newShader = new OGLShader("RasterisationVert.glsl", "DestroyedFragment.glsl");
			cube->SetSecondTexture(OGLTexture::RGBATextureFromFilename("perlin_noise.png"));
			cube->SetShader(newShader);
		}
		// split into smaller cubes
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F4)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Triangles);
			GameTimer a;
			renderer->setGameTimer(a);
			OGLShader* newShader = new OGLShader("RasterisationVert.glsl", "RasterisationFrag.glsl", "FallGeom.glsl");
			cube->SetShader(newShader);
		}
		// bubble gum stretch
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F5)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			GameTimer a;
			renderer->setGameTimer(a);
			OGLShader* newShader = new OGLShader("tessVert.glsl", "tessFrag.glsl","", "splinterTCS.glsl", "splinterTES.glsl");
			cube->SetSecondTexture(OGLTexture::RGBATextureFromFilename("perlin_noise.png"));
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Patches);
			cube->SetShader(newShader);
		}
		// burns out the texture because reflection gets too high
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F6)) {
			rotation = 0.0f;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			cube->GetMesh()->SetPrimitiveType(GeometryPrimitive::Triangles);
			GameTimer a;
			renderer->setGameTimer(a);
			OGLShader* newShader = new OGLShader("LightingVertex.glsl", "LightingFragment.glsl");
			cube->SetShader(newShader);
			renderer->SetLightProperties(Vector3(0, 0, 0), Vector3(1.0f, 0.3f, 0.2f), 10.0f);
		}
		renderer->DrawString("OpenGL Rendering!", Vector2(10, 10));
		renderer->SetViewMatrix(Matrix4::Translation(viewPosition));

		renderer->Render();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) { viewPosition.x += 0.1f; }

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) { viewPosition.x -= 0.1f; }

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) { viewPosition.y -= 0.1f; }

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_V)) { viewPosition.y += 0.1f; }

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) { viewPosition.z += 0.1f; }

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) { viewPosition.z -= 0.1f; }

		renderer->SetViewMatrix(Matrix4::Translation(viewPosition));


		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_HOME)) {
			w->SetFullScreen(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_END)) {
			w->SetFullScreen(false);
		}

		w->SetTitle(std::to_string(time));
	}

	delete renderer;

	Window::DestroyGameWindow();
}