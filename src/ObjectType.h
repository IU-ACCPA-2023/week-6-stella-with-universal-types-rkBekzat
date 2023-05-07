//
// Created by bekzat on 29.03.23.
//

#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace Stella{
    class ObjectType {
    public:
        MyTypeTag typeTag;
        int data = -1;

        std::vector<ObjectType> params = {};
        std::vector<std::pair<StellaIdent, ObjectType>> records ;
        std::vector<ObjectType> returns = {};

        ObjectType(){
            typeTag = MyTypeTag::UndefinedTag;
        }

        ObjectType(MyTypeTag tag){
            typeTag = tag;
        }

        ObjectType(MyTypeTag tag, int value){
            typeTag = tag;
            data = value;
        }

        bool contain(StellaIdent ident){
            for(int i = 0; i < records.size(); i++){
                if(records[i].first == ident) {
                    return true;
                }
            }
            return false;
        }

        ObjectType get(StellaIdent ident){
            for(int i = 0; i < records.size(); i++){
                if(records[i].first == ident){
                    return records[i].second;
                }
            }
        }

        bool cmp(std::pair<StellaIdent, ObjectType> a, std::pair<StellaIdent, ObjectType> b){
            return a.first < b.first;
        }

        void add(StellaIdent ident, ObjectType val){
            records.push_back({ident, val});
        }


        ObjectType& operator= (const ObjectType &other){
            typeTag = other.typeTag;
            params = other.params;
            records = other.records;
            returns = other.returns;
            return *this;
        }

        bool operator==(const ObjectType& other){
            return !(*this != other);
        }

        bool operator!=(const ObjectType& other){
            if(typeTag != other.typeTag) return true;
            if(params.size() != other.params.size()) return true;
            if(returns.size() != other.returns.size()) return true;
            if(records.size() != other.records.size()) return true;
            if(typeTag == MyTypeTag::SumTypeTag){
                return params[0] != other.params[0] && params[1] != other.params[1];
            }
            for(int i = 0; i < records.size(); i++){
                if(records[i].first != other.records[i].first || records[i].second != other.records[i].second){
                    return true;
                }
            }
            for(int i = 0; i < params.size(); i++){
                if(params[i] != other.params[i]){
                    return true;
                }
            }
            for(int i = 0; i < returns.size(); i++){
                if(returns[i] != other.returns[i]){
                    return true;
                }
            }
            return false;
        }

        void printObject(){
            std::cout << "Print Object:\n";
            std::cout << "TypeTag: " << typeTag << "\n";
            std::cout << "Params: " << params.size() << "\n";
            for(int i = 0; i < params.size(); i++){
                params[i].printObject();
                std::cout << "\n";
            }
            std::cout << "Returns: " << returns.size() << "\n";
            for(int i = 0; i < returns.size(); i++){
                returns[i].printObject();
                std::cout << "\n";
            }
            std::cout << "END Printing\n\n";
        }

    };
}