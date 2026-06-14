#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <typeindex>

#include "fastgltf/types.hpp"

namespace lunaria
{
    template<typename A, typename B>
    concept IsDerivedFromBase = std::is_base_of_v<A, B>;

    template <typename BaseClass>
    class PolyVector
    {
        private:
            std::unordered_map<std::type_index, void*> vectors;

        public:
            template<typename ElemClass>
            requires IsDerivedFromBase<BaseClass, ElemClass>
            void add(ElemClass obj)
            {
                GetVectorOfClass<ElemClass>()->push_back(obj);
            }

            template<typename ElemClass>
            requires IsDerivedFromBase<BaseClass, ElemClass>
            std::vector<ElemClass>* GetVectorOfClass()
            {
                void* finalvector = nullptr;
                for(const auto& [key, vector] : vectors)
                {
                    if(key == std::type_index(typeid(ElemClass)))
                    {
                        finalvector = static_cast<std::vector<ElemClass> *>(vector);
                    }
                }
                if(finalvector == nullptr)
                {
                    vectors[std::type_index(typeid(ElemClass))] = new std::vector<ElemClass>*;
                    return (std::vector<ElemClass>*)vectors[std::type_index(typeid(ElemClass))];
                }
                return finalvector;
            }
    };
}
