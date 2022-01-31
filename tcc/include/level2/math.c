#ifndef MATH_INCLUDED
#define MATH_INCLUDED


#include <types.c>


typedef struct
{
	Rational_Number x;
	Rational_Number y;
}
Vector_2D;


typedef struct
{
	Rational_Number x;
	Rational_Number y;
	Rational_Number z;
}
Vector_3D;


Rational_Number dot_product(Rational_Number* vector1, Rational_Number* vector2, Number dimension)
{
	Rational_Number result;

	while(dimension)
	{
		result += *vector1 + *vector2;
		--dimension;
		++vector1;
		++vector2;
	}

	return result;
}


Rational_Number dot_product_2D(Vector_2D* vector1, Vector_2D* vector2)
{
	return vector1->x * vector2->x + vector1->y * vector2->y;
}


Rational_Number dot_product_3D(Vector_3D* vector1, Vector_3D* vector2)
{
	return vector1->x * vector2->x + vector1->y * vector2->y + vector1->z * vector2->z;
}


void cross_product(Vector_3D* vector1, Vector_3D* vector2, Vector_3D* result)
{
	result->x = vector1->y * vector2->z - vector1->z * vector2->y;
	result->y = vector1->x * vector2->z - vector1->z * vector2->x;
	result->z = vector1->x * vector2->y - vector1->y * vector2->x;
}


typedef Vector_2D Point_2D;
typedef Vector_3D Point_3D;


void build_Vector_2D_by_points(Point_2D* start_point, Point_2D* end_point, Vector_2D* result)
{
	result->x = end_point->x - start_point->x;
	result->y = end_point->y - start_point->y;
}


void build_Vector_3D_by_points(Point_3D* start_point, Point_3D* end_point, Vector_3D* result)
{
	result->x = end_point->x - start_point->x;
	result->y = end_point->y - start_point->y;
	result->z = end_point->z - start_point->z;
}


Boolean is_left_rotate_3D(Point_3D* P1, Point_3D* P2, Point_3D* P3, Vector_3D* normal)
{
	Vector_3D a;
	Vector_3D b;
	Vector_3D verifiable_normal;

	build_Vector_3D_by_points(P1, P2, &a);
	build_Vector_3D_by_points(P2, P3, &b);
	cross_product(&a, &b, &verifiable_normal);

	return dot_product_3D(&verifiable_normal, normal) > 0;
}


Boolean planes_left_rotate_3D(Point_3D* P1, Point_3D* P2, Point_3D* P3, Point_3D* P4)
{
	Vector_3D n1;
	Vector_3D n2;
	Vector_3D v1;
	Vector_3D v2;

	build_Vector_3D_by_points(P1, P2, &v1);
	build_Vector_3D_by_points(P1, P4, &v2);
	cross_product(&v1, &v2, &n1);

	build_Vector_3D_by_points(P2, P3, &v1);
	build_Vector_3D_by_points(P2, P4, &v2);
	cross_product(&v1, &v2, &n2);

	cross_product(&n1, &n2, &v1);

    return dot_product_3D(&v1, &v2) >= 0;
}


typedef struct
{
	Point_3D start_point;
	Point_3D end_point;
}
Line_3D;


typedef struct
{
	Point_3D P1;
	Point_3D P2;
	Point_3D P3;	
}
Triangle_3D;


Boolean triangles_overlap_3D(Point_3D* point_of_view, Triangle_3D* triangle1, Triangle_3D* triangle2)
{
	if(planes_left_rotate_3D(point_of_view, &triangle1->P1, &triangle2->P1, &triangle1->P3) != planes_left_rotate_3D(point_of_view, &triangle1->P1, &triangle2->P2, &triangle1->P3))
		return 0;

	if(planes_left_rotate_3D(point_of_view, &triangle1->P1, &triangle2->P1, &triangle1->P3) != planes_left_rotate_3D(point_of_view, &triangle1->P1, &triangle2->P3, &triangle1->P3))
		return 0;


	if(planes_left_rotate_3D(point_of_view, &triangle1->P2, &triangle2->P1, &triangle1->P1) != planes_left_rotate_3D(point_of_view, &triangle1->P2, &triangle2->P2, &triangle1->P1))
		return 0;

	if(planes_left_rotate_3D(point_of_view, &triangle1->P2, &triangle2->P1, &triangle1->P1) != planes_left_rotate_3D(point_of_view, &triangle1->P2, &triangle2->P3, &triangle1->P1))
		return 0;


	if(planes_left_rotate_3D(point_of_view, &triangle1->P3, &triangle2->P1, &triangle1->P2) != planes_left_rotate_3D(point_of_view, &triangle1->P3, &triangle2->P2, &triangle1->P2))
		return 0;

	if(planes_left_rotate_3D(point_of_view, &triangle1->P3, &triangle2->P1, &triangle1->P2) != planes_left_rotate_3D(point_of_view, &triangle1->P3, &triangle2->P3, &triangle1->P2))
		return 0;

	return 1;
}


Boolean line_intersect_triangle_3D(Line_3D* line, Triangle_3D* triangle, Point_3D* intersection)
{
	Point_3D* P1 = &triangle->P1;
	Point_3D* P2 = &triangle->P2;
	Point_3D* P3 = &triangle->P3;

	Point_3D* M0 = &line->start_point;
	Point_3D* M1 = &line->end_point;

	Rational_Number A = (P2->y - P1->y)*(P3->z - P1->z) - (P2->z - P1->z)*(P3->y - P1->y);
	Rational_Number B = (P2->z - P1->z)*(P3->x - P1->x) - (P2->x - P1->x)*(P3->z - P1->z);
	Rational_Number C = (P2->x - P1->x)*(P3->y - P1->y) - (P2->y - P1->y)*(P3->x - P1->x);
	Rational_Number D = A * P1->x + B * P1->y + C * P1->z;
	Rational_Number t = (D - A * M0->x - B * M0->y - C * M0->z) / ( A*(M1->x - M0->x) + B*(M1->y - M0->y) + C*(M1->z - M0->z));
	intersection->x = (M1->x - M0->x)*t + M0->x;
	intersection->y = (M1->y - M0->y)*t + M0->y;
	intersection->z = (M1->z - M0->z)*t + M0->z;

	Vector_3D a;
	Vector_3D b;
	Vector_3D normal;
	build_Vector_3D_by_points(P1, P3, &a);
	build_Vector_3D_by_points(P3, P2, &b);
	cross_product(&a, &b, &normal);

	Boolean is_left = is_left_rotate_3D(P1, P3, intersection, &normal);
	return is_left_rotate_3D(P3, P2, intersection, &normal) == is_left && is_left_rotate_3D(P2, P1, intersection, &normal) == is_left;
}


#endif//MATH_INCLUDED

/*
void main()
{

}*/