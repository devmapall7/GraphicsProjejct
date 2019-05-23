#include "RasterisationMesh.h"
#include<algorithm>
#include"..\..\Common\Maths.h"
#include"..\..\Common\Vector2.h"
using namespace std;
namespace NCL
{
	namespace CSC3223
	{
		OGLMesh* RasterisationMesh::CreateLineFromPoints(const std::vector<Vector3>& inVerts, const std::vector<Vector4>& inColours, bool bresenham)
		{
			RasterisationMesh* line = new RasterisationMesh();
			if (bresenham)
			{
				for (int i = 0; i < inVerts.size() - 1;i += 2)
				{
					line->RasterBresenhamLine(inVerts[i], inVerts[i + 1], inColours[i], inColours[i + 1]);
				}
			}
			else
			{
				for (int i = 0; i < inVerts.size() - 1;i += 2)
				{
					line->RasterBasicLine(inVerts[i], inVerts[i + 1], inColours[i], inColours[i + 1]);
				}
			}
			line->SetPrimitiveType(GeometryPrimitive::Points);
			line->UploadToGPU();
			return line;
		}
		void RasterisationMesh::RasterBasicLine(const Vector3& v0, const Vector3& v1, const Vector4& c0, const Vector4& c1)
		{
			float lineLength = (v0 - v1).Length();
			int numPixels = (int)lineLength;
			float lerpAmount = 1.0f / (numPixels - 1);
			float t = 0.0f;
			for (int i = 0; i < numPixels;++i)
			{
				positions.emplace_back(Maths::Lerp<Vector3>(v0, v1, t));
				colours.emplace_back(Maths::Lerp<Vector4>(c0, c1, t));
				t += lerpAmount;
			}
		}
		void RasterisationMesh::RasterBresenhamLine(const Vector3& v0, const Vector3& v1, const Vector4& c0, const Vector4& c1)
		{

			Vector3 dir = v1 - v0;
			Vector3 currentPos = v0;
			int scanRange = 0;
			float slope = 0.0f;
			float* periodAxis = nullptr;
			float* scanAxis = nullptr;
			int scanDelta = 0;
			int targetDelta = 0;
			if (abs(dir.y) > abs(dir.x))
			{
				slope = (dir.x / dir.y);
				scanRange = (int)abs(dir.y);
				periodAxis = &currentPos.x;
				scanAxis = &currentPos.y;
				scanDelta = (dir.y < 0.0f) ? -1 : 1;
				targetDelta = (dir.x < 0.0f) ? -1 : 1;
			}
			float absSlope = abs(slope);
			float error = 0.0f;
			Vector4 colourDiff = c1 - c0;
			Vector4 colourDelta = colourDiff / scanRange;
			Vector4 currentColour = c0;
			for (int i = 0;i < scanRange;++i)
			{
				positions.emplace_back(currentPos);
				colours.emplace_back(currentColour);
				currentColour += colourDelta;
				error += absSlope;
				if (error > 0.5f)
				{
					error -= 1.0f;
					(*periodAxis) += targetDelta;
				}
				(*scanAxis) += scanDelta;
			}
		}
		OGLMesh* RasterisationMesh::CreateTriangleFromPoints(const std::vector<Vector3>& inVerts, const std::vector<Vector4>& inColours, int type)
		{
			RasterisationMesh* tri = new RasterisationMesh();
			if (type == 1)
			{
				for (size_t i = 0; i < inVerts.size() - 2;i += 3)
				{
					tri->RasterBarycentricTriangle(inVerts[i], inVerts[i + 1], inVerts[i + 2], inColours[i], inColours[i+1], inColours[i+2]);
				}
			}
			else if (type == 2)
			{
				for (size_t i = 0; i < inVerts.size() - 2;i += 3)
				{
					tri->RasterSpanTriangle(inVerts[i], inVerts[i + 1], inVerts[i + 2], inColours[i], inColours[i + 1], inColours[i + 2]);
				}
			}
			else
			{
				for (size_t i = 0; i < inVerts.size() - 2;i += 3)
				{
					tri->RasterLineEquationTriangle(inVerts[i], inVerts[i + 1], inVerts[i + 2], inColours[i], inColours[i + 1], inColours[i + 2]);
				}
			}
			tri->SetPrimitiveType(GeometryPrimitive::Points);
			tri->UploadToGPU();
			return tri;
		}

