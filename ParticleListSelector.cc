
// vi: tabstop=4:expandtab

#include "ParticleListSelector.hh"

ParticleListSelector::ParticleListSelector()
: sys(*gcpe()->GetMMSystem())
{
}

void SelectHandshakeParticles::begin() const
{
    /* TODO We could try to hijack the active epoch if we are
            allowed to read the piggyback data. In this case we
        cannot call AccessEnd() in end()
     */
    CHECK_ALWAYS(!this->sys.CanAccess(), "Another access epoch is already active.");
    this->sys.AccessBegin(md::interface::System::RD_PIGGYBACK);
}

void SelectHandshakeParticles::end() const
{
    this->sys.AccessEnd();
}

void SelectNonHandshakeParticles::begin() const
{
    CHECK_ALWAYS(!this->sys.CanAccess(), "Another access epoch is already active.");
    this->sys.AccessBegin(md::interface::System::RD_PIGGYBACK);
}

void SelectNonHandshakeParticles::end() const
{
    this->sys.AccessEnd();
}

SelectParticlesInBox::SelectParticlesInBox(const polylib::Box<double,DIM>& B)
: box(B)
{
}

void SelectParticlesInBox::begin() const
{
    CHECK_ALWAYS(!this->sys.CanAccess(), "Another access epoch is already active.");
    this->sys.AccessBegin(md::interface::System::RD_PIGGYBACK |
                  md::interface::System::RD_REFP);
}

void SelectParticlesInBox::end() const
{
    this->sys.AccessEnd();
}

