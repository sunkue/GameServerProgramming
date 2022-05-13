/// @ref core
/// @file glm/detail/func_exponential_simd.inl

#include "../simd/exponential.h"

#if GLM_ARCH & GLM_ARCH_SSE2_BIT

namespace glm{
namespace detail
{
	template <precision P>
	struct compute_sqrt<tvec4, float, P, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = _mm_sqrt_ps(v.Data);
			return result;
		}
	};

	template <>
	struct compute_sqrt<tvec4, float, aligned_lowp, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, aligned_lowp> call(tvec4<float, aligned_lowp> const & v)
		{
			tvec4<float, aligned_lowp> result(uninitialize);
			result.Data = glm_vec4_sqrt_lowp(v.Data);
			return result;
		}
	};
}//namespace detail
}//namespace glm

#endif//GLM_ARCH & GLM_ARCH_SSE2_BIT
