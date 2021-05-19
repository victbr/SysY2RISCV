#include "utils.hpp"

void Recycle(TreeNode* ptr){
	for (auto it: ptr->SonNode){
		Recycle(it.first);	
	}
	delete ptr;
}
