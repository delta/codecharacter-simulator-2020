#include "physics/vector.hpp"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace physics;
using namespace testing;

TEST(VectorTest, EqualityTest) {
  Vector<int64_t> a1 = {2, 4};
  Vector<int64_t> a2 = {2, 4};

  ASSERT_EQ(a1, a2);
}

TEST(VectorTest, AdditionTest) {
  Vector<int32_t> a = {2, 4};
  Vector<int32_t> b = {1, 2};

  Vector<int32_t> c = {3, 6};
  ASSERT_EQ(a + b, c);
}

TEST(VectorTest, SubtractionTest) {
  Vector<int32_t> a = {3, 6};
  Vector<int32_t> b = {1, 2};

  Vector<int32_t> c = {2, 4};
  ASSERT_EQ(a - b, c);
}

TEST(VectorTest, ScalarMultiplicationTest) {
  Vector<float> a = {2, 4};
  int scalar = 3;

  Vector<float> ans = {6, 12};
  ASSERT_EQ(a * scalar, ans);
}

TEST(VectorTest, ScalarDivisionTest) {
  // when scalar is not 0
  Vector<float> a1 = {2, 4};
  int scalar1 = 2;

  Vector<float> ans1 = {1, 2};
  ASSERT_EQ(a1 / scalar1, ans1);

  // When scalar is 0
  Vector<float> a2 = {2, 4};
  int scalar2 = 0;

  Vector<float> ans2 = Vector<float>::null;
  ASSERT_EQ(a1 / scalar2, ans2);
}

TEST(VectorTest, DotProductTest) {
  Vector<float> a = {1, 2};
  Vector<float> b = {0.5, 1};

  double c = 2.5;
  ASSERT_EQ(a.dot(b), c);
}

TEST(VectorTest, MagnitudeTest) {
  Vector<uint8_t> a = {3, 4};

  double mag = 5;
  ASSERT_EQ(a.magnitude(), mag);
}

TEST(VectorTest, FloorTest) {
  Vector<double> a = {3.99, 4.01};

  Vector<double> ans = {3, 4};
  ASSERT_EQ(a.floor(), ans);
}

TEST(VectorTest, CeilTest) {
  Vector<double> a = {5.69, 1.01};

  Vector<double> ans = {6, 2};
  ASSERT_EQ(a.ceil(), ans);
}
