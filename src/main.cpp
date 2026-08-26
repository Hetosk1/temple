
#include <iostream>
#include "KVStore.hpp"

int main(){

	KVStore kv = KVStore();

	kv.put("het", "backend");
	kv.put("dev", "frontend");
	kv.put("nihil", "db");

	std::cout << kv.get("het") << "\n";
	std::cout << kv.get("dev") << "\n";
	std::cout << kv.get("nihil") << "\n";

	std::cout << "deleted nihil: " << int(kv.delete_key("nihil")) << "\n";

	std::cout << "exists nihil: " << int(kv.exists("nihil")) << "\n";

	return 0;
}
