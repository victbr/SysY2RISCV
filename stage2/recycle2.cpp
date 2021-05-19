#include "utils2.hpp"

namespace stage2{

void Recycle(TreeNode* ptr){
	for (auto it: ptr->SonNode){
		Recycle(it.first);	
	}
	delete ptr;
}

}
