
// vi: tabstop=4:expandtab

#include "PatchyFunction.hh"


PatchyFunction::PatchyFunction(double bkgVal)
: bkgVal_(bkgVal)
{
}

PatchyFunction::PatchyFunction(const PatchyFunction& f)
: bkgVal_(f.bkgVal_)
{
    std::vector<GeoPrimitive*>::const_iterator P = f.pr_.begin();   
    for(; P != f.pr_.end(); ++P) {
        pr_.push_back((*P)->Clone());
    }
    std::vector<Function<double,3>*>::const_iterator F = f.fct_.begin();
    for(; F != f.fct_.end(); ++F) {
        fct_.push_back((*F)->Clone());
    }   
}

PatchyFunction::~PatchyFunction()
{
    std::vector<GeoPrimitive*>::iterator P = pr_.begin();   
    for(; P != pr_.end(); ++P) {
        delete *P;
    }
    std::vector<Function<double,3>*>::iterator F = fct_.begin();
    for(; F != fct_.end(); ++F) {
        delete *F;
    }
}

PatchyFunction& PatchyFunction::AddNewPrimitive(const GeoPrimitive* P,
                        const Function<double,3>* f)
{
     pr_.push_back(P->Clone());
    fct_.push_back(f->Clone());
    
    return *this;
}

double PatchyFunction::val(const double* p) const
{
    std::vector<GeoPrimitive*>::const_iterator P =  pr_.begin();
    std::vector<Function<double,3>*>::const_iterator F = fct_.begin();  
    for(; P != pr_.end(); ++P, ++F) {
        if((*P)->IsInside(p)) {
            return (*F)->val(p);
        }
    }
    
    return bkgVal_;
}

PatchyFunction* PatchyFunction::Clone() const
{
    return new PatchyFunction(*this);
}

