#ifndef STATEMAP_HXX
#define STATEMAP_HXX

#include "JvmObject.hxx"

class StateMap : public JvmObject<StateMap>
{
    friend class JvmObject<StateMap>;

private:
    StateMap(jobject stateMap);
};

#endif // STATEMAP_HXX
