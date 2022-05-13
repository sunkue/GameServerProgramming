/// @ref core
/// @file glm/detail/func_common_simd.inl

#if GLM_ARCH & GLM_ARCH_SSE2_BIT

#include "../simd/common.h"

#include <immintrin.h>

namespace glm{
namespace detail
{
	template <precision P>
	struct compute_abs_vector<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_abs(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_abs_vector<int, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<int, P> call(tvec4<int, P> const & v)
		{
			tvec4<int, P> result(uninitialize);
			result.Data = glm_ivec4_abs(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_floor<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_floor(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_ceil<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_ceil(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_fract<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_fract(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_round<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_round(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_mod<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & x, tvec4<float, P> const & y)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_mod(x.Data, y.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_min_vector<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v1, tvec4<float, P> const & v2)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = _mm_min_ps(v1.Data, v2.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_min_vector<int32, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<int32, P> call(tvec4<int32, P> const & v1, tvec4<int32, P> const & v2)
		{
			tvec4<int32, P> result(uninitialize);
			result.Data = _mm_min_epi32(v1.Data, v2.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_min_vector<uint32, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<int32, P> call(tvec4<uint32, P> const & v1, tvec4<uint32, P> const & v2)
		{
			tvec4<uint32, P> result(uninitialize);
			result.Data = _mm_min_epu32(v1.Data, v2.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_max_vector<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v1, tvec4<float, P> const & v2)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = _mm_max_ps(v1.Data, v2.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_max_vector<int32, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<int32, P> call(tvec4<int32, P> const & v1, tvec4<int32, P> const & v2)
		{
			tvec4<int32, P> result(uninitialize);
			result.Data = _mm_max_epi32(v1.Data, v2.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_max_vector<uint32, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<uint32, P> call(tvec4<uint32, P> const & v1, tvec4<uint32, P> const & v2)
		{
			tvec4<uint32, P> result(uninitialize);
			result.Data = _mm_max_epu32(v1.Data, v2.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_clamp_vector<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & x, tvec4<float, P> const & minVal, tvec4<float, P> const & maxVal)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = _mm_min_ps(_mm_max_ps(x.Data, minVal.Data), maxVal.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_clamp_vector<int32, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<int32, P> call(tvec4<int32, P> const & x, tvec4<int32, P> const & minVal, tvec4<int32, P> const & maxVal)
		{
			tvec4<int32, P> result(uninitialize);
			result.Data = _mm_min_epi32(_mm_max_epi32(x.Data, minVal.Data), maxVal.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_clamp_vector<uint32, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<uint32, P> call(tvec4<uint32, P> const & x, tvec4<uint32, P> const & minVal, tvec4<uint32, P> const & maxVal)
		{
			tvec4<uint32, P> result(uninitialize);
			result.Data = _mm_min_epu32(_mm_max_epu32(x.Data, minVal.Data), maxVal.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_mix_vector<float, bool, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & x, tvec4<float, P> const & y, tvec4<bool, P> const & a)
		{
			__m128i const Load = _mm_set_epi32(-(int)a.w, -(int)a.z, -(int)a.y, -(int)a.x);
			__m128 const Mask = _mm_castsi128_ps(Load);

			tvec4<float, P> Result(uninitialize);
#			if 0 && GLM_ARCH & GLM_ARCH_AVX
				Result.data = _mm_blendv_ps(x.data, y.data, Mask);
#			else
				Result.Data = _mm_or_ps(_mm_and_ps(Mask, y.Data), _mm_andnot_ps(Mask, x.Data));
#			endif
			return Result;
		}
	};
/* FIXME
	template <precision P>
	struct compute_step_vector<float, P, tvec4>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const& edge, tvec4<float, P> const& x)
		{
			tvec4<float, P> result(uninitialize);
			result.data = glm_vec4_step(edge.data, x.data);
			return result;
		}
	};
*/
	template <precision P>
	struct compute_smoothstep_vector<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const& edge0, tvec4<float, P> const& edge1, tvec4<float, P> const& x)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_smoothstep(edge0.Data, edge1.Data, x.Data);
			return result;
		}
	};
}//namespace detail
}//namespace glm

#endif//GLM_ARCH & GLM_ARCH_SSE2_BIT
