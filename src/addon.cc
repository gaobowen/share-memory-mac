#include <napi.h>
#include "share-memory-mac.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "CreateShareMemory"), Napi::Function::New(env, CreateShareMemory));
    exports.Set(Napi::String::New(env, "ReadShareMemory"), Napi::Function::New(env, ReadShareMemory));
    exports.Set(Napi::String::New(env, "ReadShareMemoryFast"), Napi::Function::New(env, ReadShareMemoryFast));
    exports.Set(Napi::String::New(env, "WriteShareMemory"), Napi::Function::New(env, WriteShareMemory));
    exports.Set(Napi::String::New(env, "WriteShareMemoryFast"), Napi::Function::New(env, WriteShareMemoryFast));
    exports.Set(Napi::String::New(env, "DeleteShareMemory"), Napi::Function::New(env, DeleteShareMemory));
    return exports;
}

NODE_API_MODULE(share_memory_mac, Init)