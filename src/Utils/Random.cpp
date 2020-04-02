#include <Vonsai/Utils/Random.hpp>

#include <random>

namespace Vonsai::Random {

static std::mt19937 s_gen(std::random_device{}());

float Float(float a_first, float a_last) { return std::uniform_real_distribution<float>{a_first, a_last}(s_gen); }
float Float01() { return Float(0.f, 1.f); }
float Float() { return Float(0.f, 1000.f); }

int Int(int a_first, int a_last) { return std::uniform_real_distribution<int>{a_first, a_last}(s_gen); }
int Int() { return Int(0, 1000); }

} // namespace Vonsai::Random
