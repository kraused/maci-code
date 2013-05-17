
// vi: tabstop=4:expandtab

#include "TimeIntegrator.hh"
#include "DesignByContract.hh"
#include "DampingMethod.hh"
#include "PluginTable.hh"


TimeIntegrator::TimeIntegrator(Maci* app)
: Pointers(app), damper(NULL), corrector(NULL)
{
}

void TimeIntegrator::plugins_setup()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->setup();
}

void TimeIntegrator::plugins_pre_integration()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->pre_integration();
}

void TimeIntegrator::plugins_post_integration()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->post_integration();
}

void TimeIntegrator::plugins_pre_force()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->pre_force();
}

void TimeIntegrator::plugins_post_force()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->post_force();
}

void TimeIntegrator::plugins_pre_step()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->pre_step();
}

void TimeIntegrator::plugins_post_step()
{
    CHECK_PRECOND_ALWAYS(plugins);
    plugins->post_step();
}

void TimeIntegrator::ApplyDamper()
{
    CHECK_PRECOND_ALWAYS(damper);
    damper->Apply();
}

