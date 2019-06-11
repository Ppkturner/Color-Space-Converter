#pragma once
#ifndef MATRIX_3X3
#define MATRIX_3X3

#include "CommonIncludes.h"

// Wrapper class for a 3x3 matrix expressed as an arary of 9 doubles
class Matrix_3x3
{
public:

	double matrix[9];

public:

	Matrix_3x3();

	Matrix_3x3(double other[9]);

	Matrix_3x3(const Matrix_3x3 &other);

	Matrix_3x3(Matrix_3x3 &&other);

	Matrix_3x3 & operator=(Matrix_3x3 other);

	Matrix_3x3 & operator+=(Matrix_3x3 other);

	Matrix_3x3 & operator*=(Matrix_3x3 other);

	Matrix_3x3 Invert();

	friend void swap(Matrix_3x3 &first, Matrix_3x3 &second)
	{
		using std::swap;

		swap(first.matrix, second.matrix);
	}

	void printMatrix();
};

Matrix_3x3 operator+(Matrix_3x3 first, Matrix_3x3 &second);

Matrix_3x3 operator*(Matrix_3x3 first, Matrix_3x3 &second);

// Multiply a 3x1 matrix with a 3x3 matrix
std::vector<double> operator*(Matrix_3x3 matrix, double vector[3]);

std::vector<double> operator*(Matrix_3x3 matrix, std::vector<double> vector);

// Multiply a 3x3 matrix by a scalar value
Matrix_3x3 operator*(Matrix_3x3 matrix, double scalar);

#endif