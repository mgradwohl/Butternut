#include "XoshiroCpp.h"
#include <glm/glm.hpp>

namespace Util
{
    class RandomVecWithinCone
    {
    public:
        static float Float() noexcept
        {
            //return s_Distribution(rng);
            
            return XoshiroCpp::FloatFromBits((uint32_t)rng()) - 0.5f;
        }

        static glm::vec3 Vec3WithinCone()
        {
            return glm::vec3(Float(), Float(), Float());
        }

    private:
        //static thread_local std::uniform_real_distribution<float> s_Distribution;
        static thread_local XoshiroCpp::Xoshiro256PlusPlus rng;
    };

    thread_local XoshiroCpp::Xoshiro256PlusPlus RandomVecWithinCone::rng(05127003142007);
    //thread_local std::uniform_real_distribution<float> RandomVecWithinCone::s_Distribution(-0.5f, std::nextafter(0.5f, 1.0f));
}