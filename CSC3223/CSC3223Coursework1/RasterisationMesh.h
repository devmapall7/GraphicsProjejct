#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
#include "RenderObject.h"
namespace NCL
{
	namespace CSC3223
	{
		class RasterisationMesh : public OGLMesh
		{
		public:
			static OGLMesh* CreateTriangleFromPoints(const std::vector<Vector3>& vertices, const std::vector<Vector4>& inColours, int type = false);
			static OGLMesh* CreateLineFromPoints(const std::vector<Vector3>& vertices, const std::vector<Vector4>& inColours, bool bresenham=false);
			RasterisationMesh();
			~RasterisationMesh();
		protected:
			void RasterLineEquationTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& c0, const Vector4& c1, const Vector4& c2);
			void RasterSpanTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& c0, const Vector4& c1, const Vector4& c2);
			void RasterBasicLine(const Vector3& p0, const Vector3& p1, const Vector4& c0, const Vector4& c1);
			void RasterBresenhamLine(const Vector3& p0, const Vector3&p1, const Vector4& c0, const Vector4& c1);
			void RasterBarycentricTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& c0, const Vector4& c1, const Vector4& c2);
		};
	}
}
