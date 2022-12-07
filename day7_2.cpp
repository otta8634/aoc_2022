#include <iostream>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

using namespace std;

#ifdef gnu
[[using gnu : pure]]
#endif
static inline int svtoi(const string_view& str_view, size_t* idx = 0, int base = 10) {
	return stoi(static_cast<string>(str_view), idx, base);
}

struct Dir {
	vector<struct Dir*> sub_dirs;
	struct Dir* parent_dir;
	uint32_t size;
	string name;

	Dir(struct Dir* _parent_dir, string _name) : parent_dir(_parent_dir), size(0), name(_name) {}
};

static void dfs_update_size(struct Dir* dir) {
	for(struct Dir* sub_dir : dir->sub_dirs)
		dfs_update_size(sub_dir);

	if(dir->parent_dir != nullptr)
		dir->parent_dir->size += dir->size;
}

static void dfs_find_size(struct Dir* dir, uint32_t& smallest, const uint32_t required) {
	for(struct Dir* sub_dir : dir->sub_dirs)
		dfs_find_size(sub_dir, smallest, required);

	if(dir->size >= required && dir->size < smallest)
		smallest = dir->size;
}

int main(void) {
	constexpr uint32_t total_size = 70'000'000,
	                   required_size = 30'000'000;
	string line;
	string_view line_view;
	struct Dir root = Dir(nullptr, "/");
	struct Dir* cur_dir;

	// Ignore first line to cd into /
	while(getline(cin, line)) {
		if(line[0] == '$') {

			if(line[2] == 'c') {
				if(line[5] == '.') { // Go back
					cur_dir = cur_dir->parent_dir;
					assert(cur_dir != nullptr);
				} else if(line[5] == '/')
					cur_dir = &root;
				else {
					const string new_dir_name = line.substr(5);
					for(struct Dir* sub_dir : cur_dir->sub_dirs)
						if(sub_dir->name == new_dir_name) {
							cur_dir = sub_dir;
							goto repeat;
						}
					exit(EXIT_FAILURE);
				}
			}

		} else {
			if(line[0] == 'd') // New dir
				cur_dir->sub_dirs.push_back(new struct Dir(cur_dir, line.substr(4)));
			else { // File size
				line_view = line;
				string::size_type i = 0;
				while(line_view[i] >= '0' && line_view[i] <= '9')
					++i;
				assert(i);
				cur_dir->size += svtoi(line_view.substr(0, i + 1));
			}
		}
repeat:
		;
	}

	dfs_update_size(&root);

	assert(total_size > root.size);
	const uint32_t unused_size = total_size - root.size;
	assert(required_size > unused_size);
	const uint32_t ntf_size = required_size - unused_size; // Need to free size

	uint32_t smallest = root.size;
	dfs_find_size(&root, smallest, ntf_size);

	cout << smallest << '\n';
	return 0;
}