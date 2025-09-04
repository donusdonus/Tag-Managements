#include "TagGroup.h"

    bool TagGroup::Add(isType type,const char * name,size_t array_size=1,isMemory memtype = isMemory::RAM)
    {
        Tag* newTag = (Tag*)_calloc(memtype,1,sizeof(Tag));   
        
        if(newTag == nullptr)
            return false;

        if(newTag->init(type,name,array_size,memtype) == false)
            return false;
        

        if(Tags.Add(newTag) == false)
        {
            newTag->Free();
            newTag = nullptr;
        }
        
        return true;
    }

    bool TagGroup::Add(isType type,const char * name,void *addr,size_t array_size=1,isMemory memtype = isMemory::RAM)
    {
        Tag* newTag = (Tag*)_calloc(memtype,1,sizeof(Tag));   
        
        if(newTag == nullptr)
            return false;

        if(newTag->init(type,name,addr,array_size,memtype) == false)
            return false;
        
        if(Tags.Add(newTag) == false)
        {
            newTag->Free();
            newTag = nullptr;
        }
        
        return true;
    }


    void TagGroup::ForEach(func_search func = nullptr)
    {
        if(func == nullptr)
            return;

        Tags.Find(func);
    }

    Tag * TagGroup::FindByIndex(size_t index)
    {
        return Tags[index];
    }
    
    Tag * TagGroup::FindByName(char *name)
    {
        Tag *tmp = nullptr;
        size_t len = Tags.Count();

        Tags.Find([](Tag *target,size_t index,void *arg){


        },name);
        
        return nullptr;
    }