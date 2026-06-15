#include "SSOUser.h"

SSOUser::SSOUser(const ot::LogInInfos& _logInInfos)
	: m_logInInfos(_logInInfos)
{
	m_sessionToken.generateToken();
}
