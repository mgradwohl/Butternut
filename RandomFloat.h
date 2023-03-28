#pragma once

#include <random>
#include <glm/glm.hpp>

namespace Util
{
    template<std::uniform_random_bit_generator T = std::random_device>
    class seed_generator
    {
    private:
        T m_Rng;

    public:
        using result_type = uint32_t;

        template<typename... Args>
        seed_generator(Args &&...args) : m_Rng(std::forward<Args>(args)...)
        { }

        template<typename Iterator>
        void generate(Iterator first, Iterator last)
        {
            std::uniform_int_distribution<result_type> dist;
            std::generate(first, last, [this, &dist]
                {
                    return dist(m_Rng);
                });
        }
    };

    class RandomFloat
	{
		public:
		    static float Float()
		    {
			    return s_Distribution(s_RandomEngine);
		    }

            //static glm::vec3 Vec3()
            //{
            //    return glm::vec3(Float(), Float(), Float());
            //}

            static glm::vec3 Vec3WithinCone()
            {
                return glm::vec3(Float(), Float(), Float());
            }

        private:
            static thread_local std::ranlux48 s_RandomEngine;
            static thread_local std::uniform_real_distribution<float> s_Distribution;
    };

    static seed_generator s_Gen;
    thread_local std::ranlux48 RandomFloat::s_RandomEngine(s_Gen);
    thread_local std::uniform_real_distribution<float> RandomFloat::s_Distribution(-0.5f, std::nextafter(0.5f, std::numeric_limits<float>::max()));
}

