#pragma once
#include <stdint.h>
#include <string>

#pragma warning(disable:4251)

namespace ot
{
	namespace app
	{
		class __declspec(dllexport) RunResult {
		public:
			const static uint32_t successCode = 0;
			const static uint32_t genericErrorCode = 1;

			bool isOk()
			{
				return m_value == successCode;
			}

			void setAsError(uint32_t _errorCode)
			{
				if (_errorCode == successCode)
				{
					m_value = genericErrorCode;
					m_fallBackToGenericErrorCode = true;
				}
				else
				{
					m_value = _errorCode;
				}
			}

			uint32_t getErrorCode() const
			{
				return m_value;
			}
			
			void setErrorMessage(const std::string& _message)
			{
				m_message = _message;
			}
			void addToErrorMessage(const std::string& _message)
			{
				m_message += _message;
			}

			const std::string getErrorMessage() const 
			{
				if (m_fallBackToGenericErrorCode)
				{
					return m_message + std::string("\nOrriginal error code was 0 (success), but it was logged as error");
				}
				return m_message;
			}

			RunResult() = default;
			RunResult& operator=(const RunResult& _other) = default;
			RunResult& operator=(RunResult&& _other) = default;


			RunResult(const RunResult& _other) = default;
			RunResult(RunResult&& _other) = default;

			bool operator==(const RunResult& _other)
			{
				return m_value == _other.m_value;
			}

			bool operator!=(const RunResult& _other)
			{
				return !(*this == _other);
			}

		private:
			std::string m_message = "";
			uint32_t m_value = successCode;
			bool m_fallBackToGenericErrorCode = false;
		};
	}
}