#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <string>

class KVStore {

	private:
		std::unordered_map<std::string, std::string> data;

	public:

		void put(const std::string &key, const std::string &value){
			data[key] = value;
		}

		std::string get(const std::string &key){
			auto exists = data.find(key);
			if(exists != data.end()) {
				return exists->second;
			}
			throw std::runtime_error("Key not found");
		}

		bool delete_key(const std::string &key){
			return data.erase(key) == 1;
		}


		bool exists(const std::string &key){
			return data.find(key) != data.end();
		}

};


