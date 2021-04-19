#ifndef SHARED_MEMORY_MAC_H
#define SHARED_MEMORY_MAC_H

#include <napi.h>
#include <map>

struct CachedData
{
    int open_id;
    void* ptr;
};

static std::map<std::string, int> _shareMemoryMap; 
static std::map<std::string, CachedData> _sharedCachedData;

Napi::Value CreateShareMemory(const Napi::CallbackInfo &info);
Napi::Value ReadShareMemory(const Napi::CallbackInfo &info);
Napi::Value ReadShareMemoryFast(const Napi::CallbackInfo &info);
Napi::Value WriteShareMemory(const Napi::CallbackInfo &info);
Napi::Value WriteShareMemoryFast(const Napi::CallbackInfo &info);
Napi::Value DeleteShareMemory(const Napi::CallbackInfo &info);

#endif