//go:build darwin

#include <CoreFoundation/CoreFoundation.h>
#include "greet.h"
#include "xpc.h"

static void ConnectionHandler(xpc_connection_t connection, xpc_object_t clientMessage){

    xpc_object_t response = xpc_dictionary_create_reply(clientMessage);
    const char *name = xpc_dictionary_get_string(clientMessage, "name");

    const char *greeting = getGreeting(name); // this is the Go function I am calling from C

    xpc_dictionary_set_string(response, "response", greeting);
    free((void*)greeting);
    xpc_connection_send_message(connection, response);
    xpc_release(response);
}

static void ListenerHandler(xpc_connection_t connection){
    xpc_connection_set_event_handler(connection, ^(xpc_object_t clientMessage) {
      ConnectionHandler(connection, clientMessage);
    });
    xpc_connection_resume(connection);
}

void runXPCServer() {

    dispatch_queue_t xpcQueue = dispatch_queue_create("my.xpc.queue", NULL);

    // Do I need to free "listener" explicitly
    xpc_connection_t listener = xpc_connection_create_mach_service("my.xpc.server", xpcQueue, XPC_CONNECTION_MACH_SERVICE_LISTENER);

    xpc_connection_set_event_handler(listener, ^(xpc_object_t connection) {
       ListenerHandler(connection);
    });

    xpc_connection_resume(listener);

    dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, xpcQueue);
    dispatch_source_set_event_handler(source, ^{
        dispatch_async(xpcQueue, ^{
        });
    });

    // dispatch_resume never returns, so how can I free "source", "xpcQueue", "listener" here? 
    dispatch_resume(source); 
}