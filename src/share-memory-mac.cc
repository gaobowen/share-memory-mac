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

//hash ref: Art Of Computer Programming Volume 3
int DEKHash(const char *str)
{
    if (!*str)
        return 0;
    int hash = 1315423911;
    int ch = (int)*str++;
    do
    {
        hash = ((hash << 5) ^ (hash >> 27)) ^ ch;
        ch = (int)*str++;
    } while (ch);
    return hash;
}

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

    key_t name_key = DEKHash(name.c_str());
    int shm_id = shmget(name_key, mem_size, 0666 | IPC_CREAT);
    printf("name=[%s], hashkey=[%d], shm_id=[%d] \n", name.c_str(), name_key, shm_id);
    if (shm_id == -1)
    {
        shm_id = shmget(name_key, page_size, 0666);
        printf("name=[%s], hashkey=[%d], shm_id=[%d] \n", name.c_str(), name_key, shm_id);
        printf("share memory [%s] create failed. \n", name.c_str());
        return Napi::Boolean::New(env, false);
    }
    else
    {
        _shareMemoryMap[name] = shm_id;
    }

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
    int offset = 0;
    if (info.Length() == 3 && info[2].IsNumber())
    {
        offset = info[2].As<Napi::Number>().Int32Value();
    }

    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    int32_t page_size = getpagesize();
    int32_t mem_size = bufflen - (bufflen % page_size) + page_size;

    key_t name_key = DEKHash(name.c_str());
    int shm_id = shmget(name_key, mem_size, 0666);
    if (shm_id != -1)
    {
        void *ptr = shmat(shm_id, 0, 0);
        if (ptr)
        {
            unsigned char *m_ptr = (unsigned char *)ptr;
            m_ptr += offset;
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
    int offset = 0;
    if (info.Length() == 3 && info[2].IsNumber())
    {
        offset = info[2].As<Napi::Number>().Int32Value();
    }

    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    std::map<std::string, CachedData>::iterator it = _sharedCachedData.find(name);
    if (it != _sharedCachedData.end())
    {
        unsigned char *m_ptr = (unsigned char *)_sharedCachedData[name].ptr;
        m_ptr += offset;
        memcpy(buff.Data(), m_ptr, bufflen);
        return Napi::Boolean::New(env, true);
    }
    else
    {
        //new cache data
        int32_t page_size = getpagesize();
        int32_t mem_size = bufflen - (bufflen % page_size) + page_size;

        key_t name_key = DEKHash(name.c_str());
        int shm_id = shmget(name_key, mem_size, 0666);
        if (shm_id != -1)
        {
            void *ptr = shmat(shm_id, 0, 0);
            if (ptr)
            {
                _sharedCachedData[name].open_id = shm_id;
                _sharedCachedData[name].ptr = ptr;
                unsigned char *m_ptr = (unsigned char *)_sharedCachedData[name].ptr;
                m_ptr += offset;
                memcpy(buff.Data(), m_ptr, bufflen);
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
    int offset = 0;
    if (info.Length() == 3 && info[2].IsNumber())
    {
        offset = info[2].As<Napi::Number>().Int32Value();
    }

    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    int32_t page_size = getpagesize();
    int32_t mem_size = bufflen - (bufflen % page_size) + page_size;

    key_t name_key = DEKHash(name.c_str());
    int shm_id = shmget(name_key, mem_size, 0666);
    if (shm_id != -1)
    {
        void *ptr = shmat(shm_id, 0, 0);
        if (ptr)
        {
            unsigned char *m_ptr = (unsigned char *)ptr;
            m_ptr += offset;
            memcpy(m_ptr, buff.Data(), bufflen);
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
    int offset = 0;
    if (info.Length() == 3 && info[2].IsNumber())
    {
        offset = info[2].As<Napi::Number>().Int32Value();
    }



    std::string name = info[0].As<Napi::String>().Utf8Value();
    Napi::Buffer<unsigned char> buff = info[1].As<Napi::Buffer<unsigned char> >();
    size_t bufflen = buff.ByteLength();

    std::map<std::string, CachedData>::iterator it = _sharedCachedData.find(name);
    if (it != _sharedCachedData.end())
    {
        unsigned char *m_ptr = (unsigned char *)_sharedCachedData[name].ptr;
        m_ptr += offset;
        memcpy(m_ptr, buff.Data(), bufflen);
        return Napi::Boolean::New(env, true);
    }
    else
    {
        //new cache data
        int32_t page_size = getpagesize();
        int32_t mem_size = bufflen - (bufflen % page_size) + page_size;

        key_t name_key = DEKHash(name.c_str());
        int shm_id = shmget(name_key, mem_size, 0666);
        if (shm_id != -1)
        {
            void *ptr = shmat(shm_id, 0, 0);
            if (ptr)
            {
                _sharedCachedData[name].open_id = shm_id;
                _sharedCachedData[name].ptr = ptr;
                unsigned char *m_ptr = (unsigned char *)_sharedCachedData[name].ptr;
                m_ptr += offset;
                //printf("WriteShareMemory %d %ld %ld \n", offset, _sharedCachedData[name].ptr, m_ptr);
                memcpy(m_ptr, buff.Data(), bufflen);
                return Napi::Boolean::New(env, true);
            }
        }
    }

    return Napi::Boolean::New(env, false);
}

//ipcs -m  //ipcrm -m [shm_id]
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
        printf("DeleteShareMemory ptr %s  \n", name.c_str());
        shmdt(_sharedCachedData[name].ptr);
        shmctl(_sharedCachedData[name].open_id, IPC_RMID, 0);
        _sharedCachedData.erase(name);
    }

    std::map<std::string, int>::iterator it = _shareMemoryMap.find(name);
    if (it != _shareMemoryMap.end())
    {
        printf("DeleteShareMemory id %s \n", name.c_str());
        shmctl(_shareMemoryMap[name], IPC_RMID, 0);
        _shareMemoryMap.erase(name);
    }

    return Napi::Boolean::New(env, true);
}