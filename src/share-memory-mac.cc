#include <napi.h>
#include <node.h>
#include <memory>
#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdio>
//#include <Foundation/Foundation.h>
#include "share-memory-mac.h"

Napi::Value CreateShareMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
        return Napi::Boolean::New(env, false);
    if (!info[0].IsString())
        return Napi::Boolean::New(env, false);
    if (!info[1].IsNumber())
        return Napi::Boolean::New(env, false);
    std::string name = info[0].As<Napi::String>().Utf8Value();
    int32_t mapsize = info[1].As<Napi::Number>().Int32Value();
    int32_t page_size = getpagesize();
    int32_t mem_size = mapsize - (mapsize % page_size) + page_size;
    //printf( "memory page size = %d /n",page_size);

    std::hash<std::string> hash_box;
    key_t name_key = hash_box(name);
    int shm_id = shmget(name_key, mem_size, 0666 | IPC_CREAT);
    printf("name=[%s], hashkey=[%d], shm_id=[%d] \n", name.c_str(), name_key, shm_id);
    if (shm_id == -1)
    {
        printf("share memory [%s] create failed. \n", name.c_str());
        return Napi::Boolean::New(env, false);
    }
    else
    {
        _shareMemoryMap[name] = shm_id;
    }

    //unsigned char* shared_memory = (unsigned char*)shmat(shm_id, 0, 0);
    // memset(shared_memory, 99, mem_size);
    // printf("shared_memory[mem_size - 3] = %d \n", shared_memory[mem_size - 3]);
    // shmdt(shared_memory);
    // shmctl(shm_id, IPC_RMID, 0);

    return Napi::Boolean::New(env, true);
}

Napi::Value ReadShareMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() < 2)
        return Napi::Boolean::New(env, false);
    if (!info[0].IsString())
        return Napi::Boolean::New(env, false);
    if (!info[1].IsBuffer())
        return Napi::Boolean::New(env, false);
    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    int32_t page_size = getpagesize();
    int32_t mem_size = bufflen - (bufflen % page_size) + page_size;

    std::hash<std::string> hash_box;
    key_t name_key = hash_box(name);
    int shm_id = shmget(name_key, mem_size, 0666 | IPC_CREAT);
    if (shm_id != -1)
    {
        void *ptr = shmat(shm_id, 0, 0);
        if (ptr)
        {
            memcpy(buff.Data(), ptr, bufflen);
            shmdt(ptr); //引用计数减一
            return Napi::Boolean::New(env, true);
        }
    }
    return Napi::Boolean::New(env, false);
}

Napi::Value ReadShareMemoryFast(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() < 2)
        return Napi::Boolean::New(env, false);
    if (!info[0].IsString())
        return Napi::Boolean::New(env, false);
    if (!info[1].IsBuffer())
        return Napi::Boolean::New(env, false);
    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    std::map<std::string, CachedData>::iterator it = _sharedCachedData.find(name);
    if (it != _sharedCachedData.end())
    {
        memcpy(buff.Data(), _sharedCachedData[name].ptr, bufflen);
        return Napi::Boolean::New(env, true);
    }
    else
    {
        //new cache data
        int32_t page_size = getpagesize();
        int32_t mem_size = bufflen - (bufflen % page_size) + page_size;
        std::hash<std::string> hash_box;
        key_t name_key = hash_box(name);
        int shm_id = shmget(name_key, mem_size, 0666 | IPC_CREAT);
        if (shm_id != -1)
        {
            void *ptr = shmat(shm_id, 0, 0);
            if (ptr)
            {
                _sharedCachedData[name].open_id = shm_id;
                _sharedCachedData[name].ptr = ptr;
                memcpy(buff.Data(), ptr, bufflen);
                return Napi::Boolean::New(env, true);
            }
        }
    }

    return Napi::Boolean::New(env, false);
}

Napi::Value WriteShareMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() < 2)
        return Napi::Boolean::New(env, false);
    if (!info[0].IsString())
        return Napi::Boolean::New(env, false);
    if (!info[1].IsBuffer())
        return Napi::Boolean::New(env, false);
    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    int32_t page_size = getpagesize();
    int32_t mem_size = bufflen - (bufflen % page_size) + page_size;

    std::hash<std::string> hash_box;
    key_t name_key = hash_box(name);
    int shm_id = shmget(name_key, mem_size, 0666 | IPC_CREAT);
    if (shm_id != -1)
    {
        void *ptr = shmat(shm_id, 0, 0);
        if (ptr)
        {
            memcpy(ptr, buff.Data(), bufflen);
            shmdt(ptr); //引用计数减一
            return Napi::Boolean::New(env, true);
        }
    }
    return Napi::Boolean::New(env, false);
}

Napi::Value WriteShareMemoryFast(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() < 2)
        return Napi::Boolean::New(env, false);
    if (!info[0].IsString())
        return Napi::Boolean::New(env, false);
    if (!info[1].IsBuffer())
        return Napi::Boolean::New(env, false);
    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    std::map<std::string, CachedData>::iterator it = _sharedCachedData.find(name);
    if (it != _sharedCachedData.end())
    {
        memcpy(buff.Data(), _sharedCachedData[name].ptr, bufflen);
        return Napi::Boolean::New(env, true);
    }
    else
    {
        //new cache data
        int32_t page_size = getpagesize();
        int32_t mem_size = bufflen - (bufflen % page_size) + page_size;
        std::hash<std::string> hash_box;
        key_t name_key = hash_box(name);
        int shm_id = shmget(name_key, mem_size, 0666 | IPC_CREAT);
        if (shm_id != -1)
        {
            void *ptr = shmat(shm_id, 0, 0);
            if (ptr)
            {
                _sharedCachedData[name].open_id = shm_id;
                _sharedCachedData[name].ptr = ptr;
                memcpy(ptr, buff.Data(), bufflen);
                return Napi::Boolean::New(env, true);
            }
        }
    }

    return Napi::Boolean::New(env, false);
}

Napi::Value DeleteShareMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() < 1)
        return Napi::Boolean::New(env, false);
    if (!info[0].IsString())
        return Napi::Boolean::New(env, false);
    std::string name = info[0].As<Napi::String>().Utf8Value();

    std::map<std::string, CachedData>::iterator itc = _sharedCachedData.find(name);
    if (itc != _sharedCachedData.end())
    {
        shmdt(_sharedCachedData[name].ptr);
        shmctl(_sharedCachedData[name].open_id, IPC_RMID, 0);
        _sharedCachedData.erase(name);
    }

    std::map<std::string, int>::iterator it = _shareMemoryMap.find(name);
    if (it != _shareMemoryMap.end())
    {
        shmctl(_shareMemoryMap[name], IPC_RMID, 0);
        _shareMemoryMap.erase(name);
    }

    return Napi::Boolean::New(env, true);
}