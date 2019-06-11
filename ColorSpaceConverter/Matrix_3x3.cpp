#include "Matrix_3x3.h"

Matrix_3x3::Matrix_3x3()
{
	// Initialize array to identity matrix
	matrix[0] = 1;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 1;
	matrix[5] = 0;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 1;
}

Matrix_3x3::Matrix_3x3(double other[9])
{
	for (int ii = 0; ii < 9; ii++)
	{
		this->matrix[ii] = other[ii];
	}
}

Matrix_3x3::Matrix_3x3(const Matrix_3x3 & other)
{
	for (int ii = 0; ii < 9; ii++)
	{
		this->matrix[ii] = other.matrix[ii];
	}
}

Matrix_3x3::Matrix_3x3(Matrix_3x3 && other)
{
	swap(*this, other);
}

Matrix_3x3 & Matrix_3x3::operator=(Matrix_3x3 other)
{
	swap(*this, other);

	return *this;
}

Matrix_3x3 & Matrix_3x3::operator+=(Matrix_3x3 other)
{
	for (int ii = 0; ii < 9; ii++)
	{
		this->matrix[ii] += other.matrix[ii];
	}

	return *this;
}

Matrix_3x3 & Matrix_3x3::operator*=(Matrix_3x3 other)
{
	/*
	 | a1 a2 a3 |   | b1 b2 b3 |   | a1b1 + a2b4 + a3b7, a1b2 + a2b5 + a3b8, a1b3 + a2b6 + a3b9 |
	 | a4 a5 a6 | * | b4 b5 b6 | = | a4b1 + a5b4 + a6b7, a4b2 + a5b5 + a6b8, a4b3 + a5b6 + a6b9 |
	 | a7 a8 a9 |	| b7 b8 b9 |   | a7b1 + a8b4 + a9b7, a7b2 + a8b5 + a9b8, a7b3 + a8b6 + a9b9 |
	*/

	double a1 = this->matrix[0];
	double a2 = this->matrix[1];
	double a3 = this->matrix[2];
	double a4 = this->matrix[3];
	double a5 = this->matrix[4];
	double a6 = this->matrix[5];
	double a7 = this->matrix[6];
	double a8 = this->matrix[7];
	double a9 = this->matrix[8];

	double b1 = other.matrix[0];
	double b2 = other.matrix[1];
	double b3 = other.matrix[2];
	double b4 = other.matrix[3];
	double b5 = other.matrix[4];
	double b6 = other.matrix[5];
	double b7 = other.matrix[6];
	double b8 = other.matrix[7];
	double b9 = other.matrix[8];

	this->matrix[0] = a1 * b1 + a2 * b4 + a3 * b7;
	this->matrix[1] = a1 * b2 + a2 * b5 + a3 * b8;
	this->matrix[2] = a1 * b3 + a2 * b6 + a3 * b9;

	this->matrix[3] = a4 * b1 + a5 * b4 + a6 * b7;
	this->matrix[4] = a4 * b2 + a5 * b5 + a6 * b8;
	this->matrix[5] = a4 * b3 + a5 * b6 + a6 * b9;

	this->matrix[6] = a7 * b1 + a8 * b4 + a9 * b7;
	this->matrix[7] = a7 * b2 + a8 * b5 + a9 * b8;
	this->matrix[8] = a7 * b3 + a8 * b6 + a9 * b9;

	return *this;
}

Matrix_3x3 Matrix_3x3::Invert()
{
	// Inv(A) = (1 / det(A)) * adj(A)
	// adj() is the adjugate of A, det(A) is the determinant of A

	// calculate the Adjugate of the matrix
	// adj(A) = transpose(cofactor(A))

	double a11 = this->matrix[0];
	double a12 = this->matrix[1];
	double a13 = this->matrix[2];
	double a21 = this->matrix[3];
	double a22 = this->matrix[4];
	double a23 = this->matrix[5];
	double a31 = this->matrix[6];
	double a32 = this->matrix[7];
	double a33 = this->matrix[8];

	// Get the cofactor of A
	double adjugate[9] =
	{
		(a22 * a33) - (a23 * a32), -1 * ((a21 * a33) - (a23 * a31)), (a21 * a32) - (a31 * a22),
		-1 * ((a12 * a33) - (a13 * a32)), (a11 * a33) - (a13 * a31), -1 * ((a11 * a32) - (a12 * a31)),
		(a12 * a23) - (a13 * a22), -1 * ((a11 * a23) - (a21 * a13)), (a11 * a22) - (a21 * a12)
	};

	// get the transpose of the cofactor matrix

	std::swap(adjugate[1], adjugate[3]);
	std::swap(adjugate[2], adjugate[6]);
	std::swap(adjugate[7], adjugate[5]);

	// Calculate the determinant of the 3x3 matrix

	double determinant =
		a11 * ((a22 * a33) - (a23 * a32)) -
		a12 * ((a21 * a33) - (a23 * a31)) +
		a13 * ((a21 * a32) - (a22 * a31));

	for (int ii = 0; ii < 9; ii++)
		adjugate[ii] *= (1 / determinant);

	return Matrix_3x3(adjugate);
}

void Matrix_3x3::printMatrix()
{
	printf("| %lf, %lf, %lf, |\n| %lf, %lf, %lf, |\n| %lf, %lf, %lf |\n",
		this->matrix[0], this->matrix[1], this->matrix[2],
		this->matrix[3], this->matrix[4], this->matrix[5],
		this->matrix[6], this->matrix[7], this->matrix[8]);
}

Matrix_3x3 operator+(Matrix_3x3 first, Matrix_3x3 & second)
{
	first += second;

	return first;
}

Matrix_3x3 operator*(Matrix_3x3 first, Matrix_3x3 & second)
{
	first *= second;

	return first;
}

std::vector<double> operator*(Matrix_3x3 matrix, double vector[3])
{
	std::vector<double> output = 
	{
		matrix.matrix[0] * vector[0] + matrix.matrix[1] * vector[1] + matrix.matrix[2] * vector[2],
		matrix.matrix[3] * vector[0] + matrix.matrix[4] * vector[1] + matrix.matrix[5] * vector[2],
		matrix.matrix[6] * vector[0] + matrix.matrix[7] * vector[1] + matrix.matrix[8] * vector[2]
	};

	return output;
}

std::vector<double> operator*(Matrix_3x3 matrix, std::vector<double> vector)
{
	std::vector<double> output(3);

	if (vector.size() == 3)
	{
		output[0] = matrix.matrix[0] * vector[0] + matrix.matrix[1] * vector[1] + matrix.matrix[2] * vector[2];
		output[1] = matrix.matrix[3] * vector[0] + matrix.matrix[4] * vector[1] + matrix.matrix[5] * vector[2];
		output[2] = matrix.matrix[6] * vector[0] + matrix.matrix[7] * vector[1] + matrix.matrix[8] * vector[2];
	}

	return output;
}

Matrix_3x3 operator*(Matrix_3x3 matrix, double scalar)
{
	for (int ii = 0; ii < 9; ii++)
		matrix.matrix[ii] *= scalar;

	return matrix;
}
