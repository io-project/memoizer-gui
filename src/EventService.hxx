#ifndef EVENTSERVICE_HXX
#define EVENTSERVICE_HXX

#include "JvmObject.hxx"

class EventService : public JvmObject<EventService>
{
    friend class JvmObject<EventService>;
private:
    EventService(JNIEnv *jniEnv, jobject eventService);
};

#endif // EVENTSERVICE_HXX
