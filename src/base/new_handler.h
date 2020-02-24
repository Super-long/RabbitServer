/**
 * Copyright lizhaolong(https://github.com/Super-long)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef NEW_HANDLER_H_
#define NEW_HANDLER_H_

#include <new>

//typedef void (*new_handler)();

class NewhSupport{
    public:
        explicit NewhSupport(std::new_handler nh) : nh_(nh) {}
        ~NewhSupport(){
            std::set_new_handler(nh_);
        }
    private:
        NewhSupport(const NewhSupport&);
        NewhSupport& operator=(const NewhSupport&);
        NewhSupport& operator&(const NewhSupport&);
        std::new_handler nh_;
};

template<typename Type>
class Base_Newhandler{
    public:
        static std::new_handler Set_new_handler(std::new_handler) throw();

        template<typename... Args>
        static void* operator new(size_t s, const Args&&... args){
            NewhSupport Temp(std::set_new_handler(Current_Hander_));
            return ::operator new(s, std::forward<Args>(args)...); //restore global new-handler   
        }
    private:
        static std::new_handler Current_Hander_;
};

template<typename Type>
std::new_handler Base_Newhandler<Type>::Set_new_handler(std::new_handler nh) throw() {
    std::new_handler oh = Current_Hander_;
    Current_Hander_ = nh;
    return oh;
}

template<typename Type>
std::new_handler Base_Newhandler<Type>::Current_Hander_ = nullptr;

//CRTP

/*
class Demo public : Base_Newhandler<Demo>{

} 
*/


#endif