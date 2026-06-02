#pragma once
namespace ot
{
    struct GroupInfo
    {
        std::string m_name;
        std::string m_domain;
        std::string m_sidType;
        bool m_mandatory = false;
        bool m_enabled = false;
        bool m_byDefault = false;
        bool m_owner = false;
        bool m_logonId = false;

        std::string fullyQualifiedName() const
        {
            return m_domain.empty() ? m_name : m_domain + "\\" + m_name;
        }
    };
}
