// @otlicense

#include "FixtureProgressUpdater.h"
#include "OTServiceFoundation/UiComponent.h"

FixtureProgressUpdater::FixtureProgressUpdater()
{
    //ot::components::UiComponent* comp = new ot::components::UiComponent("","","",0,nullptr);
    //m_updater.m_uiComponent = comp;
}

uint64_t FixtureProgressUpdater::getTriggerFrequency() const
{
    return m_updater.m_triggerFrequency;
}
