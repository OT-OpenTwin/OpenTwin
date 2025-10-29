// @otlicense

#pragma once
namespace DataStorageAPI
{
	class __declspec(dllexport) ArrayBinaryConverter
	{
	public:
		template<typename T> static  bsoncxx::types::b_binary convertArrayToBinary(T arr[], const size_t size) {
			bsoncxx::types::b_binary bin_val{
			bsoncxx::binary_sub_type::k_binary, static_cast<std::uint32_t>(size * sizeof(T)), reinterpret_cast<std::uint8_t*>(arr) };
			return bin_val;
		}
		template<typename T> static  T* convertBinaryToArray(bsoncxx::types::b_binary data) {
			T* arrayRef = (T*)data.bytes;
			return arrayRef;
		}
	};
}