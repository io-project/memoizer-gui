#include "EventService.hxx"

EventService::EventService(JNIEnv *jniEnv, jobject eventService) :
    JvmObject<EventService>(jniEnv,eventService)
{
}
