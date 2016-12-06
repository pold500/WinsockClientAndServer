#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "tiny_obj_helpers.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

static bool TestLoadObj(const char* filename, const char* basepath = NULL,
	bool triangulate = true) {
	std::cout << "Loading " << filename << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;


	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename,
		basepath, triangulate);

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return false;
	}

	PrintInfo(attrib, shapes, materials);

	return true;
}

//static bool TestStreamLoadObj() {
//	std::cout << "Stream Loading " << std::endl;
//
//	std::stringstream objStream;
//	objStream << "mtllib cube.mtl\n"
//		"\n"
//		"v 0.000000 2.000000 2.000000\n"
//		"v 0.000000 0.000000 2.000000\n"
//		"v 2.000000 0.000000 2.000000\n"
//		"v 2.000000 2.000000 2.000000\n"
//		"v 0.000000 2.000000 0.000000\n"
//		"v 0.000000 0.000000 0.000000\n"
//		"v 2.000000 0.000000 0.000000\n"
//		"v 2.000000 2.000000 0.000000\n"
//		"# 8 vertices\n"
//		"\n"
//		"g front cube\n"
//		"usemtl white\n"
//		"f 1 2 3 4\n"
//		"g back cube\n"
//		"# expects white material\n"
//		"f 8 7 6 5\n"
//		"g right cube\n"
//		"usemtl red\n"
//		"f 4 3 7 8\n"
//		"g top cube\n"
//		"usemtl white\n"
//		"f 5 1 4 8\n"
//		"g left cube\n"
//		"usemtl green\n"
//		"f 5 6 2 1\n"
//		"g bottom cube\n"
//		"usemtl white\n"
//		"f 2 6 7 3\n"
//		"# 6 elements";
//
//	std::string matStream(
//		"newmtl white\n"
//		"Ka 0 0 0\n"
//		"Kd 1 1 1\n"
//		"Ks 0 0 0\n"
//		"\n"
//		"newmtl red\n"
//		"Ka 0 0 0\n"
//		"Kd 1 0 0\n"
//		"Ks 0 0 0\n"
//		"\n"
//		"newmtl green\n"
//		"Ka 0 0 0\n"
//		"Kd 0 1 0\n"
//		"Ks 0 0 0\n"
//		"\n"
//		"newmtl blue\n"
//		"Ka 0 0 0\n"
//		"Kd 0 0 1\n"
//		"Ks 0 0 0\n"
//		"\n"
//		"newmtl light\n"
//		"Ka 20 20 20\n"
//		"Kd 1 1 1\n"
//		"Ks 0 0 0");
//
//	using namespace tinyobj;
//	class MaterialStringStreamReader : public MaterialReader {
//	public:
//		MaterialStringStreamReader(const std::string& matSStream)
//			: m_matSStream(matSStream) {}
//		virtual ~MaterialStringStreamReader() {}
//		virtual bool operator()(const std::string& matId,
//			std::vector<material_t>* materials,
//			std::map<std::string, int>* matMap,
//			std::string* err) {
//			(void)matId;
//			(void)err;
//			LoadMtl(matMap, materials, &m_matSStream);
//			return true;
//		}
//
//	private:
//		std::stringstream m_matSStream;
//	};
//
//	MaterialStringStreamReader matSSReader(matStream);
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string err;
//	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &objStream,
//		&matSSReader);
//
//	if (!err.empty()) {
//		std::cerr << err << std::endl;
//	}
//
//	if (!ret) {
//		return false;
//	}
//
//	//PrintInfo(attrib, shapes, materials);
//
//	return true;
//}

#ifdef STANDALONE_APP
int main(int argc, char** argv) {

	assert(true == TestStreamLoadObj());
	assert(true == TestLoadObj("models/catmark_torus_creases0.obj", "models/", false));

	return 0;
}
#endif
