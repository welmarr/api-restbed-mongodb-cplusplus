#include "UserRepository.hpp"
#include "../../utility/UUID.hpp"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/exception/operation_exception.hpp>

namespace App
{
    namespace Data
    {

        namespace Database
        {
            oatpp::Object<App::Data::Database::Model::User> UserRepository::userFromJson(const oatpp::Object<App::Data::Json::JsonUser> &dto)
            {
                auto user = App::Data::Database::Model::User::createShared();
                user->_id = App::Utility::UUID::generate_uuid_v4();
                user->username = dto->username;
                user->active = dto->active;
                user->role = dto->role;
                user->created_at = dto->created_at;
                return user;
            }

            oatpp::Object<App::Data::Json::JsonUser> UserRepository::jsonFromUser(const oatpp::Object<App::Data::Database::Model::User> &user)
            {
                auto dto = App::Data::Json::JsonUser::createShared();
                dto->username = user->username;
                dto->active = user->active;
                dto->role = user->role;
                dto->created_at = user->created_at;
                return dto;
            }

            UserRepository::UserRepository(const std::string &cName): collectionName(cName)
            {
            }

            bool UserRepository::deleteOne(const oatpp::String &key)
            {
                auto connacquire = connexxion->getPool()->acquire();
                auto collection = (*connacquire)[connexxion->getDatabaseName()][collectionName];

                auto result =
                    collection.delete_one(connexxion->createMongoDocument(
                        oatpp::Fields<oatpp::String>({{"_id", key}})));

                if (result)
                {
                    return result->deleted_count() == 1;
                }
                return false;
            }

            oatpp::Object<App::Data::Json::JsonUser> UserRepository::create(const oatpp::Object<App::Data::Json::JsonUser> &userDto)
            {
                
                std::cout << "Log : " << "UserRepository::create" << " [" << connexxion->getDatabaseName() << " : " << collectionName << "]" << std::endl;

                auto connacquire = connexxion->getPool()->acquire();
                auto collection = (*connacquire)[connexxion->getDatabaseName()][collectionName];

                collection.insert_one(connexxion->createMongoDocument(userFromJson(userDto)));

                return userDto;
            }

            oatpp::Object<App::Data::Json::JsonUser> UserRepository::update(const oatpp::Object<App::Data::Json::JsonUser> &userDto)
            {
                return oatpp::Object<App::Data::Json::JsonUser>();
            }

            oatpp::Object<App::Data::Json::JsonUser> UserRepository::getOne(const oatpp::String &key)
            {
                auto connacquire = connexxion->getPool()->acquire();
                auto collection = (*connacquire)[connexxion->getDatabaseName()][collectionName];

                auto result = collection.find_one(connexxion->createMongoDocument(oatpp::Fields<oatpp::String>({{"_id", key}})));

                if (result)
                {
                    auto view = result->view();
                    auto bson = oatpp::String((const char *)view.data(), view.length());
                    auto user = (connexxion->getObjectMapper()).readFromString<oatpp::Object<App::Data::Database::Model::User>>(bson);
                    return jsonFromUser(user);
                }

                return nullptr;
            }

            oatpp::List<oatpp::Object<App::Data::Json::JsonUser>> UserRepository::getAll()
            {
                return oatpp::List<oatpp::Object<App::Data::Json::JsonUser>>();
            }
        }
    }
}