		void RasterisationMesh::RasterLineEquationTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& c0, const Vector4& c1, const Vector4& c2)
		{
			Vector2 topLeft, bottomRight;
			Maths::ScreenBoxOfTri(p0, p1, p2, topLeft, bottomRight);
			Vector3 linev0v1 = Vector3::Cross(Vector3(p0.x, p0.y, 1), Vector3(p1.x, p1.y, 1));
			Vector3 linev1v2 = Vector3::Cross(Vector3(p1.x, p1.y, 1), Vector3(p2.x, p2.y, 1));
			Vector3 linev2v0 = Vector3::Cross(Vector3(p2.x, p2.y, 1), Vector3(p0.x, p0.y, 1));
			for (int y = (int)topLeft.y;y < bottomRight.y;++y)
			{
				for (int x = (int)topLeft.x; x < bottomRight.x;++x)
				{
					Vector3 screenPos((float)x, (float)y, 1.0f);
					float line01 = Vector3::Dot(linev0v1, screenPos);
					float line12 = Vector3::Dot(linev1v2, screenPos);
					float line20 = Vector3::Dot(linev2v0, screenPos);
					if (line01 <= 0 && line12 <= 0 && line20 <= 0)
					{
						positions.emplace_back(screenPos);
						float lineSum = line01 + line12 + line20;
						line01 /= lineSum;
						line12 /= lineSum;
						line20 /= lineSum;
						Vector4 colour = (c0*line12) + (c1*line20) + (c2*line01);
						colours.emplace_back(colour);
					}
				}
			}
		}

		void RasterisationMesh::RasterBarycentricTriangle(const Vector3&p0, const Vector3&p1, const Vector3&p2, const Vector4& c0, const Vector4& c1, const Vector4& c2)
		{
			Vector2 topLeft, bottomRight;
			Maths::ScreenBoxOfTri(p0, p1, p2, topLeft, bottomRight);
			float triArea = Maths::ScreenAreaOfTri(p0, p1, p2);
			float areaRecip = 1.0f / triArea;
			int subTriArea[3];

			for (int y = (int)topLeft.y;y < bottomRight.y;++y)
			{
				for (int x = (int)topLeft.x;x < bottomRight.x;++x)
				{
					Vector3 screenPos((float)x, (float)y, 0.0f);
					subTriArea[0] = abs(ScreenAreaOfTri(p0, screenPos, p1));
					subTriArea[1] = abs(ScreenAreaOfTri(p1, screenPos, p2));
					subTriArea[2] = abs(ScreenAreaOfTri(p2, screenPos, p0));
					float triSum = subTriArea[0] + subTriArea[1] + subTriArea[2];
					if ((int)triSum > triArea) continue;
					float alpha = subTriArea[1] * areaRecip;
					float beta = subTriArea[2] * areaRecip;
					float gamma = subTriArea[0] * areaRecip;

					Vector4 colour = ((c0*alpha) + (c1*beta) + (c2*gamma));

					positions.emplace_back(screenPos);
					colours.emplace_back(colour);
				}
			}
		}

		void RasterisationMesh::RasterSpanTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& c0, const Vector4& c1, const Vector4& c2)
		{
			const Vector3* vertices[3] = { &v0,&v1,&v2 };
			if (vertices[1]->y<vertices[0]->y) std::swap(vertices[1], vertices[0]);
			if (vertices[2]->y<vertices[0]->y) std::swap(vertices[2], vertices[0]);
			if (vertices[2]->y<vertices[1]->y) std::swap(vertices[2], vertices[1]);
			Vector3 longDir = *vertices[2] - *vertices[0];
			Vector3 longDelta = longDir / longDir.y;
			Vector3 shortDir = *vertices[1] - *vertices[0];
			Vector3 shortDelta = shortDir / shortDir.y;
			float longX = vertices[0]->x;
			float shortX = vertices[0]->x;
			for (int y = vertices[0]->y;y < vertices[1]->y;++y)
			{
				int startX = std::min(longX, shortX);
				int endX = std::max(longX, shortX);
				for (int x = startX;x < endX;++x)
				{
					positions.emplace_back(Vector3(x, y, 1));
				}
				longX += longDelta.x;
				shortX += shortDelta.x;
			}
			shortDir = *vertices[2] - *vertices[1];
			shortDelta = shortDir / shortDir.y;
			for (int y = vertices[1]->y;y < vertices[2]->y;++y)
			{
				int startX = std::min(longX, shortX);
				int endX = std::max(longX, shortX);

				for (int x = startX;x < endX;++x)
				{
					positions.emplace_back(Vector3(x, y, 1));
				}
				longX += longDelta.x;
				shortX += shortDelta.x;
			}
		}
		RasterisationMesh::RasterisationMesh()
		{
		}


		RasterisationMesh::~RasterisationMesh()
		{
		}
	}
}