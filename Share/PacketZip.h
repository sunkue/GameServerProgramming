#pragma once
#include "Includes.h"
#include <array>
/////////////////////////////////////////////////////

#define SUNKUE_NET Net

/////////////////////////////////////////////////////

namespace SUNKUE_NET
{
	constexpr auto float_precision1 = 1;
	constexpr auto float_precision2 = 2;

	template<class compressedT, size_t N>
	struct nt
	{
		std::array<compressedT, N> Data;

		using this_type = nt<compressedT, N>;
		template<class originT> inline static nt<compressedT, N> encode(const originT& var);
		template<class originT> inline static originT decode(const nt<compressedT, N>& var);
	};
};

namespace SUNKUE_NET
{
#define NT_ENCODE(type, n, origin_type)template<> template<> inline nt<type, n> nt<type, n>::encode(const origin_type& var)
#define NT_DECODE(type, n, origin_type)template<> template<> inline origin_type nt<type, n>::decode(const nt<type, n>& var)

	//////////////////////////////////////////////////////////////////////////////
	
	NT_ENCODE(int8, 3, glm::vec3)
	{
		constexpr auto mul = float_precision1 * 10;
		this_type ret{};
		ret.Data.at(0) = (char)round(var.x * mul);
		ret.Data.at(1) = (char)round(var.y * mul);
		ret.Data.at(2) = (char)round(var.z * mul);
		return ret;
	};

	NT_DECODE(int8, 3, glm::vec3)
	{
		constexpr auto div = float_precision1 * 10;
		glm::vec3 ret
		{
			(float)var.Data.at(0),
			(float)var.Data.at(1),
			(float)var.Data.at(2)
		};
		ret /= div;
		return ret;
	};

	//////////////////////////////////////////////////////////////////////////////
	
	NT_ENCODE(uint8, 2, glm::vec2)
	{
		this_type ret{};
		ret.Data.at(0) = static_cast<unsigned int>(round(var.x));
		ret.Data.at(1) = static_cast<unsigned int>(round(var.y));
		return ret;
	};

	NT_DECODE(uint8, 2, glm::vec2)
	{
		glm::vec2 ret
		{
			static_cast<float>(var.Data.at(0)),
			static_cast<float>(var.Data.at(1))
		};
		return ret;
	};

	//////////////////////////////////////////////////////////////////////////////
};



/////////////////////////////////////////////////////

	/*
int main()
{
	vec3 origin{ 1.18f, 2.21f, -3.36f };

	auto net_data = Net::nt<char, 3>::encode<vec3>(origin);
	auto rec_data = Net::nt<int8, 3>::decode<vec3>(net_data);

	std::cout << rec_data.x << std::endl;
	std::cout << rec_data.y << std::endl;
	std::cout << rec_data.z << std::endl;
}
	*/