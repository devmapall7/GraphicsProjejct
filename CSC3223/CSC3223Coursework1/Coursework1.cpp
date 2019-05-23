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
#include "RasterisationMesh.h"

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

void Planet(float rad, int subDV, int subDH, Renderer& renderer, RenderObject* planet, string texture)
{
	std::vector<Vector3> coords;
	std::vector<Vector4> colours;
	std::vector<Vector2> texCoords;
	float DV = 2 * PI / subDV;
	float DH = PI / subDH;
	planet->GetMesh()->SetPrimitiveType(GeometryPrimitive::TriangleStrip);
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
	planet->GetMesh()->SetVertexPositions(coords);
	planet->GetMesh()->SetVertexColours(colours);
	planet->GetMesh()->SetVertexTextureCoords(texCoords);
	planet->GetMesh()->UploadToGPU();
	TextureBase* newTex = OGLTexture::RGBATextureFromFilename(texture);
	planet->SetBaseTexture(newTex);
	renderer.AddRenderObject(planet);
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
	bool depthMode=false, raster=false, blendMode=false;
	int blend = 0;
	w->LockMouseToWindow(true);
	Renderer*	renderer = new Renderer(*w);
	Matrix4 resetView = renderer->GetViewMatrix();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	StarField(*renderer);
	OGLMesh* planet = new OGLMesh();
	OGLMesh* planet1 = new OGLMesh();


	RenderObject* PlanetObj = new RenderObject((planet), Matrix4::Translation(Vector3(0, 0, -10)));
	Planet(1, 50, 50, *renderer,PlanetObj,"Ariel.JPG");

	OGLMesh* moon = new OGLMesh();
	RenderObject* MoonObj = new RenderObject((moon), Matrix4::Translation(Vector3(2, 0, -10)));
	Planet(0.3, 50, 50, *renderer, MoonObj, "Sedna.PNG");

	OGLMesh* ship1 = new OGLMesh();
	ship1->SetPrimitiveType(GeometryPrimitive::Triangles);
	ship1->SetVertexPositions({ Vector3(0,0,0),Vector3(0,-0.01,-2),Vector3(0,1,-2), Vector3(-1.5,0,-2),Vector3(1.5,0,-2),Vector3(0,0,-6),Vector3(0,0,-6), Vector3(1.5,0,-2),Vector3(0,1,-2),Vector3(0,0,-6), Vector3(0,1,-2),Vector3(-1.5,0,-2),Vector3(0,1,-2),Vector3(-1.5,0,-2),Vector3(1.5,0,-2) });
	ship1->SetVertexColours({ Vector4(1, 0, 0, 1) ,Vector4(1, 0, 0, 1) ,Vector4(1, 1, 1, 1) ,Vector4(0.2, 0, 0.1, 1) ,Vector4(0.4, 0, 0.1, 1) ,Vector4(0.1, 0, 0.1, 1),Vector4(1, 1, 1, 0.33) ,Vector4(0, 0, 1, 0.33) ,Vector4(0, 0, 1, 0.33),Vector4(1, 1, 1, 0.33) ,Vector4(0, 0, 1, 0.33) ,Vector4(0, 0, 1, 0.33),Vector4(0.1, 0.4, 0.7, 1) ,Vector4(0.2, 0.9, 0.1, 1) ,Vector4(0.5, 0.5, 0.5, 1) });
	ship1->UploadToGPU();

	RenderObject* ShipObj1 = new RenderObject((ship1), Matrix4::Translation(Vector3(-2, 0, -10))*Matrix4::Scale(Vector3(0.1,0.1,0.1)));
	renderer->AddRenderObject(ShipObj1);

	OGLMesh* triangle1 = new OGLMesh();
	triangle1->SetVertexPositions({ Vector3(-1.5,0,-2),Vector3(1.5,0,-2),Vector3(0,0,-6) });
	triangle1->SetVertexColours({ Vector4(1, 1, 1, 1),Vector4(1, 1, 1, 1),Vector4(1, 1, 1, 1) });
	triangle1->UploadToGPU();
	renderer->AddRenderObject(new RenderObject((triangle1), Matrix4::Translation(Vector3(-5, 0, -10))));

	OGLMesh* triangle2 = new OGLMesh();
	triangle2->SetVertexPositions({ Vector3(-1.5,0,-2),Vector3(1.5,0,-2),Vector3(0,0,-6) });
	triangle2->SetVertexColours({ Vector4(1, 0, 0, 1),Vector4(1, 0, 0, 1),Vector4(1, 0, 0, 1) });
	triangle2->UploadToGPU();
	renderer->AddRenderObject(new RenderObject((triangle2), Matrix4::Translation(Vector3(-5, 0, -10))));


	float aspect = w->GetScreenAspect();
	Matrix4 proj = Matrix4::Perspective(1.0f, 1000000.0f, aspect, 45.0f);
	Matrix4 reset = renderer->GetProjectionMatrix();
	renderer->SetProjectionMatrix(proj);
	Vector3 viewPosition(0, 0, 0);
	float yaw=0, pitch=0;

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float time = w->GetTimer()->GetTimeDelta();

		renderer->Update(time);

		renderer->DrawString("OpenGL Rendering!", Vector2(10, 10));
		renderer->Render();
		if(!raster)
		{
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
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F1))
		{
			renderer->EnableDepthBuffer(!depthMode);
			depthMode = !depthMode;
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F2))
		{
			if (depthMode) glDepthFunc(GL_LESS);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F3))
		{
			if (depthMode) glDepthFunc(GL_LEQUAL);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F4))
		{
			if (depthMode) glDepthFunc(GL_EQUAL);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F5))
		{
			renderer->EnableBlending(!blendMode);
			blendMode=!blendMode;
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F6))
		{
			if (blendMode)
			{
				renderer->SetBlendToLinear();
				blend = 0;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F7))
		{
			if (blendMode)
			{
				renderer->SetBlendToAdditive();
				blend = 1;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F8))
		{
			if (blendMode)
			{
				renderer->SetBlendToInvert();
				blend = 2;
			}
		}
		if (blendMode)
		{
			renderer->EnableBlending(blendMode);
			if (blend==0) renderer->SetBlendToLinear();
			else if (blend==1) renderer->SetBlendToAdditive();
			else renderer->SetBlendToInvert();
		}
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F9))
		{
			raster = true;
			renderer->DeleteAllRenderObjects();
			renderer->SetProjectionMatrix(reset);
			renderer->SetViewMatrix(resetView);
			OGLMesh * rasterLine = (OGLMesh *)RasterisationMesh::CreateLineFromPoints({ Vector3(40,40,0), Vector3(40,200,0),Vector3(40,200,0), Vector3(130,200,0),Vector3(200,200,0), Vector3(200,80,0),Vector3(200,80,0), Vector3(250,40,0),Vector3(200,80,0), Vector3(150,40,0), Vector3(400,40,0),Vector3(400,200,0), Vector3(400,200,0),Vector3(475,200,0),Vector3(475,200,0),Vector3(475,40,0),Vector3(475,40,0),Vector3(400,40,0) }, { Vector4(1 , 0 , 0 , 1) , Vector4(0 , 1 , 0 , 1), Vector4(0 , 1 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(1, 0 , 0 , 1),Vector4(0 , 1 , 0 , 1),Vector4(0 , 1 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(0 , 1 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(0 , 1 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(0 , 1 , 0 , 1),Vector4(0 , 1 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(1 , 0 , 0 , 1),Vector4(0 , 1 , 0 , 1) }, false);
			renderer->AddRenderObject(new RenderObject((rasterLine)));
			OGLMesh * rasterTriangle = (OGLMesh *)RasterisationMesh::CreateTriangleFromPoints({ Vector3(300,40,0),Vector3(300,120,0),Vector3(375,80,0),Vector3(300,120,0),Vector3(300,200,0),Vector3(375,160,0) }, { Vector4(1 , 0 , 0 , 1) , Vector4(0 , 1 , 0 , 1), Vector4(0 , 0 , 1 , 1),Vector4(0 , 1 , 0 , 1),Vector4(0 , 0 , 1 , 1),Vector4(1 , 0 , 0 , 1) });
			renderer->AddRenderObject(new RenderObject((rasterTriangle)));
		}
		PlanetObj->SetTransform(PlanetObj->GetTransform() * Matrix4::Rotation(1.0f,Vector3(-1.0, -2, 0)));
		MoonObj->SetTransform(MoonObj->GetTransform()* Matrix4::Rotation(1.0f, Vector3(1.0, 2, 0)));
		ShipObj1->SetTransform(ShipObj1->GetTransform() * Matrix4::Translation(Vector3(0, 0, -0.1)));
		w->SetTitle(std::to_string(time));
	}

	delete renderer;

	Window::DestroyGameWindow();
}