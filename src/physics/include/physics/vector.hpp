/**
 * @file vector.h
 * 2D Vector class definition
 */

#pragma once

#include <cmath>
#include <iostream>

namespace physics {
/**
 * 2D Vector class
 */

template <typename T> class Vector {
public:
  Vector();

  Vector(T x, T y);
  /**
   * @brief 	Null vector : {-1, -1}
   */
  static Vector<T> null;

  /**
   * @brief 	Equal to operator for vector comparison
   * @param 	rhs The vector to be compared against
   * @return 	true if vectors are equal, else false
   */
  bool operator==(const Vector<T> &rhs) const;

  /**
   * @brief 	Not equal to operator for vector comparison
   * @param 	rhs The vector to be compared against
   * @return 	true if vectors are not equal, else false
   */
  bool operator!=(const Vector<T> &rhs) const;

  /**
   * @brief 	Vector addition operator
   * @param 	rhs The vector to be added with
   * @return 	Sum of two vectors
   */
  Vector<T> operator+(const Vector<T> &rhs) const;

  /**
   * @brief 	Vector subraction operator
   * @param 	rhs The vector which subtracts the called vector
   * @return 	Difference of two vectors
   */
  Vector<T> operator-(const Vector<T> &rhs) const;

  /**
   * @brief 	Scalar addition operator
   * @param 	scalar The scalar quantity to add
   * @return 	new vector, with added scalar
   */
  template <typename T2> Vector<T> operator+(const T2 &scalar) const;

  /**
   * @brief 	Scalar subtraction operator
   * @param 	scalar The scalar quantity to subtract
   * @return 	new vector, with subtracted scalar
   */
  template <typename T2> Vector<T> operator-(const T2 &scalar) const;

  /**
   * @brief 	Scalar multiplication operator
   * @param 	scalar The scalar quantity to multiply
   * @return 	new scaled vector
   */
  template <typename T2> Vector<T> operator*(const T2 &scalar) const;

  /**
   * @brief 	Scalar division operator
   * @param 	scalar The scalar quantity to divide by
   * @return 	new scaled vector, or null vector if scalar is 0
   */
  template <typename T2> Vector<T> operator/(const T2 &scalar) const;

  /**
   * @brief 	Global function to overload << ostream operator
   * @param 	out ostream instance
   * @param 	vector vector to be output streamed
   * @return 	ostream instance
   */
  template <typename T2>
  friend std::ostream &operator<<(std::ostream &out, const Vector<T2> &vector);

  /**
   * @brief Check for non-null vector
   * @return true, if non-null vector, false if null vector
   */
  explicit operator bool() const;

  /**
   * @brief 	Dot product of two vectors
   * @param 	rhs The vector with which dot product is computed
   * @return 	scalar value, which is the result of dot product
   */
  double dot(const Vector<T> &rhs) const;

  /**
   * @brief 	Magnitude of a vector
   * @return 	scalar value, which represents magnitude
   */
  double magnitude() const;

  /**
   * @brief 	Distance between two vectors
   * @param 	rhs The vector with which distance is computed
   * @return 	Euclidean distance between two vectors
   */
  double distance(const Vector<T> &other) const;

  /**
   * @brief Calculates the floor of vector members
   * @return new vector with floored values
   */
  Vector<T> floor() const;

  /**
   * @brief Calculates the ceil of vector members
   * @return new vector with ceiled values
   */
  Vector<T> ceil() const;

  /**
   * @brief Return a new Vector<double_t> from another Vector type
   * @return Vector<double_t>
   */
  Vector<double_t> to_double() const;

  /**
   * @brief Return a new Vector<int64_t> from another Vector type
   * @return Vector<int64_t>
   */
  Vector<int64_t> to_int() const;

  T x, y;
};

template <typename T> Vector<T> Vector<T>::null = Vector<T>{-1, -1};

template <typename T> Vector<T>::Vector() : x(), y() {}

template <typename T> Vector<T>::Vector(T x, T y) : x(x), y(y) {}

template <typename T> bool Vector<T>::operator==(const Vector<T> &rhs) const {
  return (x == rhs.x && y == rhs.y);
}

template <typename T> bool Vector<T>::operator!=(const Vector<T> &rhs) const {
  return (x != rhs.x || y != rhs.y);
}

template <typename T>
Vector<T> Vector<T>::operator+(const Vector<T> &rhs) const {
  return Vector(x + rhs.x, y + rhs.y);
}

template <typename T>
Vector<T> Vector<T>::operator-(const Vector<T> &rhs) const {
  return Vector(x - rhs.x, y - rhs.y);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator+(const T2 &scalar) const {
  return Vector(x + scalar, y + scalar);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator-(const T2 &scalar) const {
  return Vector(x - scalar, y - scalar);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator*(const T2 &scalar) const {
  return Vector(x * scalar, y * scalar);
}

template <typename T>
template <typename T2>
Vector<T> Vector<T>::operator/(const T2 &scalar) const {
  if (scalar)
    return Vector(x / scalar, y / scalar);
  else
    return null;
}

template <typename T>
std::ostream &operator<<(std::ostream &out, const Vector<T> &vector) {
  out << "(" << vector.x << ", " << vector.y << ")";
  return out;
}

template <typename T> double Vector<T>::dot(const Vector<T> &rhs) const {
  return (x * rhs.x + y * rhs.y);
}

template <typename T> double Vector<T>::magnitude() const {
  return sqrt(x * x + y * y);
}

template <typename T> double Vector<T>::distance(const Vector<T> &other) const {
  return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
}

template <typename T> Vector<T> Vector<T>::floor() const {
  return Vector<T>(std::floor(x), std::floor(y));
}

template <typename T> Vector<T> Vector<T>::ceil() const {
  return Vector<T>(std::ceil(x), std::ceil(y));
}

template <typename T> Vector<double_t> Vector<T>::to_double() const {
  return Vector<double_t>(static_cast<double_t>(x), static_cast<double_t>(y));
}

template <typename T> Vector<int64_t> Vector<T>::to_int() const {
  return Vector<int64_t>(static_cast<int64_t>(x), static_cast<int64_t>(y));
}

template <typename T> Vector<T>::operator bool() const {
  if (*this == Vector<T>::null) {
    return false;
  }
  return true;
}

} // namespace physics
