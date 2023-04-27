#pragma once
#include "../utils/matrix.hpp"

class Ray{
    public:
        Ray(const gl::vec3& origin,const gl::vec3& direction):origin(origin),direction(direction){};
        ~Ray()=default;

        void setOrigin(const gl::vec3& origin){
            this->origin=origin;
        };

        void setDirection(const gl::vec3& direction){
            this->direction=direction;
        };

        gl::vec3 getOrigin() const{
            return this->origin;
        };

        gl::vec3 getDirection() const{
            return this->direction;
        };
    private:
        gl::vec3 origin;
        gl::vec3 direction;
};