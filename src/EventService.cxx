#include "EventService.hxx"

#include "LocalRefsGuard.hxx"
#include "JvmException.hxx"

void EventService::shutdown(JNIEnv *jniEnv)
{
    LocalRefsGuard localRefs(jniEnv);
    jclass shutdownEventClass=jniEnv->FindClass("pl/edu/uj/tcs/memoizer/runtime/ShutdownEvent");
    localRefs.checkIn(shutdownEventClass);
    jmethodID shutdownEventInit=jniEnv->GetMethodID(
                shutdownEventClass,
                "<init>",
                "()V");
    localRefs.check(shutdownEventInit);
    jobject shutdownEvent=jniEnv->NewObject(shutdownEventClass,shutdownEventInit);
    localRefs.checkIn(shutdownEvent);
    localRefs.remove(shutdownEventClass);
    jclass eventServiceClass=jniEnv->GetObjectClass(unwrap());
    localRefs.checkIn(eventServiceClass);
    jmethodID callId=jniEnv->GetMethodID(
                eventServiceClass,
                "call",
                "(Lpl/edu/uj/tcs/memoizer/events/IEvent;)V");
    localRefs.check(callId);
    localRefs.remove(eventServiceClass);
    jniEnv->CallVoidMethod(unwrap(),callId,shutdownEvent);
    JvmException::checkEnv(jniEnv);
}

EventService::EventService(JNIEnv *jniEnv, jobject eventService) :
    JvmObject<EventService>(jniEnv,eventService)
{
}
