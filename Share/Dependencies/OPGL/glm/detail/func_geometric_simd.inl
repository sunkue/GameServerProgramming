/// @ref core
/// @file glm/detail/func_geometric_simd.inl

#include "../simd/geometric.h"

#if GLM_ARCH & GLM_ARCH_SSE2_BIT

namespace glm{
namespace detail
{
	template <precision P>
	struct compute_length<tvec4, float, P, true>
	{
		GLM_FUNC_QUALIFIER static float call(tvec4<float, P> const & v)
		{
			return _mm_cvtss_f32(glm_vec4_length(v.Data));
		}
	};

	template <precision P>
	struct compute_distance<tvec4, float, P, true>
	{
		GLM_FUNC_QUALIFIER static float call(tvec4<float, P> const & p0, tvec4<float, P> const & p1)
		{
			return _mm_cvtss_f32(glm_vec4_distance(p0.Data, p1.Data));
		}
	};

	template <precision P>
	struct compute_dot<tvec4, float, P, true>
	{
		GLM_FUNC_QUALIFIER static float call(tvec4<float, P> const& x, tvec4<float, P> const& y)
		{
			return _mm_cvtss_f32(glm_vec1_dot(x.Data, y.Data));
		}
	};

	template <precision P>
	struct compute_cross<float, P, true>
	{
		GLM_FUNC_QUALIFIER static tvec3<float, P> call(tvec3<float, P> const & a, tvec3<float, P> const & b)
		{
			__m128 const set0 = _mm_set_ps(0.0f, a.z, a.y, a.x);
			__m128 const set1 = _mm_set_ps(0.0f, b.z, b.y, b.x);
			__m128 const xpd0 = glm_vec4_cross(set0, set1);

			tvec4<float, P> result(uninitialize);
			result.Data = xpd0;
			return tvec3<float, P>(result);
		}
	};

	template <precision P>
	struct compute_normalize<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const & v)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_normalize(v.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_faceforward<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const& N, tvec4<float, P> const& I, tvec4<float, P> const& Nref)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_faceforward(N.Data, I.Data, Nref.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_reflect<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const& I, tvec4<float, P> const& N)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_reflect(I.Data, N.Data);
			return result;
		}
	};

	template <precision P>
	struct compute_refract<float, P, tvec4, true>
	{
		GLM_FUNC_QUALIFIER static tvec4<float, P> call(tvec4<float, P> const& I, tvec4<float, P> const& N, float eta)
		{
			tvec4<float, P> result(uninitialize);
			result.Data = glm_vec4_refract(I.Data, N.Data, _mm_set1_ps(eta));
			return result;
		}
	};
}//namespace detail
}//namespace glm

#endif//GLM_ARCH & GLM_ARCH_SSE2_BIT
