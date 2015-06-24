//
//  ray.h
//  DifViewer
//
//  Created by HiGuy Smith on 1/2/15.
//  Copyright (c) 2015 CouleeApps. All rights reserved.
//

#ifndef ray_h
#define ray_h

#include "point3.h"

template <typename T>
class Ray {
public:
	Point3<T> origin;
	Point3<T> direction;

	Ray(const Point3<T> &origin, const Point3<T> &direction) : origin(origin), direction(direction) {};
	Ray(const T &ox, const T &oy, const T &oz, const T &dx, const T &dy, const T &dz) : origin(Point3<T>(ox, oy, oz)), direction(Point3<T>(dx, dy, dz)) {};

	bool intersects(const PlaneF &plane) const;
	bool intersects(const TriangleF &triangle) const;

	F32 distance(const TriangleF &triangle) const;
	Point3<T> intersection(const PlaneF &plane) const;
};

#endif
