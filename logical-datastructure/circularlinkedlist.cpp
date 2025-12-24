#include "Circularlinkedlist.h"
#include <memory>

CircularLinkedList::CircularLinkedList() {
	dummy_ = new Node();
	std::memset(dummy_->data_.name_, 0, MAX_NAME_SIZE);
	dummy_->next_ = dummy_;
	dummy_->prev_ = dummy_;
}

CircularLinkedList::~CircularLinkedList() {
	Node* temp = dummy_->next_;
	while (temp != dummy_) {
		Node* old = temp;
		temp = temp->next_;
		delete old;
	}
	delete dummy_;
	dummy_ = nullptr;
}

void CircularLinkedList::add_node_back(Data* new_data) {
	Node* new_node = new Node();
	std::strncpy(new_node->data_.name_, new_data->name_, MAX_NAME_SIZE);
	new_node->data_.name_[MAX_NAME_SIZE - 1] = '\0';
	Node* temp = dummy_->prev_;
	new_node->next_ = dummy_;
	new_node->prev_ = temp;
	temp->next_ = new_node;
	dummy_->prev_ = new_node;
}

void CircularLinkedList::add_node_front(Data* new_data) {
	Node* new_node = new Node();
	std::strncpy(new_node->data_.name_, new_data->name_, MAX_NAME_SIZE);
	new_node->data_.name_[MAX_NAME_SIZE - 1] = '\0';
	Node* temp = dummy_->next_;
	new_node->next_ = temp;
	new_node->prev_ = dummy_;
	temp->prev_ = new_node;
	dummy_->next_ = new_node;
}

bool CircularLinkedList::add_node_after(Data* new_data, Data* target_data) {
	Node* temp = dummy_->next_;
	while (temp != dummy_) {
		if (strcmp(target_data->name_, temp->data_.name_) == 0) {
			Node* target_node = temp;
			Node* new_node = new Node();
			std::strncpy(new_node->data_.name_, new_data->name_, MAX_NAME_SIZE);
			new_node->data_.name_[MAX_NAME_SIZE - 1] = '\0';
			new_node->prev_ = target_node;
			new_node->next_ = target_node->next_;
			target_node->next_->prev_ = new_node;
			target_node->next_ = new_node;
			return true;
		}
		else temp = temp->next_;
	}
	return false;
}

bool CircularLinkedList::add_node_before(Data* new_data, Data* target_data) {
	Node* temp = dummy_->next_;
	while (temp != dummy_) {
		if (strcmp(target_data->name_, temp->data_.name_) == 0) {
			Node* target_node = temp;
			Node* new_node = new Node();
			std::strncpy(new_node->data_.name_, new_data->name_, MAX_NAME_SIZE);
			new_node->data_.name_[MAX_NAME_SIZE - 1] = '\0';
			new_node->prev_ = target_node->prev_;
			new_node->next_ = target_node;
			target_node->prev_->next_ = new_node;
			target_node->prev_ = new_node;
			return true;
		}
		else temp = temp->next_;
	}
	return false;
}

bool CircularLinkedList::del_node(Data* target_data) {
	Node* temp = dummy_->next_;
	while (temp != dummy_) {
		if (strcmp(target_data->name_, temp->data_.name_)==0) {
			Node* target_node = temp;
			temp = temp->next_;
			target_node->prev_->next_ = target_node->next_;
			target_node->next_->prev_ = target_node->prev_;
			delete target_node;
			return true;
		}
		else temp = temp->next_;
	}
	return false;
}

bool CircularLinkedList::del_node(Node* target_node) {
	if (target_node == dummy_)
		return false;
	else {
		target_node->prev_->next_ = target_node->next_;
		target_node->next_->prev_ = target_node->prev_;
		delete target_node;
		return true;
	}
}

void CircularLinkedList::clear_list() {
	Node* temp = dummy_->next_;
	while (temp != dummy_) {
		Node* old = temp;
		temp = temp->next_;
		delete old;
	}
	dummy_->next_ = dummy_;
	dummy_->prev_ = dummy_;
}

Node* CircularLinkedList::search_node(Data* target_data) {
	Node* temp = dummy_->next_;
	while (temp != dummy_) {
		if (strcmp(target_data->name_, temp->data_.name_) == 0)
			return temp;
		else temp = temp->next_;
	}
	return nullptr;
}

Node* CircularLinkedList::get_dummy() {
	return dummy_;
}


