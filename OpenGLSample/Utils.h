#pragma once
#include "Face.h"

class Utils {
public:
	static bool isEqualTo(Vec3 v1, Vec3 v2) {
		if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) {
			return true;
		}
		else {
			return false;
		}
	}
};