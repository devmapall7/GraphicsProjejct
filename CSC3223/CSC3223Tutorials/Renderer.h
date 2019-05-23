#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Common/TextureWriter.h"
#include "RenderObject.h"
#include "../../Common/Vector2.h"
#include "../../Common/Maths.h"

namespace NCL {
	namespace CSC3223 {

		struct Light
		{
			Vector3 position;
			float radius;
			Vector3 colour;
		};

		class Renderer : public OGLRenderer
		{
		public:
			Renderer(Window& w);
			~Renderer();

			void AddRenderObject(RenderObject* ro) {
				renderObjects.emplace_back(ro);
			}

			void DeleteAllRenderObjects() {
				for (const RenderObject* object : renderObjects) {
					delete object;
				}
				renderObjects.clear();
			}
			
			void SetProjectionMatrix(const Matrix4&m) {
				projMatrix = m;
			}

			void SetViewMatrix(const Matrix4&m) {
				viewMatrix = m;
			}
			int getWidth()
			{
				return currentWidth;
			}
			int getHeight()
			{
				return currentHeight;
			}
			void EnableDepthBuffer(bool state)
			{
				if (state)
				{
					glEnable(GL_DEPTH_TEST);
				}
				else
				{
					glDisable(GL_DEPTH_TEST);
				}
			}
			void WriteDepthBuffer(const string&filepath) const
			{
				float* data = new float[currentWidth*currentHeight];
				glReadPixels(0, 0, currentWidth, currentHeight, GL_DEPTH_COMPONENT, GL_FLOAT, data);
				
				char*pixels = new char[currentWidth*currentHeight * 3];
				char*pixelPointer = pixels;

				for (int y = 0;y < currentHeight;++y)
				{
					for (int x = 0;x < currentWidth;++x)
					{
						float depthValue = data[(y*currentWidth) + x];

						float mult = 1.0f / 0.333f;
						float redAmount = Maths::Clamp(depthValue, 0.0f, 0.333f)*mult;
						float greenAmount = (Maths::Clamp(depthValue, 0.333f, 0.666f)-0.333f)*mult;
						float blueAmount = (Maths::Clamp(depthValue, 0.666f, 1.0f)-0.666f)*mult;

						unsigned char redByte = (char)(redAmount * 255);
						unsigned char greenByte = (char)(greenAmount * 255);
						unsigned char blueByte = (char)(blueAmount * 255);

						*pixelPointer++ = (char)(redAmount * 255);
						*pixelPointer++ = (char)(greenAmount * 255);
						*pixelPointer++ = (char)(blueAmount * 255);
					}
				}
				TextureWriter::WritePNG(filepath, pixels, currentWidth, currentHeight, 3);
				delete pixels;
				delete data;
			}
			void SetLightProperties(Vector3 pos, Vector3 colour, float radius);
		protected:
			GameTimer frameTimer;
			void RenderNode(RenderObject* root);
	
			void OnWindowResize(int w, int h)	override;

			void RenderFrame()	override;
			OGLShader*		defaultShader;

			Matrix4		projMatrix;
			Matrix4		viewMatrix;

			vector<RenderObject*> renderObjects;

			void ApplyLightToShader(const Light&l, const OGLShader*s);

			Light activeLight;
		};
	}
}

