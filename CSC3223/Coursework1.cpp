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


void StarField(Renderer& renderer)
{
	for (int i = 0; i < 5000; i++)
	{
		Vector3 positions(0, 0, 0);
		positions.x = (float)(rand() - rand());
		positions.y = (float)(rand() - rand());
		positions.z = (float)(rand() - rand());
		OGLMesh* pointMesh = new OGLMesh();
		pointMesh->SetVertexPositions({ positions });
		pointMesh->SetPrimitiveType(GeometryPrimitive::Points);
		pointMesh->SetVertexColours({ Vector4(1 ,1 ,1 ,1) });
		RenderObject* star = new RenderObject(pointMesh,Matrix4::Translation(positions));
		star->GetMesh()->UploadToGPU();
		renderer.AddRenderObject(star);
	}
}

void Planet(float rad, int subDV, int subDH, Renderer& renderer)
{
	std::vector<Vector3> coords;
	std::vector<Vector4> colours;
	std::vector<Vector2> texCoords;
	float DV = 2 * PI / subDV;
	float DH = PI / subDH;
	OGLMesh* planet = new OGLMesh();
	planet->SetPrimitiveType(GeometryPrimitive::TriangleStrip);
	for (int i=0;i<=subDV;i++)
		for (int j = 0; j <= subDH; j++)
		{
			Vector3 temp;
			temp.x= sin(PI - (DH*j))*cos(DV*i)*rad;
			temp.y= sin(PI - (DH*j))*sin(DV*i)*rad;
			temp.z= cos(PI - (DH*j))*rad;
			coords.push_back(temp);
			texCoords.push_back(Vector2(temp.x, temp.y));
		//	colours.push_back(Vector4(0.1, 0, 1, 1));
			colours.push_back(Vector4(1, 1, 1, 1));
			temp.x = sin(PI - (DH*j))*cos(DV*(i+1))*rad;
			temp.y = sin(PI - (DH*j))*sin(DV*(i+1))*rad;
			temp.z = cos(PI - (DH*j))*rad;
			coords.push_back(temp);
			texCoords.push_back(Vector2(temp.x, temp.y));
			//colours.push_back(Vector4(0.1, 0, 1, 1));
			colours.push_back(Vector4(1, 1, 1, 1));
		}
	planet->SetVertexPositions(coords);
	planet->SetVertexColours(colours);
	planet->SetVertexTextureCoords(texCoords);
	planet->UploadToGPU();
	RenderObject* obj = new RenderObject((planet), Matrix4::Translation(Vector3(0, 0, -10)));
	TextureBase* newTex = OGLTexture::RGBATextureFromFilename("Ariel.JPG");
	obj->SetBaseTexture(newTex);
	renderer.AddRenderObject(obj);
}

void cameraUpdate(float& yaw, float& pitch, Vector3& position)
{
	Vector2 temp;
		temp = Window::GetMouse()->GetRelativePosition();
		yaw -= temp.x;
		pitch -= temp.y;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y -= 1;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y += 1;
		}
}

int main() {
	Window*w = Window::CreateGameWindow("CSC3223 Coursework 1!");
	if (!w->HasInitialised()) {
		return -1;
	}

	w->LockMouseToWindow(true);
	Renderer*	renderer = new Renderer(*w);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	StarField(*renderer);
	Planet(1, 50, 50, *renderer);
	float aspect = w->GetScreenAspect();
	Matrix4 proj = Matrix4::Perspective(1.0f, 1000000.0f, aspect, 45.0f);

	renderer->SetProjectionMatrix(proj);
	Vector3 viewPosition(0, 0, 0);
	float yaw=0, pitch=0;

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float time = w->GetTimer()->GetTimeDelta();

		renderer->Update(time);

		renderer->DrawString("OpenGL Rendering!", Vector2(10, 10));
		renderer->Render();

		cameraUpdate(yaw, pitch, viewPosition);
		renderer->SetViewMatrix(Matrix4::Rotation(-pitch, Vector3(1,0,0))*Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *Matrix4::Translation(-viewPosition));

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